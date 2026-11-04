#!/usr/bin/env python3
"""Compare native C++ polar math with original 6502 routines using py65 1.2.0.

Run extract.py first. py65 is an analysis-only dependency, never part of the game.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

from py65.devices.mpu6502 import MPU


RUNNER = r'''
#include <Game/Polar.hpp>
#include <iostream>
int main() {
  for (unsigned a = 0; a < 128; ++a)
    for (unsigned d = 0; d < 64; ++d) {
      auto p = gyrus::game::project(a, d);
      std::cout.put(p.x); std::cout.put(p.y); std::cout.put(p.clipped);
    }
  for (unsigned a = 0; a < 128; ++a)
    for (unsigned t = 0; t < 128; ++t)
      std::cout.put(static_cast<unsigned char>(gyrus::game::steeringDelta(a, t)));
}
'''


def compare(memory_path, compiler, root):
    memory = memory_path.read_bytes()
    if len(memory) != 65536:
        raise ValueError('Expected extracted 64KB address space')
    cpu = MPU(memory=list(memory))

    def original(address, accumulator, values):
        cpu.pc, cpu.a, cpu.x, cpu.y, cpu.sp, cpu.p = address, accumulator, 0, 0, 0xfd, 0x30
        # RTS returns to a sentinel outside the program.
        cpu.memory[0x1fe], cpu.memory[0x1ff] = 0xfe, 0x7f
        for key, value in values.items():
            cpu.memory[key] = value
        for _ in range(1000):
            cpu.step()
            if cpu.pc == 0x7fff:
                return cpu.a, cpu.y, cpu.p & 1
        raise RuntimeError(f'Original ${address:04X} did not return')

    with tempfile.TemporaryDirectory(prefix='gyrus-polar-oracle-') as temporary:
        folder = Path(temporary)
        source = folder / 'runner.cpp'
        binary = folder / 'runner'
        source.write_text(RUNNER)
        subprocess.run([compiler, '-std=c++20', '-O2', '-I' + str(root / 'src/lib'),
                        str(source), str(root / 'src/lib/Game/Polar.cpp'), '-o', str(binary)],
                       check=True)
        native = subprocess.check_output([str(binary)])
    if len(native) != 128 * 64 * 3 + 128 * 128:
        raise RuntimeError('Unexpected native oracle output size')
    offset = 0
    for angle in range(128):
        for depth in range(64):
            expected = original(0xa0cb, 0, {0x8b: angle, 0x8c: depth})
            actual = tuple(native[offset:offset + 3])
            if actual != expected:
                raise AssertionError(f'Projection ({angle}, {depth}): C++ {actual}, 6502 {expected}')
            offset += 3
    for angle in range(128):
        for target in range(128):
            expected = original(0xa0bd, angle, {0x4e: target})[0]
            if native[offset] != expected:
                raise AssertionError(f'Steering ({angle}, {target}): C++ {native[offset]}, 6502 {expected}')
            offset += 1
    return dict(projection_cases=8192, steering_cases=16384, mismatches=0,
                original_projection='$A0CB', original_steering='$A0BD',
                depth_domain=[0, 63], angle_domain=[0, 127])


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--memory', type=Path, default=Path('analysis/loaded-memory.bin'))
    parser.add_argument('--compiler', default='c++')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    print(json.dumps(compare(args.memory, args.compiler, root), indent=2))
