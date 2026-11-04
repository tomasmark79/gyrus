#!/usr/bin/env python3
"""Read-only title-mode runtime observations in an isolated Atari800 monitor."""
import os, pty, re, select, subprocess, time, hashlib
from pathlib import Path
from types import SimpleNamespace

def main():
    args = SimpleNamespace(emulator='atari800',state='analysis/title.a8s')
    master, slave = pty.openpty()
    env = dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
    process = subprocess.Popen([args.emulator, '-xl', '-nobasic', '-nosound',
        '-no-autosave-config', '-turbo', '-state', args.state, '-monitor'],
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
        command('bpc 8069')
        for frame in range(1801):
            if frame in (0,1,60,600,1800):
                path = f'/tmp/gyrus-title-review-{frame}.bin'
                command(f'write 0000 ffff {path}')
                memory = Path(path).read_bytes()
                print(dict(frame=frame, gameFlag=memory[0x57], frameCounter=memory[0x55],
                    music=memory[0x69], musicActive=memory[0x66],
                    stars=sum(v<128 for v in memory[0x20e8:0x20fc]),
                    bitmapSHA=hashlib.sha256(memory[0x30c4:0x3fc4]).hexdigest(),
                    hud=list(memory[0x3fd8:0x3fec])), flush=True)
                if frame == 60: print(command('pokey'),flush=True)
            if frame != 1800: command('cont')
    finally:
        process.kill()
        process.wait()
        os.close(master)

if __name__ == '__main__': main()
