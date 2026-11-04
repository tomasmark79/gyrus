#!/usr/bin/env python3
"""$92E6 setup, stopping at final player raster jump $A01D.
Only score-refresh, audio, and final XOR raster writes are intercepted.
Palette, HUD, clears, messages, projection and descriptor geometry execute.
"""
from pathlib import Path
import random, subprocess, tempfile
from py65.devices.mpu6502 import MPU
ROOT=Path(__file__).resolve().parents[2]
RUNNER=r'''
#include <Game/Stage.hpp>
#include <iostream>
using namespace gyrus::game;
int main(){unsigned active,level,level0,warps,planet,special,death,saved,wave;
while(std::cin>>active>>level>>level0>>warps>>planet>>special>>death>>saved>>wave){
Turns t;EnemyPaths p;EnemyState e;FormationControl f;
t.activePlayer=active;t.players[0].level=level0;auto& a=t.players[active];
a.level=level;a.warps=warps;a.planetOffset=planet;a.special=special;a.savedFormation=saved;a.savedWave=wave;t.deathCountdown=death;
f.frameCounter=79;f.gridPhase=3;p.collisionMask=77;p.cursors.fill(19);p.phases.fill(9);
for(unsigned i=0;i<36;++i){e.formation[i].angle=(i*10)&126;e.formation[i].depth=44+(i%3)*4;}
auto s=initializeStage(t,p,e,f);
for(auto v:{p.scoreChain,e.remainingBonusEnemies,f.randomTurnMask,f.launchMask,s.specialShotReload,f.maxIncoming,
 s.playfieldColors[0],s.playfieldColors[1],s.playfieldColors[2],s.music,a.special,s.upgradeTargetCount,
 s.enemyShotCap,s.enemyShotCooldownReload,e.formationCount,e.nextFormationSlot,p.nextWave,
 s.messageOffset,s.messageCountdown,t.deathCountdown,s.playerPosition.x,s.playerPosition.y,
 f.frameCounter,f.gridPhase,p.collisionMask})std::cout<<+v<<' ';
for(auto v:p.phases)std::cout<<+v<<' ';for(auto v:f.attackState)std::cout<<+v<<' ';
for(auto& a:e.formation)std::cout<<+a.angle<<' '<<+a.depth<<' '<<(a.visible?+a.width:0)<<' ';
std::cout<<'\n';}}
'''
def main():
    rng=random.Random(0x92e6)
    cases=[]
    for level in list(range(20))+[63,127,254,255]:
      for active in (0,1):
       for death in (0,255):
        cases.append((active,level,rng.randrange(1,16),rng.randrange(4),rng.randrange(6)*8,rng.choice([0,1,3,128]),death,rng.choice([0,1,12,36]),rng.randrange(6)))
    memory=list((ROOT/'analysis/initialized-memory.bin').read_bytes())
    expected=[]
    for case in cases:
      active,level,level0,warps,planet,special,death,saved,wave=case
      cpu=MPU(memory=memory.copy());m=cpu.memory;cpu.pc=0x92e6
      m[0xa3]=level0;m[0x92]=active
      for a,v in [(0xa3,level),(0x9b,warps),(0x9d,planet),(0xad,special),(0x97,saved),(0xa1,wave),(0xaa,5)]:m[a+active]=v
      m[0xac]=death;m[0x56]=79;m[0x5b]=3;m[0xf9]=77
      for i in range(10):m[0x24+i]=9;m[0xdc+i]=7
      for i in range(36):m[0x551+i+1]=(i*10)&126;m[0x5a9+i+1]=44+(i%3)*4
      music=None
      def ret():cpu.sp=(cpu.sp+1)&255;lo=m[0x100+cpu.sp];cpu.sp=(cpu.sp+1)&255;cpu.pc=((m[0x100+cpu.sp]<<8)|lo)+1
      for steps in range(300000):
        if cpu.pc==0xa01d:break
        if cpu.pc==0xb901:music=cpu.a;ret()
        elif cpu.pc in (0x982f,0x9b98):ret()
        else:cpu.step()
      else:raise RuntimeError(hex(cpu.pc))
      row=[m[a] for a in [0xcd,0xda,0xe7,0xe9,0xf6,0xe8,0xd016,0xd017,0xd018]]+[music,m[0xad+active],m[0xb1]]
      row += [m[a] for a in [0xa6,0xa8,0x96,0xf5,0xa0,0xfa,0x9a,0xac,0x46e,0x4c6,0x56,0x5b,0xf9]]
      row += m[0x24:0x2e]+m[0xdc:0xe6]
      for i in range(36):row += [m[0x552+i],m[0x5aa+i],m[0x602+i]]
      expected.append(row)
    with tempfile.TemporaryDirectory() as temp:
      path=Path(temp);(path/'run.cpp').write_text(RUNNER)
      subprocess.run(['g++','-std=c++20','-O2','-I'+str(ROOT/'src/lib'),str(path/'run.cpp'),*[str(ROOT/f'src/lib/Game/{n}.cpp') for n in ['Stage','Formation','Enemies','Polar']],'-o',str(path/'run')],check=True)
      actual=subprocess.check_output([str(path/'run')],input=''.join(' '.join(map(str,c))+'\n' for c in cases).encode()).decode().splitlines()
    for case,want,line in zip(cases,expected,actual):
      got=list(map(int,line.split()))
      if got!=want:
        differences=[(i,a,b)for i,(a,b)in enumerate(zip(got,want))if a!=b]
        raise AssertionError((case,differences))
    assert len(actual)==len(expected)
    print(f'Stage setup oracle passed: {len(cases)} complete initializations, zero mismatches')
if __name__=='__main__':main()
