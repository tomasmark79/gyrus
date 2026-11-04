#!/usr/bin/env python3
"""Export original Gyruss scene data and fixed layout maps, without CPU code.

Font bytes are copied from the initialized emulator OS font, not the ATR.
RGB uses the explicitly identified classic Atari800 default.act profile.
"""
import argparse
import hashlib
from pathlib import Path
import urllib.request

PALETTE_URL = 'https://raw.githubusercontent.com/atari800/atari800/c3d8074628af3e6556a04228d0eb77d142fef195/act/default.act'
PALETTE_SHA = '9d3973cff27fa73e1a57301af404de6e49fc89b56cc9f9497806b10efccde1f1'


def layouts(memory):
    result = []
    text_kinds = {0x3fd8:256, 0x3fc4:264, 0x8e4e:272, 0x8000:280, 0x8014:288}
    for variant in range(4):
        data = bytearray(memory)
        data[0x445:0x44e] = memory[0x8e06 + variant * 9:0x8e0f + variant * 9]
        data[0x456:0x45f] = memory[0x8e2a + variant * 9:0x8e33 + variant * 9]
        pc, address = 0x403, 0
        rows = []
        for _ in range(200):
            opcode = data[pc]
            pc += 1
            mode = opcode & 15
            if mode == 1:
                if opcode & 64:
                    break
                pc = data[pc] | data[pc + 1] << 8
                continue
            if opcode & 64:
                address = data[pc] | data[pc + 1] << 8
                pc += 2
            if mode == 6:
                rows.extend(text_kinds[address] + i for i in range(8))
                address += 20
            elif mode == 13:
                assert (address - 0x30c4) % 40 == 0
                row = (address - 0x30c4) // 40
                assert 0 <= row < 96
                rows.extend([row, row])
                address += 40
            else:
                raise ValueError(f'Unexpected display mode {mode}')
        assert len(rows) in (184, 192, 200)
        result.extend(rows + [65535] * (200 - len(rows)))
    return result


def export(memory, destination, palette_path):
    word = lambda address: memory[address] | memory[address + 1] << 8
    planets, pixels, masks = [], [], []
    for index in range(6):
        record = 0x9f08 + index * 8
        source = word(record)
        height, width = memory[record + 5] >> 3, memory[record + 5] & 7
        fields = [len(pixels), width, height, memory[record + 7], memory[record + 6],
                  memory[record + 2] & 254, memory[record + 3] & 254, memory[record + 4] & 254]
        pixels.extend(memory[source + 4:source + 4 + width * height])
        for layer in range(2):
            pointer = word(source + layer * 2)
            count = memory[pointer]
            fields.extend([len(masks), count, memory[pointer + 1] & 254,
                           memory[pointer + 2], memory[pointer + 3]])
            masks.extend(memory[pointer + 4:pointer + 4 + count])
        planets.append('{' + ','.join(map(str, fields)) + '}')
    palette = palette_path.read_bytes() if palette_path else urllib.request.urlopen(PALETTE_URL).read()
    if hashlib.sha256(palette).hexdigest() != PALETTE_SHA:
        raise ValueError('Unexpected classic palette hash')
    font = memory[0x1e00:0x2000]
    out = ['// Generated data only; no original CPU instructions.',
           '// Original ATR SHA-256 dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc.',
           f'// Initialized RAM SHA-256 {hashlib.sha256(memory).hexdigest()}.',
           '// Font copied by original game from emulator OS; NOT part of ATR.',
           f'// Font SHA-256 {hashlib.sha256(font).hexdigest()}.',
           f'// Classic palette: {PALETTE_URL}', f'// Palette SHA-256 {PALETTE_SHA}.',
           '// This palette is not the current emulator generated PAL profile.',
           '#pragma once', '#include <array>', '#include <cstdint>',
           'namespace gyrus::game::scene_data {',
           'struct Planet {',
           '  std::uint16_t offset; std::uint8_t widthBytes, height, x, y, pf0, pf1, pf2;',
           '  std::uint16_t playerOffset; std::uint8_t playerRows, playerColor, playerX, playerY;',
           '  std::uint16_t missileOffset; std::uint8_t missileRows, missileColor, missileX, missileY;',
           '};']

    def array(name, kind, values):
        out.append(f'inline constexpr std::array<{kind}, {len(values)}> {name}{{{{')
        for start in range(0, len(values), 16):
            out.append('    ' + ', '.join(str(v) for v in values[start:start + 16]) + ',')
        out.append('}};')

    array('planets', 'Planet', planets)
    array('planetPixels', 'std::uint8_t', pixels)
    array('planetMasks', 'std::uint8_t', masks)
    array('scanlineLayouts', 'std::uint16_t', layouts(memory))
    array('font', 'std::uint8_t', font)
    array('copyrightText', 'std::uint8_t', memory[0x8e4e:0x8e62])
    array('perfectText', 'std::uint8_t', memory[0x8000:0x8028])
    array('messages', 'std::uint8_t', memory[0x998c:0x99f0])
    array('planetNames', 'std::uint8_t', memory[0x995c:0x998c])
    array('stagePalettes', 'std::uint8_t', memory[0x8dd7:0x8de3])
    array('classicPalette', 'std::uint8_t', palette)
    out.append('} // namespace gyrus::game::scene_data')
    destination.write_text('\n'.join(out) + '\n')
    print(f'Exported {len(pixels)} planet bytes, {len(masks)} overlay bytes, 512 OS-font bytes and four scanline layouts')


def fnv(values):
    result = 2166136261
    for value in values:
        result = ((result ^ value) * 16777619) & 0xffffffff
    return result


def oracle(memory):
    from py65.devices.mpu6502 import MPU

    def run(mem, entry, a=0, x=0):
        cpu = MPU(memory=mem, pc=entry)
        cpu.sp = 0xfd
        cpu.a, cpu.x = a, x
        mem[0x1fe], mem[0x1ff], mem[0xd40b] = 0xff, 0x6f, 0x80
        for _ in range(200000):
            if cpu.pc == 0x7000:
                return cpu
            cpu.step()
        raise RuntimeError('Original routine did not return')

    for title in (False, True):
        for planet in range(6):
            mem = list(memory)
            mem[0x30c4:0x3fc4] = [0] * 3840
            mem[0x57], mem[0x92], mem[0x9d] = int(not title), 0, planet * 8
            run(mem, 0x9f38)
            pixels = [(v >> bit) & 3 for v in mem[0x30c4:0x3fc4] for bit in (6,4,2,0)]
            print('planet', planet, 'title', title, 'hash', fnv(pixels))

    for title in (False, True):
        mem = list(memory)
        mem[0x3fd8:0x3fec] = [0] * 20
        mem[0x57], mem[0x92] = int(not title), 0
        mem[0x2110:0x211c] = [0,0,1,2,3,4,0,5,6,7,8,9]
        mem[0xaa], mem[0xa3] = 3, 1
        if not title:
            run(mem, 0x98bc)
            run(mem, 0x99f0)
        run(mem, 0x982f)
        if title:
            mem[0x92] = 1
            run(mem, 0x982f)
        print('hud title', title, bytes(mem[0x3fd8:0x3fec]).hex())

    for index in range(5):
        mem = list(memory)
        mem[0x92], mem[0x9b], mem[0x9d], mem[0xda] = 0, 3, 0, 28
        run(mem, 0x98de, x=index * 20)
        print('message', index, bytes(mem[0x3fc4:0x3fd8]).hex())


def starfield_oracle(memory):
    from py65.devices.mpu6502 import MPU

    class RandomMemory(list):
        seed = 0x12345678
        reads = 0

        def __getitem__(self, address):
            if address == 0xd20a:
                self.seed = (self.seed * 1664525 + 1013904223) & 0xffffffff
                self.reads += 1
                return self.seed >> 24
            return super().__getitem__(address)

    mem = RandomMemory(memory)
    mem[0x30c4:0x3fc4] = [0] * 3840
    mem[0x20e8:0x20fc] = [255] * 20
    frame_hashes, states = [], []
    for tick in range(300):
        mem[0x55] = tick & 255
        mem[0xb8] = int(120 <= tick < 180)
        mem[0xf9] = 2 if tick >= 200 else 0
        cpu = MPU(memory=mem, pc=0x8d1e)
        cpu.sp = 0xfd
        mem[0x1fe], mem[0x1ff] = 0xff, 0x6f
        for _ in range(100000):
            if cpu.pc == 0x7000:
                break
            cpu.step()
        else:
            raise RuntimeError('Original starfield routine did not return')
        pixels = [(v >> bit) & 3 for v in mem[0x30c4:0x3fc4] for bit in (6,4,2,0)]
        frame_hashes.extend(fnv(pixels).to_bytes(4, 'little'))
        for index in range(20):
            angle = mem[0x20e8 + index]
            states.extend([1,angle,mem[0x20fc + index] & 63] if angle < 128 else [0,0,0])
    print('starfield300 frame aggregate', fnv(frame_hashes), 'state aggregate', fnv(states), 'random reads', mem.reads)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('memory', type=Path)
    parser.add_argument('--output', type=Path, default=Path('src/lib/Game/SceneGraphicsData.hpp'))
    parser.add_argument('--palette', type=Path)
    parser.add_argument('--oracle', action='store_true')
    parser.add_argument('--starfield-oracle', action='store_true')
    args = parser.parse_args()
    memory = args.memory.read_bytes()
    if len(memory) != 65536:
        raise ValueError('Expected complete initialized RAM')
    if args.starfield_oracle:
        starfield_oracle(memory)
    elif args.oracle:
        oracle(memory)
    else:
        export(memory, args.output, args.palette)
