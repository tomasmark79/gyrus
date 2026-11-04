#!/usr/bin/env python3
"""Differential tests for the typed player/shot model against original 6502 code.

Requires py65==1.2.0 and analysis/initialized-memory.bin. Bitmap raster writes and
sound requests are intercepted; original shot projection/size/clipping executes.
"""
import argparse
from pathlib import Path
import random
import subprocess
import tempfile

from py65.devices.mpu6502 import MPU


RUNNER = r'''
#include <Game/Player.hpp>
#include <iostream>
using namespace gyrus::game;
int main() {
  char mode;
  while (std::cin >> mode) {
    PlayerState p;
    PlayerGates g;
    unsigned a,b,c,d,e,f,h,i,j;
    if (mode == 'I') {
      std::cin >> a >> b >> c >> d;
      p.input.pendingFire=c; p.input.triggerLatched=d;
      sampleInput(p,a,b);
      std::cout << +p.input.joystick << ' ' << +p.input.pendingFire << ' '
                << p.input.triggerLatched;
    } else if (mode == 'M') {
      std::cin >> a >> b >> c >> d >> e >> f >> h >> i;
      p.angle=a; p.depth=b; p.aimAngle=c; p.input.joystick=d;
      g.deathCountdown=f; g.transition=h; g.explosion=i;
      updatePlayerMovement(p,e,g);
      std::cout << +p.angle << ' ' << +p.depth << ' ' << +p.aimAngle << ' '
                << +g.transition;
    } else {
      std::cin >> a >> b >> c >> d >> e >> f >> h >> i;
      p.angle=a; p.depth=b; p.input.pendingFire=c;
      g.deathCountdown=d; g.transition=e; g.messageCountdown=f;
      g.dualShot=h; g.suppressFireSound=i;
      for(auto& s:p.shots) {
        std::cin >> a >> b >> c >> d >> e;
        s.angle=a; s.depth=b; s.active=c!=0; s.hit=c&128;
        s.width=c&127; s.position={static_cast<uint8_t>(d),static_cast<uint8_t>(e),false};
      }
      const auto event=updatePlayerShots(p,g);
      std::cout << +p.input.pendingFire << ' ' << event.fireSound << ' ' << +event.spawnedMask;
      for(const auto& s:p.shots)
        std::cout << ' ' << +s.angle << ' ' << +s.depth << ' '
                  << (s.active ? s.width | (s.hit?128:0) : 0) << ' '
                  << +s.position.x << ' ' << +s.position.y;
    }
    std::cout << '\n';
  }
}
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--memory', type=Path, default=Path('analysis/initialized-memory.bin'))
    parser.add_argument('--compiler', default='c++')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    base = list(args.memory.read_bytes())
    if len(base) != 65536:
        raise ValueError('Expected full initialized address space')
    rng = random.Random(0x8956)
    cases = []
    for nibble in range(16):
        for pressed in range(2):
            for pending in [0, 1, 255]:
                for latched in range(2):
                    cases.append(('I', [nibble, pressed, pending, latched]))
    for angle in range(128):
        for nibble in range(16):
            for tick in [1, 2, 8]:
                cases.append(('M', [angle, 12, 77, nibble, tick, 0, 0, 0]))
    for transition in range(256):
        for tick in [7, 8]:
            for explosion in [0, 1]:
                cases.append(('M', [35, 12, 77, 6, tick, 0, transition, explosion]))
    cases.append(('M', [35, 12, 77, 6, 8, 1, 22, 0]))
    for angle in range(128):
        for dual in range(2):
            cases.append(('S', [angle, 12, 1, 0, 0, 0, dual, 0] + [0] * 30))
    for _ in range(1200):
        header = [rng.randrange(128), rng.choice([12, 20, 28, 36]),
                  rng.choice([0, 1, 2]), rng.choice([0, 0, 1]),
                  rng.choice([0, 0, 16]), rng.choice([0, 0, 1]),
                  rng.randrange(2), rng.randrange(2)]
        slots = []
        for _ in range(6):
            angle = rng.randrange(128) & 126
            depth = rng.choice([12, 20, 24, 32, 40, 52, 54])
            width = 3 if depth < 24 else 2 if depth < 32 else 1
            state = rng.choice([0, width, width | 128])
            slots.extend([angle, depth, state, 80, 48])
        cases.append(('S', header + slots))

    def invoke(cpu, address, stop=0x7fff, hooks=None):
        cpu.pc, cpu.sp, cpu.p = address, 0xfd, 0x30
        cpu.memory[0x1fe], cpu.memory[0x1ff] = 0xfe, 0x7f
        for _ in range(20000):
            if cpu.pc == stop:
                return
            if hooks and cpu.pc in hooks:
                hooks[cpu.pc](cpu)
            else:
                cpu.step()
        raise RuntimeError(f'Original ${address:04X} failed to return at ${cpu.pc:04X}')

    def return_from_call(cpu):
        cpu.pc = (cpu.stPopWord() + 1) & 65535

    def original(mode, v):
        cpu = MPU(memory=base.copy())
        m = cpu.memory
        if mode == 'I':
            nibble, pressed, pending, latched = v
            m[0x92], m[0xd300], m[0xd010] = 0, nibble, int(not pressed)
            m[0x90], m[0xed] = pending, latched
            invoke(cpu, 0x9a3d, 0x9a5e)
            return [m[0x8f], m[0x90], m[0xed]]
        if mode == 'M':
            angle, depth, aim, nibble, tick, death, transition, explosion = v
            m[0x51e], m[0x576], m[0xb2], m[0x8f] = angle, depth, aim, nibble
            m[0x55], m[0xac], m[0xb8], m[0xd8] = tick, death, transition, explosion
            # The caller skips $8B6B while $AC is nonzero ($855F..$85C5).
            if not death:
                invoke(cpu, 0x8b6b, hooks={0x9c14: return_from_call})
            return [m[0x51e], m[0x576], m[0xb2], m[0xb8]]
        angle, depth, pending, death, transition, message, dual, suppress = v[:8]
        m[0x51e], m[0x576], m[0x90] = angle, depth, pending
        m[0xac], m[0xb8], m[0x9a], m[0x94], m[0xd9], m[0x92] = death, transition, message, dual, suppress, 0
        for index in range(1, 7):
            a, d, state, x, y = v[8 + (index-1)*5:13 + (index-1)*5]
            m[0x51e+index], m[0x576+index], m[0x5ce+index] = a, d, state
            m[0x46e+index], m[0x4c6+index] = x, y
        sounds, spawned = [], []

        def sound(cpu):
            sounds.append(cpu.y)
            return_from_call(cpu)

        def raster(cpu):
            cpu.p &= ~1  # successful raster tail returns carry clear
            return_from_call(cpu)

        def spawning(cpu):
            spawned.append(cpu.x)
            cpu.step()

        invoke(cpu, 0x8956, hooks={0x9afb: return_from_call, 0x9b98: raster,
                                 0xb995: sound, 0x89ca: spawning, 0x89e4: spawning})
        mask = sum(1 << (index-1) for index in spawned if m[0x5ce+index])
        result = [m[0x90], int(3 in sounds), mask]
        for index in range(1, 7):
            result.extend([m[0x51e+index], m[0x576+index], m[0x5ce+index],
                           m[0x46e+index], m[0x4c6+index]])
        return result

    with tempfile.TemporaryDirectory(prefix='gyrus-player-oracle-') as temporary:
        folder = Path(temporary)
        source, binary = folder / 'runner.cpp', folder / 'runner'
        source.write_text(RUNNER)
        subprocess.run([args.compiler, '-std=c++20', '-O2', '-I' + str(root / 'src/lib'),
                        str(source), str(root / 'src/lib/Game/Player.cpp'),
                        str(root / 'src/lib/Game/Polar.cpp'), '-o', str(binary)], check=True)
        data = ''.join(mode + ' ' + ' '.join(map(str, values)) + '\n' for mode, values in cases)
        output = subprocess.check_output([str(binary)], input=data, text=True).splitlines()
    if len(output) != len(cases):
        raise AssertionError('Native result count mismatch')
    counts = dict(I=0, M=0, S=0)
    for (mode, values), result in zip(cases, output):
        expected, actual = original(mode, values), list(map(int, result.split()))
        if expected != actual:
            raise AssertionError(f'{mode} {values}\n6502: {expected}\nC++:  {actual}')
        counts[mode] += 1
    print(f'Player oracle passed: input={counts["I"]}, movement={counts["M"]}, shots={counts["S"]}; zero mismatches')


if __name__ == '__main__':
    main()
