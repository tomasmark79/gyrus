#!/usr/bin/env python3
"""Replay isolated GTIA contact/calibration fixtures in Atari800's monitor.

Requires Atari800 and the locally captured analysis/game-start.a8s. Does not
modify that state file. No emulator is used by the C++ runtime.
"""
import argparse
import os
import pty
import re
import select
import subprocess
import time


def program(prior):
    code = [0x78]  # SEI
    def lda(value): code.extend((0xa9, value))
    def sta(address): code.extend((0x8d, address & 255, address >> 8))
    lda(0)
    for address in [0xd40e, 0xd20e, *range(0xd001, 0xd00d)]: sta(address)
    for address, value in [(0xd400, 0x2e), (0xd407, 0x20),
                           (0xd01d, 3), (0xd01b, prior), (0xd402, 0),
                           (0xd403, 4), (0xd000, 88), (0xd001, 90),
                           (0xd002, 92)]:
        lda(value)
        sta(address)
    code.extend((0xad, 0x0b, 0xd4, 0xd0, 0xfb))  # wait frame boundary
    lda(0)
    sta(0xd01e)
    code.extend((0xad, 0x0b, 0xd4, 0xf0, 0xfb))
    code.extend((0xad, 0x0b, 0xd4, 0xd0, 0xfb))
    for channel in range(3):
        code.extend((0xad, 4 + channel, 0xd0))
        sta(0x7f00 + channel)
    end = 0x7000 + len(code)
    code.extend((0x4c, end & 255, end >> 8))
    return code, end


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--state', default='analysis/game-start.a8s')
    parser.add_argument('--emulator', default='atari800')
    args = parser.parse_args()
    master, slave = pty.openpty()
    env = dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
    process = subprocess.Popen([args.emulator, '-xl', '-nobasic', '-nosound',
        '-no-autosave-config', '-state', args.state, '-monitor'],
        stdin=slave, stdout=slave, stderr=slave, env=env)
    os.close(slave)
    def receive():
        data = ''
        deadline = time.monotonic() + 15
        while time.monotonic() < deadline:
            ready, _, _ = select.select([master], [], [], 0.1)
            if ready:
                data += os.read(master, 65536).decode(errors='replace')
                clean = re.sub(r'\x1b\[[0-9;?]*[A-Za-z]', '', data)
                if clean.endswith('> '): return clean
        raise RuntimeError('Monitor timeout: ' + data[-1000:])
    def command(value):
        os.write(master, (value + '\n').encode())
        return receive()
    try:
        receive()
        for value in ['f 2180 23ff 00', 'f 30c4 3feb 00',
                      'f 0400 046a 0d', 'c 0400 70 70 70 46 d8 3f 4d c4 30',
                      'c 0467 0d 41 00 04', 'c 2238 80']:
            command(value)
        for prior in (0x11, 0x04):
            code, end = program(prior)
            for offset in range(0, len(code), 16):
                command(f'c {0x7000 + offset:04x} ' +
                        ' '.join(f'{b:02x}' for b in code[offset:offset + 16]))
            command(f'bpc {end:04x}')
            cases = [(40, [1, 0, 0], [0, 0, 0])]
            cases += [(36, [i, 0, 0], [0 if i == 0 else 1 << (i-1), 0, 0])
                      for i in range(4)]
            cases += [(36, [1, 2, 3], [1, 2, 4])]
            for row, indices, expected in cases:
                command('f 30c4 3feb 00')
                combined = indices == [1, 2, 3]
                command(f'c 22b8 {128 if combined else 0:02x}')
                command(f'c 2338 {128 if combined else 0:02x}')
                address = 0x30c4 + row * 40 + 10
                command(f'c {address:04x} {indices[0]*64+indices[1]*4:02x} {indices[2]*64:02x}')
                command('cont 7000')
                result = command('m 7f00')
                match = re.search(r'7F00:\s+([0-9A-F]{2})\s+([0-9A-F]{2})\s+([0-9A-F]{2})', result)
                if not match: raise RuntimeError(result)
                actual = [int(v, 16) for v in match.groups()]
                assert actual == expected, (prior, row, indices, actual, expected)
                print(f'PRIOR={prior:02X} y={row} PF={indices}: PnPF={actual}')
    finally:
        process.terminate()
        try:
            process.wait(timeout=1)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait(timeout=5)
        os.close(master)


if __name__ == '__main__':
    main()
