#!/usr/bin/env python3
"""Compare clean C++ effects with original 6502 routines (py65==1.2.0).

This is an offline test oracle, never part of the game runtime. It models
register writes only, not POKEY synthesis or Atari interrupt timing.
"""
import argparse
import hashlib
import os
from pathlib import Path
import subprocess
import tempfile

from py65.devices.mpu6502 import MPU

ROOT = Path(__file__).resolve().parents[2]
ATR_SHA256 = 'dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc'
MIXED_STARTS = {0: 0, 3: 7, 10: 9, 20: 4, 40: 2, 45: 8,
                100: 1, 110: 5, 200: 6, 201: 3, 600: 2, 800: 8}


def original_memory(path):
    data = path.read_bytes()
    if hashlib.sha256(data).hexdigest() != ATR_SHA256:
        raise ValueError('Oracle requires the inspected GYRUSS.atr image')
    memory = [0] * 65536
    memory[0x8000:0xc000] = data[406:406 + 16384]
    return memory


def run_until(cpu, stop):
    for _ in range(10000):
        if cpu.pc == stop:
            return
        cpu.step()
    raise RuntimeError(f'Original audio routine did not return: PC={cpu.pc:04x}')


def original_trace(memory, scenario, frames):
    cpu = MPU(memory=memory.copy())
    result = bytearray()
    for frame in range(frames):
        effect = MIXED_STARTS.get(frame) if scenario == 'mixed' else (int(scenario) if frame == 0 else None)
        if effect is not None:
            cpu.pc, cpu.y, cpu.sp = 0xb995, effect, 0xfd
            cpu.memory[0x1fe:0x200] = [0xff, 0x6f]
            run_until(cpu, 0x7000)
        cpu.pc = 0xb9db
        cpu.memory[0x55] = frame & 255
        run_until(cpu, 0xbb18)
        for x in range(0, 8, 2):
            result.extend((cpu.memory[0xd200 + x], cpu.memory[0xd201 + x],
                           int(cpu.memory[0x72 + x] != 0)))
    return bytes(result)


def fnv1a(data):
    value = 14695981039346656037
    for byte in data:
        value = ((value ^ byte) * 1099511628211) & ((1 << 64) - 1)
    return value


PROBE = r'''
#include <Game/SoundEffects.hpp>
#include <cstdio>
#include <cstdlib>
#include <string_view>
int main(int argc, char** argv) {
    if (argc != 3) return 2;
    gyrus::game::SoundEffects effects;
    const bool mixed = std::string_view(argv[1]) == "mixed";
    constexpr int times[] = {0,3,10,20,40,45,100,110,200,201,600,800};
    constexpr int ids[] = {0,7,9,4,2,8,1,5,6,3,2,8};
    const int frames = std::atoi(argv[2]);
    for (int frame=0; frame<frames; ++frame) {
        if (mixed) {
            for (int i=0; i<12; ++i) if (frame==times[i]) effects.start(ids[i]);
        } else if (frame==0) effects.start(std::atoi(argv[1]));
        effects.tick(static_cast<unsigned char>(frame));
        for (const auto& c: effects.channels()) {
            std::putchar(c.frequency); std::putchar(c.control); std::putchar(c.active);
        }
    }
}
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--atr', type=Path, default=ROOT / 'GYRUSS.atr')
    parser.add_argument('--fixtures', action='store_true', help='Print independent trace hashes')
    parser.add_argument('--verify', action='store_true', help='Compile and compare actual C++ implementation')
    parser.add_argument('--cxx', default=os.environ.get('CXX', 'g++'))
    args = parser.parse_args()
    memory = original_memory(args.atr)
    cases = [(str(i), 1024) for i in range(10)] + [('mixed', 1024)]
    expected = {case: original_trace(memory, case[0], case[1]) for case in cases}
    if args.fixtures:
        for case, trace in expected.items():
            print(f'{case[0]:5s} {case[1]:4d} frames: 0x{fnv1a(trace):016x}ULL')
    if args.verify:
        with tempfile.TemporaryDirectory(prefix='gyrus-audio-oracle-') as tmp:
            probe = Path(tmp) / 'probe.cpp'
            binary = Path(tmp) / 'probe'
            probe.write_text(PROBE)
            subprocess.run([args.cxx, '-std=c++20', '-Wall', '-Wextra', '-Werror',
                            '-I', str(ROOT / 'src/lib'), str(probe),
                            str(ROOT / 'src/lib/Game/SoundEffects.cpp'), '-o', str(binary)], check=True)
            for (scenario, frames), reference in expected.items():
                actual = subprocess.check_output([str(binary), scenario, str(frames)])
                if actual != reference:
                    mismatch = next((i for i, (a, b) in enumerate(zip(actual, reference)) if a != b),
                                    min(len(actual), len(reference)))
                    frame, offset = divmod(mismatch, 12)
                    raise AssertionError(f'{scenario}: mismatch at frame {frame}, channel {offset // 3}, '
                                         f'field {offset % 3} (0=frequency, 1=control, 2=active); '
                                         f'C++={actual[mismatch:mismatch+1].hex()} '
                                         f'6502={reference[mismatch:mismatch+1].hex()}')
                print(f'PASS {scenario}: {frames} frames, all channel registers and active flags match')
            print(f'PASS total: {sum(frames for _, frames in cases)} frames; no POKEY synthesis claim')
    if not args.fixtures and not args.verify:
        parser.error('Select --fixtures and/or --verify')


if __name__ == '__main__':
    main()
