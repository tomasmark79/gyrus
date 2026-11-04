#!/usr/bin/env python3
"""Decode Gyruss graphics from a RAM dump taken after startup at $8069.

The output uses indices, not an assumed RGB Atari palette. The SVG uses a
diagnostic palette for visual identification only. No game CPU code is run.
"""
import argparse
import hashlib
import json
from pathlib import Path


def unpack(data):
    return [(value >> shift) & 3 for value in data for shift in (6, 4, 2, 0)]


def decode(memory):
    def word(address):
        return memory[address] | memory[address + 1] << 8

    variants = []
    for kind in range(29):
        metadata = word(0xac95 + kind * 2)
        for depth in range(8):
            packed_sizes = memory[metadata + 1 + depth // 2]
            size = (packed_sizes >> (0 if depth & 1 else 4)) & 15
            stride = (size + 6) // 4
            for angle in range(16):
                bank = word(0xaccf + kind * 2 + (0x3a if (size // 2) & 2 else 0))
                source = word(bank + ((angle >> 1) & 6))
                source += memory[metadata + 5 + (angle & 3)]
                source += memory[metadata + 8 + size]
                pixels = []
                for row in range(size):
                    pixels.extend(unpack(memory[source + row * stride:source + (row + 1) * stride]))
                variants.append(dict(kind=kind, depth=depth, orientation=angle,
                                     metadata=metadata, source=source,
                                     width=stride * 4, height=size, pixels=pixels))
    title = dict(source=0xa909, width=104, height=34,
                 pixels=unpack(memory[0xa909:0xa909 + 26 * 34]))
    font = []
    for code in range(64):
        bitmap = memory[0x1e00 + code * 8:0x1e08 + code * 8]
        font.append(dict(code=code, width=8, height=8,
                         pixels=[(byte >> bit) & 1 for byte in bitmap for bit in range(7, -1, -1)]))
    return dict(title=title, font=font, sprites=variants)


def svg(atlas):
    colors = ('#080c14', '#4f8ce0', '#efab45', '#fa5b76')
    out = ['<svg xmlns="http://www.w3.org/2000/svg" width="1044" height="720" viewBox="0 0 522 360">',
           '<rect width="522" height="360" fill="#080c14"/>',
           '<g fill="white" font-family="monospace" font-size="7">',
           '<text x="8" y="10">GYRUSS - recovered packed sprites (diagnostic colours)</text></g>']

    def draw(image, x, y, scale=1):
        width = image['width']
        for i, pixel in enumerate(image['pixels']):
            if pixel:
                out.append(f'<rect x="{x + i % width * scale}" y="{y + i // width * scale}" width="{scale}" height="{scale}" fill="{colors[pixel]}"/>')

    draw(atlas['title'], 8, 20, 2)
    for kind in range(29):
        x = 8 + (kind % 15) * 34
        y = 110 + (kind // 15) * 120
        out.append(f'<text x="{x}" y="{y}" fill="white" font-family="monospace" font-size="7">{kind:02d}</text>')
        for depth in range(8):
            draw(atlas['sprites'][(kind * 8 + depth) * 16], x, y + 4 + depth * 12)
    out.append('</svg>')
    return '\n'.join(out)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('memory', type=Path)
    parser.add_argument('--output', type=Path, default=Path('analysis'))
    args = parser.parse_args()
    memory = args.memory.read_bytes()
    if len(memory) != 65536:
        raise ValueError('Expected a complete 64KB memory dump')
    atlas = decode(memory)
    atlas['memory_sha256'] = hashlib.sha256(memory).hexdigest()
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / 'graphics.json').write_text(json.dumps(atlas, separators=(',', ':')) + '\n')
    (args.output / 'graphics-atlas.svg').write_text(svg(atlas))
    unique = {tuple(v['pixels']) for v in atlas['sprites']}
    print(f"Decoded title, 64 font glyphs, {len(atlas['sprites'])} sprite variants ({len(unique)} unique pixel sequences)")
