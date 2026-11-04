#!/usr/bin/env python3
"""Reference-test the clean music sequencer against Gyruss's original 6502 code.

Requires py65==1.2.0. Music is sampled immediately before the original SFX
pass ($BB03), with explicit effect ownership flags supplied by the test.
This verifies sequencing/register state, not audio synthesis or CPU timing.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile

from py65.devices.mpu6502 import MPU
from audio_oracle import ROOT, original_memory, run_until, fnv1a

FRAMES = 16384
MIXED_STARTS = {0: 5, 41: 7, 999: 0, 1500: 1, 2000: 2,
                2500: 3, 6000: 4, 11000: 5, 25000: 6, 27000: 7}


def ownership(frame, masked):
    return (1 << ((frame // 37) % 4)) if masked and frame % 113 < 31 else 0


def call(cpu, address):
    cpu.pc, cpu.sp = address, 0xfd
    cpu.memory[0x1fe:0x200] = [0xff, 0x6f]
    run_until(cpu, 0x7000)


def start(cpu, track):
    for x in range(0, 8, 2):
        cpu.memory[0x72 + x] = 0
    cpu.a = track
    call(cpu, 0xb901)
    active = [cpu.memory[0x72 + x] for x in range(0, 8, 2)]
    return next((value - 1 for value in active if value), 255)


def original_trace(memory, scenario, frames, masked):
    cpu = MPU(memory=memory.copy())
    trace = bytearray()
    transitions = []
    last_mode = -1
    for frame in range(frames):
        requested = 255
        track = MIXED_STARTS.get(frame) if scenario == 'mixed' else (int(scenario) if frame == 0 else None)
        if track is not None:
            requested = start(cpu, track)
        mask = ownership(frame, masked)
        for channel in range(4):
            cpu.memory[0x72 + channel * 2] = int(bool(mask & (1 << channel)))
        if scenario == 'mixed' and frame == 2400:
            call(cpu, 0xbc16)
        cpu.pc = 0xb9db
        run_until(cpu, 0xbb03)
        for channel in range(4):
            trace.extend(cpu.memory[0xd200 + channel * 2:0xd202 + channel * 2])
        trace.extend((int(cpu.memory[0x66] != 0), requested))
        mode = cpu.memory[0x69]
        if mode != last_mode:
            transitions.append((frame, mode))
            last_mode = mode
    return bytes(trace), transitions


PROBE = r'''
#include <Game/Music.hpp>
#include <cstdio>
#include <cstdlib>
#include <string_view>
int main(int argc, char** argv) {
    if (argc != 4) return 2;
    gyrus::game::Music music;
    const bool mixed=std::string_view(argv[1])=="mixed";
    const int frames=std::atoi(argv[2]);
    const bool masked=std::atoi(argv[3]);
    constexpr int times[]={0,41,999,1500,2000,2500,6000,11000,25000,27000};
    constexpr int tracks[]={5,7,0,1,2,3,4,5,6,7};
    for(int frame=0;frame<frames;++frame) {
        if(mixed) {
            for(int i=0;i<10;++i) if(frame==times[i]) music.start(tracks[i]);
        } else if(frame==0) music.start(std::atoi(argv[1]));
        const auto requested=music.takeEffectRequest();
        const unsigned mask=masked && frame%113<31 ? 1U<<((frame/37)%4):0;
        if(mixed && frame==2400) music.stop(mask);
        music.tick(mask);
        for(const auto& c:music.channels()) { std::putchar(c.frequency); std::putchar(c.control); }
        std::putchar(music.isPlaying()); std::putchar(requested.value_or(255));
    }
}
'''


def print_data(memory):
    data = memory[0xbc35:0xc000]
    print('// Generated from the inspected GYRUSS.atr by tools/reverse/music_oracle.py --print-data.')
    print('// Original music streams $BC35..$BFFF; SHA-256 of source ATR:')
    print('// dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc')
    print('#pragma once\n#include <array>\n#include <cstdint>\n\nnamespace gyrus::game::detail {')
    print('inline constexpr std::uint16_t musicDataBase = 0xbc35;')
    print(f'inline constexpr std::array<std::uint8_t, {len(data)}> musicData{{{{')
    for offset in range(0, len(data), 16):
        print('    ' + ', '.join(f'0x{x:02x}' for x in data[offset:offset+16]) + ',')
    print('}};\n// Original $B955 pointer table, converted to stream offsets.')
    print('inline constexpr std::array<std::array<std::uint16_t, 4>, 8> trackStarts{{')
    for track in range(8):
        starts = [memory[0xb955 + track*8 + x] + 256*memory[0xb956 + track*8 + x] - 0xbc35
                  for x in range(0, 8, 2)]
        print('    {{' + ', '.join(str(x) for x in starts) + '}},')
    print('}};\n} // namespace gyrus::game::detail')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixtures',action='store_true')
    parser.add_argument('--verify',action='store_true')
    parser.add_argument('--print-data',action='store_true')
    parser.add_argument('--cxx',default='g++')
    args=parser.parse_args()
    memory=original_memory(ROOT/'GYRUSS.atr')
    if args.print_data:
        print_data(memory)
        return
    cases=[(str(track),FRAMES,masked) for masked in (False,True) for track in range(8)]
    cases.append(('mixed',32768,True))
    with tempfile.TemporaryDirectory(prefix='gyrus-music-oracle-') as tmp:
        binary=Path(tmp)/'probe'
        if args.verify:
            probe=Path(tmp)/'probe.cpp';probe.write_text(PROBE)
            subprocess.run([args.cxx,'-std=c++20','-Wall','-Wextra','-Werror','-I',str(ROOT/'src/lib'),
                            str(probe),str(ROOT/'src/lib/Game/Music.cpp'),'-o',str(binary)],check=True)
        for scenario,frames,masked in cases:
            reference,transitions=original_trace(memory,scenario,frames,masked)
            if args.fixtures:
                print(f'{scenario:5s} masked={int(masked)} 0x{fnv1a(reference):016x}ULL modes={transitions}')
            if args.verify:
                actual=subprocess.check_output([str(binary),scenario,str(frames),str(int(masked))])
                if actual!=reference:
                    i=next((i for i,(a,b) in enumerate(zip(actual,reference)) if a!=b),min(len(actual),len(reference)))
                    raise AssertionError(f'{scenario} masked={masked}: frame {i//10}, field {i%10}, '
                                         f'C++={actual[i:i+1].hex()} original={reference[i:i+1].hex()}')
                print(f'PASS track={scenario} masked={int(masked)}: {frames} frames')
        if args.verify:print(f'PASS total: {sum(case[1] for case in cases)} music frames')
    if not args.fixtures and not args.verify:parser.error('Select --fixtures, --verify, or --print-data')


if __name__=='__main__':main()
