#!/usr/bin/env python3
"""Compare death/turn state before stage reinitialization; no raster or OS calls."""
from pathlib import Path
import random
import subprocess
import tempfile
from py65.devices.mpu6502 import MPU

ROOT = Path(__file__).resolve().parents[2]
ADDRESSES = [a+p for p in range(2) for a in (0xaa,0xf7,0xad,0xa1,0x97,0x94)]
ADDRESSES += [0x92,0xac,0x99,0x96,0xa0,0xb1,0x57]

def original(memory, values):
    cpu = MPU(memory=list(memory))
    for address,value in zip(ADDRESSES,values): cpu.memory[address]=value
    cpu.pc,cpu.sp=0x855f,0xfd
    for _ in range(1000):
        if cpu.pc in (0x85fc,0x85c5,0x92e6,0x8ef0):
            event={0x92e6:1,0x8ef0:2}.get(cpu.pc,0)
            return bytes([cpu.memory[a] for a in ADDRESSES]+[event])
        if cpu.pc==0x98bc: # HUD redraw has no turn-state effects
            cpu.pc=(cpu.stPopWord()+1)&65535
        else: cpu.step()
    raise RuntimeError('Death scheduler did not terminate')

def main():
    memory=(ROOT/'analysis/loaded-memory.bin').read_bytes()
    rng=random.Random(8561)
    cases=[]
    for _ in range(8192):
        players=[v for p in range(2) for v in
                 (rng.randrange(6),rng.randrange(6),rng.choice([0,1,3,128,255]),
                  rng.randrange(6),rng.randrange(37),rng.randrange(2))]
        cases.append(players+[rng.randrange(2),rng.choice([0,1,1,2,90,255]),
                     rng.choice([0,0,0,1,10]),rng.randrange(37),rng.randrange(6),
                     rng.randrange(4),1])
    source=r'''
#include <Game/Turns.hpp>
#include <iostream>
using namespace gyrus::game;
int main(){unsigned v[19]; while(std::cin>>v[0]){
for(unsigned i=1;i<19;++i)std::cin>>v[i];
Turns s;
for(unsigned p=0;p<2;++p){auto& x=s.players[p];auto* q=v+6*p;
x.lives=q[0];x.protection=q[1];x.special=q[2];x.savedWave=q[3];
x.savedFormation=q[4];x.dualShot=q[5];}
s.activePlayer=v[12];s.deathCountdown=v[13];s.incomingCount=v[14];
s.formationCount=v[15];s.waveIndex=v[16];s.specialCount=v[17];s.playing=v[18];
auto e=advanceDeath(s);
for(auto& p:s.players)for(unsigned x:{p.lives,p.protection,p.special,
p.savedWave,p.savedFormation,static_cast<unsigned char>(p.dualShot)})std::cout.put(x);
for(unsigned x:{s.activePlayer,s.deathCountdown,s.incomingCount,s.formationCount,
s.waveIndex,s.specialCount,static_cast<unsigned char>(s.playing)})std::cout.put(x);
std::cout.put(static_cast<int>(e));}}
'''
    with tempfile.TemporaryDirectory(prefix='gyrus-turn-') as temp:
        path=Path(temp)
        (path/'runner.cpp').write_text(source)
        subprocess.run(['c++','-std=c++20','-O2','-I'+str(ROOT/'src/lib'),
                        str(path/'runner.cpp'),str(ROOT/'src/lib/Game/Turns.cpp'),
                        '-o',str(path/'runner')],check=True)
        actual=subprocess.check_output([str(path/'runner')],input=''.join(
            ' '.join(map(str,c))+'\n' for c in cases).encode())
    assert len(actual)==20*len(cases)
    for i,case in enumerate(cases):
        expected=original(memory,case)
        got=actual[i*20:i*20+20]
        if got!=expected: raise AssertionError(f'{case}: native={list(got)}, original={list(expected)}')
    print(f'Turn oracle passed: {len(cases)} cases, zero mismatches')

if __name__=='__main__': main()
