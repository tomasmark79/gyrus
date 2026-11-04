#!/usr/bin/env python3
"""Extract the file stream used by this ATR's observed three-sector loader.

Analysis tool only; the C++ game must not execute the original program.
Optional disassembly requires py65==1.2.0.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def extract(path, output):
    data = path.read_bytes()
    if data[:2] != b'\x96\x02':
        raise ValueError('Not an ATR image')
    sector_size = struct.unpack_from('<H', data, 4)[0]
    if sector_size != 128 or data[17] != 3:
        raise ValueError('This extractor only supports the observed 128-byte, three-sector loader')
    # $0709..$070B is the loader byte counter, decremented at $084F..$085F.
    stream_length = int.from_bytes(data[25:28], 'little')
    start = 16 + 3 * sector_size
    stream = data[start:start + stream_length]
    if len(stream) != stream_length or stream[:2] != b'\xff\xff':
        raise ValueError('Invalid or truncated executable stream')
    memory = bytearray(65536)
    segments = []
    cursor = 2
    while cursor < len(stream):
        if stream[cursor:cursor + 2] == b'\xff\xff':
            cursor += 2
            continue
        if cursor + 4 > len(stream):
            raise ValueError('Truncated segment header')
        low, high = struct.unpack_from('<HH', stream, cursor)
        cursor += 4
        length = high - low + 1
        if length <= 0 or cursor + length > len(stream):
            raise ValueError('Invalid segment size')
        payload = stream[cursor:cursor + length]
        memory[low:high + 1] = payload
        filename = f'segment-{low:04x}-{high:04x}.bin'
        (output / filename).write_bytes(payload)
        segments.append(dict(start=low, end=high, atr_offset=start + cursor,
                             file=filename, sha256=hashlib.sha256(payload).hexdigest()))
        cursor += length
    (output / 'loaded-memory.bin').write_bytes(memory)
    (output / 'gyruss.xex').write_bytes(stream)
    report = dict(source=str(path), sha256=hashlib.sha256(data).hexdigest(),
                  sector_size=sector_size, stream_length=stream_length,
                  run_address=int.from_bytes(memory[0x2e0:0x2e2], 'little'),
                  segments=segments)
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return memory, report


def disassemble(memory, output, entries):
    from py65.devices.mpu6502 import MPU
    from py65.disassembler import Disassembler
    cpu = MPU(memory=list(memory))
    dis = Disassembler(cpu)
    pending = list(entries)
    seen = {}
    calls = set()
    indirect = set()
    while pending:
        pc = pending.pop()
        while 0x8000 <= pc < 0xc000 and pc not in seen:
            length, text = dis.instruction_at(pc)
            op = memory[pc]
            seen[pc] = (length, text)
            target = memory[pc + 1] | memory[pc + 2] << 8
            if op == 0x20:
                pending.append(target)
                calls.add(target)
            elif op == 0x4c:
                pending.append(target)
                break
            elif op == 0x6c:
                indirect.add((pc, target))
                break
            elif op in (0x00, 0x40, 0x60):
                break
            elif op & 0x1f == 0x10:
                offset = memory[pc + 1]
                pending.append((pc + 2 + (offset if offset < 128 else offset - 256)) & 65535)
            pc += length
    lines = ['; Reachable static control flow. Indirect dispatch and interrupt roots may be missing.']
    for pc, (length, text) in sorted(seen.items()):
        raw = memory[pc:pc + length].hex(' ')
        lines.append(f'{pc:04X}: {raw:8s}  {text}')
    (output / 'reachable.asm').write_text('\n'.join(lines) + '\n')
    (output / 'control-flow.json').write_text(json.dumps(dict(
        instructions=len(seen), direct_calls=sorted(calls),
        indirect_jumps=[dict(address=a, pointer=p) for a, p in sorted(indirect)]), indent=2) + '\n')
    print(f'{len(seen)} reachable instructions, {len(calls)} call targets, {len(indirect)} indirect jumps')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('atr', type=Path)
    parser.add_argument('--output', type=Path, default=Path('analysis'))
    parser.add_argument('--disassemble', action='store_true')
    parser.add_argument('--root', action='append', type=lambda s: int(s, 16), default=[],
                        help='Additional observed callback entry (hexadecimal)')
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    memory, report = extract(args.atr, args.output)
    print(json.dumps(report, indent=2))
    if args.disassemble:
        disassemble(memory, args.output, [report['run_address'], *args.root])
