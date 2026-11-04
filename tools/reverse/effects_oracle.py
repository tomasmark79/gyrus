#!/usr/bin/env python3
"""Execute original player explosion initialization/update offline with py65."""
from pathlib import Path
from py65.devices.mpu6502 import MPU


def run(memory, entry, y=0, carry=False, stop=0x7000):
    cpu = MPU(memory=memory, pc=entry)
    cpu.sp, cpu.y = 0xfd, y
    cpu.p = (cpu.p & 254) | carry
    memory[0x1fe:0x200] = [0xff, 0x6f]
    for _ in range(100000):
        if cpu.pc == stop:
            return
        cpu.step()
    raise RuntimeError('Original routine did not return')


def main():
    original = list(Path('analysis/initialized-memory.bin').read_bytes())
    aggregate = 2166136261
    for angle in range(128):
        for x, y in [(80,48), (2,2), (155,91)]:
            memory = original.copy()
            memory[0x30c4:0x3fc4] = [0] * 3840
            memory[0x51e], memory[0x46e], memory[0x4c6] = angle, x, y
            memory[0xac] = 90
            run(memory, 0xa01d, 0x80)
            for frame in range(64):
                memory[0x55] = frame
                if memory[0xd8]:
                    run(memory, 0x8cc4)
                pixels = [(byte >> shift) & 3 for byte in memory[0x30c4:0x3fc4]
                          for shift in (6,4,2,0)]
                state = [memory[0xd8]]
                for slot in range(29,34):
                    state += [memory[0x46e+slot], memory[0x4c6+slot],
                              memory[0xb1+slot], memory[0xb6+slot]]
                for value in pixels + state:
                    aggregate = ((aggregate ^ value) * 16777619) & 0xffffffff
    print('384 explosions, 24576 frames, frame/state FNV:', aggregate)


def popups():
    original = list(Path('analysis/initialized-memory.bin').read_bytes())
    aggregate = 2166136261
    for angle in (0,15,32,63,96,127):
        for depth in (0,18,40,58):
            memory = original.copy()
            memory[0x30c4:0x3fc4] = [0] * 3840
            for slot in range(17,23):
                for base in (0x46e,0x4c6,0x51e,0x576,0x5ce,0x626,0x67e):
                    memory[base+slot] = 0
            memory[0x43:0x46] = [0]*3
            for tick in range(320):
                if tick in (0,1,2,3,160,161,162):
                    memory[0x4e], memory[0x4f] = angle, depth
                    run(memory, 0x8c91, (tick + angle // 16) % 8, bool(tick & 1))
                run(memory, 0x8c21)
                values = [(byte >> shift) & 3 for byte in memory[0x30c4:0x3fc4]
                          for shift in (6,4,2,0)]
                for slot in range(17,20):
                    values += [memory[0x5ce+slot],memory[0x32+slot],
                               memory[0x51e+slot],memory[0x576+slot],
                               memory[0x67e+slot]//2-21 if memory[0x67e+slot] else 0,
                               memory[0x46e+slot],memory[0x4c6+slot]]
                for value in values:
                    aggregate = ((aggregate ^ value) * 16777619) & 0xffffffff
    print('24 popup sequences, 7680 frames, frame/state FNV:', aggregate)


def hits():
    original = list(Path('analysis/initialized-memory.bin').read_bytes())
    aggregate = 2166136261
    for angle in (0,15,32,63,96,127):
        for depth in (18,28,40,58):
            memory = original.copy()
            memory[0x30c4:0x3fc4] = [0] * 3840
            for slot in range(12,17):
                for base in (0x46e,0x4c6,0x51e,0x576,0x5ce,0x626,0x67e,4,9):
                    memory[base+slot] = 0
                memory[slot-1] = 0
            for tick in range(100):
                if tick < 6 or 50 <= tick < 56:
                    memory[0x51e+39], memory[0x576+39] = angle, depth
                    memory[0x67e+39] = 2*((tick+angle//8)%14)
                    run(memory, 0x81b3, 39, stop=0x8217)
                memory[0x55] = tick
                run(memory, 0x86a9)
                values = [(byte >> shift) & 3 for byte in memory[0x30c4:0x3fc4]
                          for shift in (6,4,2,0)]
                for slot in range(12,17):
                    values += [memory[0x5ce+slot],memory[slot-1],
                               memory[0x51e+slot],memory[0x576+slot],memory[0x67e+slot]//2,
                               memory[4+slot],memory[9+slot],
                               memory[0x46e+slot],memory[0x4c6+slot]]
                for value in values:
                    aggregate = ((aggregate ^ value) * 16777619) & 0xffffffff
    print('24 hit sequences, 2400 frames, frame/state FNV:', aggregate)


if __name__ == '__main__':
    import sys
    if '--popups' in sys.argv: popups()
    elif '--hits' in sys.argv: hits()
    else: main()
