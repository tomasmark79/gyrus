#!/usr/bin/env python3
"""Stage/planet progression through $9425, stopping before $92E6 init."""
from pathlib import Path
import itertools
import subprocess
import tempfile
from py65.devices.mpu6502 import MPU
from turn_oracle import ROOT

def main():
    memory=(ROOT/'analysis/loaded-memory.bin').read_bytes()
    cases=list(itertools.product(range(2),range(256),range(4),range(0,48,8)))
    expected=bytearray()
    for player,level,warps,planet in cases:
        cpu=MPU(memory=list(memory))
        cpu.pc=0x9425
        cpu.memory[0x92]=player
        for address,value in ((0xa3,level),(0x9b,warps),(0x9d,planet),
                              (0xa1,3),(0xad,128),(0xf7,5)):
            cpu.memory[address+player]=value
        for _ in range(100):
            if cpu.pc==0x92e6: break
            cpu.step()
        else: raise RuntimeError('Stage advance did not terminate')
        expected.extend(cpu.memory[a+player] for a in (0xa3,0x9b,0x9d,0xa1,0xad,0xf7))
    source=r'''
#include <Game/Turns.hpp>
#include <iostream>
using namespace gyrus::game;
int main(){unsigned player,level,warps,planet;
while(std::cin>>player>>level>>warps>>planet){Turns s;s.activePlayer=player;
auto& p=s.players[player];p.level=level;p.warps=warps;p.planetOffset=planet;
p.savedWave=3;p.special=128;p.protection=5;advanceStage(s);
for(auto x:{p.level,p.warps,p.planetOffset,p.savedWave,p.special,p.protection})std::cout.put(x);}}
'''
    with tempfile.TemporaryDirectory(prefix='gyrus-stage-') as temp:
        path=Path(temp)
        (path/'runner.cpp').write_text(source)
        subprocess.run(['c++','-std=c++20','-O2','-I'+str(ROOT/'src/lib'),
                        str(path/'runner.cpp'),str(ROOT/'src/lib/Game/Turns.cpp'),
                        '-o',str(path/'runner')],check=True)
        actual=subprocess.check_output([str(path/'runner')],input=''.join(
            ' '.join(map(str,c))+'\n' for c in cases).encode())
    assert len(actual)==len(expected)
    for i,case in enumerate(cases):
        got=actual[i*6:i*6+6]; want=expected[i*6:i*6+6]
        if got!=want: raise AssertionError(f'{case}: native={list(got)}, original={list(want)}')
    print(f'Stage oracle passed: {len(cases)} cases, zero mismatches')

if __name__=='__main__': main()
