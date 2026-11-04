#!/usr/bin/env python3
"""Compare shared-register C++ music/effects output against the original VBI.

Requires py65==1.2.0. This checks frame-boundary sequencing, not synthesis.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile
from py65.devices.mpu6502 import MPU
from audio_oracle import ROOT, original_memory, run_until, fnv1a
from music_oracle import call

FRAMES = 16384


def trace(memory, track):
    cpu = MPU(memory=memory.copy())
    output = bytearray()
    for frame in range(FRAMES):
        if frame in (0, 8192):
            cpu.a = track
            call(cpu, 0xb901)
        if frame % 173 == 0 or frame % 271 == 1:
            cpu.y = (frame // 37) % 10
            call(cpu, 0xb995)
        if frame == 7777:
            call(cpu, 0xbc16)
        cpu.memory[0x55] = frame & 255
        # Deliberate paused frames retain actual registers and all effect state.
        cpu.memory[0x93] = int(frame % 997 >= 980)
        cpu.pc = 0xb9db
        run_until(cpu, 0xbb18)
        output.extend(cpu.memory[0xd200:0xd208])
        mask = sum((1 << c) for c in range(4) if cpu.memory[0x72 + c*2])
        output.extend((mask, int(bool(cpu.memory[0x66]))))
    return bytes(output)


PROBE = r'''
#include <Game/Audio.hpp>
#include <cstdio>
#include <cstdlib>
int main(int argc,char** argv) {
    if(argc!=2) return 2;
    gyrus::game::Audio audio;
    const auto track=static_cast<unsigned char>(std::atoi(argv[1]));
    for(unsigned f=0;f<16384;++f) {
        if(f==0 || f==8192) audio.startMusic(track);
        if(f%173==0 || f%271==1) audio.startEffect((f/37)%10);
        if(f==7777) audio.stopMusic();
        if(f%997<980) audio.tick(static_cast<unsigned char>(f));
        for(const auto& c:audio.registers()) { std::putchar(c.frequency); std::putchar(c.control); }
        std::putchar(audio.effectMask()); std::putchar(audio.musicPlaying());
    }
}
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixtures', action='store_true')
    parser.add_argument('--verify', action='store_true')
    parser.add_argument('--cxx', default='g++')
    args = parser.parse_args()
    memory = original_memory(ROOT / 'GYRUSS.atr')
    with tempfile.TemporaryDirectory(prefix='gyrus-combined-audio-') as tmp:
        binary = Path(tmp) / 'probe'
        if args.verify:
            source = Path(tmp) / 'probe.cpp'; source.write_text(PROBE)
            subprocess.run([args.cxx, '-std=c++20', '-Wall', '-Wextra', '-Werror',
                            '-I', str(ROOT / 'src/lib'), str(source),
                            *(str(ROOT / 'src/lib/Game' / file) for file in
                              ('Audio.cpp', 'Music.cpp', 'SoundEffects.cpp')),
                            '-o', str(binary)], check=True)
        for track in range(8):
            expected = trace(memory, track)
            if args.fixtures: print(f'{track}: 0x{fnv1a(expected):016x}ULL')
            if args.verify:
                actual = subprocess.check_output([str(binary), str(track)])
                if actual != expected:
                    i = next((i for i,(a,b) in enumerate(zip(actual, expected)) if a != b),
                             min(len(actual),len(expected)))
                    raise AssertionError(f'track={track} frame={i//10} field={i%10}: '
                                         f'C++={actual[i:i+1].hex()} original={expected[i:i+1].hex()}')
                print(f'PASS track {track}: {FRAMES} combined music/SFX frames')
        if args.verify: print(f'PASS total: {8*FRAMES} shared-register frames')
    if not args.verify and not args.fixtures: parser.error('Select --verify or --fixtures')


if __name__ == '__main__': main()
