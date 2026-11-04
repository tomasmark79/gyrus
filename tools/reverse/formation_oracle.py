#!/usr/bin/env python3
"""Compare formation launch and dynamic attack programs to original 6502 code.

Requires py65 1.2.0. Graphics raster writes are intercepted for launch only;
dynamic-program checks execute all original instructions without interception.
"""
import argparse
from pathlib import Path
import random
import subprocess
import tempfile

from py65.devices.mpu6502 import MPU

RUNNER = r'''
#include <Game/Formation.hpp>
#include <iostream>
using namespace gyrus::game;
int main() {
  char mode;
  while(std::cin>>mode) {
    EnemyPaths p; EnemyState s; FormationControl c;
    EnemyInputs input;
    unsigned calls=0,ra,rb,rc;
    if(mode=='P') {
      unsigned index,attack,angle,depth,type,level,player;
      std::cin>>index>>attack>>angle>>depth>>type>>level>>player>>ra>>rb>>rc;
      c=formationControl(level); c.attackState[index]=attack;
      input.playerAngle=player;
      input.randomByte=[&](){const unsigned v[]={ra,rb,rc};return uint8_t(v[(calls++)%3]);};
      auto& actor=s.incoming[index]; actor.angle=angle; actor.depth=depth; actor.spriteType=type;
      p.cursors[index]=33; p.phases[index]=255;
      auto& point=p.tracks[index&1][(index>>1)+1];point={20,60,136};
      updateFormationProgram(p,s,c,index,input);
      std::cout<<+p.cursors[index]<<' '<<+p.phases[index]<<' '<<+c.attackState[index]
               <<' '<<+actor.depth<<' '<<+point.depth<<' '<<+point.angle<<' '<<+point.style<<' '<<calls;
    } else {
      unsigned count,available,level,enabled,death,player;
      std::cin>>count>>available>>level>>enabled>>death>>player>>ra;
      c=formationControl(level);input.playerAngle=player;input.returnToFormation=enabled;
      input.deathCountdown=death;
      input.randomByte=[&](){++calls;return uint8_t(ra);};
      p.incomingCount=count;s.formationCount=available;
      for(unsigned i=0;i<36;++i) {
        projectEnemyActor(s.formation[i]);
        if(i>=available){s.formation[i].visible=false;s.formation[i].width=0;}
      }
      for(unsigned i=0;i<count;++i){s.incoming[i].visible=true;s.incoming[i].width=3;}
      const auto launched=attemptFormationLaunch(p,s,c,input);
      std::cout<<launched<<' '<<+p.incomingCount<<' '<<+s.formationCount<<' '<<calls;
      for(auto v:p.cursors)std::cout<<' '<<+v;
      for(auto v:p.phases)std::cout<<' '<<+v;
      for(auto v:c.attackState)std::cout<<' '<<+v;
      for(const auto& lane:p.tracks)for(unsigned i=1;i<6;++i)
        std::cout<<' '<<+lane[i].depth<<' '<<+lane[i].angle<<' '<<+lane[i].style;
      for(const auto& group:{s.incoming.data(),s.formation.data()}) {
        const unsigned size=group==s.incoming.data()?10:36;
        for(unsigned i=0;i<size;++i){const auto& a=group[i];
          std::cout<<' '<<+a.angle<<' '<<+a.depth<<' '<<+a.heading<<' '<<+a.spriteType
                   <<' '<<(a.visible?a.width|(a.hit?128:0):0)<<' '<<+a.position.x
                   <<' '<<+a.position.y<<' '<<+a.formationSlot;
        }
      }
    }
    std::cout<<'\n';
  }
}
'''


class Memory(list):
    calls = 0
    random_bytes = [0]

    def __getitem__(self, index):
        if index == 0xd20a:
            value = self.random_bytes[self.calls % len(self.random_bytes)]
            self.calls += 1
            return value
        return super().__getitem__(index)


TRACE_RUNNER = r'''
#include <Game/Formation.hpp>
#include <iostream>
using namespace gyrus::game;
int main() {
  unsigned level,hitTest,deathTest;
  while(std::cin>>level>>hitTest>>deathTest) {
    EnemyPaths p;EnemyState s;auto c=formationControl(level);
    p.scoreChain=2;p.cadence=3;p.evenTimer=3;p.oddTimer=1;s.formationCount=36;
    for(auto& a:s.formation)projectEnemyActor(a);
    unsigned calls=0;
    const unsigned randoms[]={0,1,128,31,0,0,12,255,0,4,0,0,6,7};
    EnemyInputs input;input.levelNumber=level;input.returnToFormation=true;
    input.randomByte=[&](){return uint8_t(randoms[(calls++)%14]);};
    for(unsigned tick=0;tick<1600;++tick) {
      input.playerAngle=(tick*2)&127;input.deathCountdown=deathTest&&tick>=300&&tick<360;
      if(hitTest&&tick%29==0)for(auto& a:s.incoming)if(a.visible){a.hit=true;break;}
      if(hitTest&&tick%41==0)for(auto& a:s.formation)if(a.visible){a.hit=true;break;}
      const auto launched=attemptFormationLaunch(p,s,c,input);
      auto event=updateEnemyEntriesWithFormation(p,s,c,input);
      event.scoreUnits+=updateFormationGrid(s,c).scoreUnits;
      unsigned sound=0,bonus=0;
      for(auto v:event.sounds)sound=sound*16+v+1;
      for(auto b:event.bonuses)bonus=bonus*65536+(b.angle<<8)+(b.depth<<1)+b.label;
      std::cout<<launched<<' '<<calls<<' '<<event.scoreUnits<<' '<<sound<<' '<<bonus
               <<' '<<+p.incomingCount<<' '<<+s.formationCount<<' '<<+p.scoreChain
               <<' '<<+s.remainingBonusEnemies<<' '<<+p.collisionMask<<' '<<+p.evenTimer<<' '<<+p.oddTimer
               <<' '<<+c.frameCounter<<' '<<+c.gridPhase;
      for(auto v:p.cursors)std::cout<<' '<<+v;
      for(auto v:p.phases)std::cout<<' '<<+v;
      for(auto v:c.attackState)std::cout<<' '<<+v;
      for(const auto& lane:p.tracks)for(unsigned i=1;i<6;++i)
        std::cout<<' '<<+lane[i].depth<<' '<<+lane[i].angle<<' '<<+lane[i].style;
      for(const auto& group:{s.incoming.data(),s.formation.data()}) {
        const unsigned size=group==s.incoming.data()?10:36;
        for(unsigned i=0;i<size;++i){const auto& a=group[i];
          std::cout<<' '<<+a.angle<<' '<<+a.depth<<' '<<+a.heading<<' '<<+a.spriteType
                   <<' '<<(a.visible?a.width|(a.hit?128:0):0)<<' '<<+a.position.x
                   <<' '<<+a.position.y<<' '<<+a.formationSlot;
        }
      }
      std::cout<<'\n';
    }
  }
}
'''


def integrated_trace(root,compiler,original):
    cases=[[1,0,0],[1,1,0],[9,0,1],[12,1,1]]
    with tempfile.TemporaryDirectory(prefix='gyrus-formation-trace-') as temporary:
        folder=Path(temporary);source,binary=folder/'runner.cpp',folder/'runner'
        source.write_text(TRACE_RUNNER)
        subprocess.run([compiler,'-std=c++20','-O2','-I'+str(root/'src/lib'),str(source),
                        str(root/'src/lib/Game/Formation.cpp'),str(root/'src/lib/Game/Enemies.cpp'),
                        str(root/'src/lib/Game/Polar.cpp'),'-o',str(binary)],check=True)
        lines=iter(subprocess.check_output([str(binary)],text=True,
                   input=''.join(' '.join(map(str,c))+'\n'for c in cases)).splitlines())
    tested=0
    for level,hit_test,death_test in cases:
        m=Memory(original);cpu=MPU(memory=m)
        m.random_bytes=[0,1,128,31,0,0,12,255,0,4,0,0,6,7]
        score,sound,bonus=0,0,0
        def ret():cpu.pc=(cpu.stPopWord()+1)&65535
        def run(address,stop=0x7fff):
            nonlocal score,sound,bonus
            cpu.pc,cpu.sp,cpu.p=address,0xfd,0x30
            m[0x1fe],m[0x1ff]=0xfe,0x7f
            for _ in range(50000):
                if cpu.pc==stop:return
                if cpu.pc in [0x9b98,0x9b9c,0x9af8]:cpu.p&=~1;ret()
                elif cpu.pc==0x982f:score+=cpu.a;ret()
                elif cpu.pc==0xb995:sound=(sound*16+cpu.y+1)&0xffffffff;ret()
                elif cpu.pc==0x8c91:
                    bonus=(bonus*65536+(m[0x4e]<<8)+(m[0x4f]<<1)+cpu.y)&0xffffffff;ret()
                else:cpu.step()
            raise RuntimeError(f'Formation trace stuck at ${cpu.pc:04X}')
        for a in range(0x1a,0x38):m[a]=0
        for a in range(0xdc,0xe6):m[a]=0
        for a in range(0x6d6,0x9d6):m[a]=0
        for index in list(range(39,49))+list(range(52,88)):
            for base in [0x51e,0x576,0x626,0x67e,0x5ce,0x46e,0x4c6]:m[base+index]=0
        for i in range(36):
            index=i+52;m[0x51e+index]=[116,106,96,84,74,64,52,42,32,20,10,0][i%12]
            m[0x576+index]=52-(i//12)*4;m[0x67e+index]=22
            cpu.x=index;run(0x9a97)
        for address,value in {0xcd:2,0xa5:0,0xf0:3,0xf2:3,0xef:3,0xf1:1,
                              0x99:0,0x96:36,0xe6:1,0xf5:0,0xda:40,0xd9:1,
                              0xf9:0,0xa3:level,0x92:0,0x56:0,0x5b:0}.items():m[address]=value
        m[0xe7]=[31,15,15,7][min(level>>2,3)]
        m[0xe9]=[127,63,31,15][min(level>>2,3)]
        m[0xe8]=min((level>>2)+2,4)
        for tick in range(1600):
            m[0x51e]=(tick*2)&127;m[0xac]=int(death_test and 300<=tick<360)
            if hit_test and tick%29==0:
                for i in range(10):
                    if m[0x5f5+i]:m[0x5f5+i]|=128;break
            if hit_test and tick%41==0:
                for i in range(36):
                    if m[0x602+i]:m[0x602+i]|=128;break
            score,sound,bonus=0,0,0
            before=m[0x99];run(0x840b);launched=(m[0x99]-before)&255
            run(0x85ff,0x861a)
            run(0x8620,0x86a0)
            expected=[launched,m.calls,score,sound,bonus]+[m[a]for a in [0x99,0x96,0xcd,0xda,0xf9,0xef,0xf1]]
            expected.extend([m[0x56],m[0x5b]])
            expected.extend(m[0x1a:0x24]);expected.extend(m[0x24:0x2e]);expected.extend(m[0xdc:0xe6])
            for lane in range(2):
                for i in range(1,6):expected.extend(m[base+lane*128+i]for base in [0x6d6,0x7d6,0x8d6])
            for indices in [range(39,49),range(52,88)]:
                for index in indices:
                    expected.extend(m[base+index]for base in [0x51e,0x576,0x626,0x67e,0x5ce,0x46e,0x4c6])
                    expected.append(m[0x2e+index-39]if index<49 else 0)
            actual=list(map(int,next(lines).split()))
            if expected!=actual:
                mismatch=[(i,a,b)for i,(a,b)in enumerate(zip(expected,actual))if a!=b]
                raise AssertionError(f'Trace level={level},hits={hit_test},death={death_test},tick={tick}: '
                                     f'(index,6502,C++) {mismatch[:20]}')
            tested+=1
    if next(lines,None)is not None:raise AssertionError('Unexpected trace output')
    print(f'Integrated formation trace passed: {tested} gameplay updates, zero mismatches')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default='c++')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    original = list((root/'analysis/initialized-memory.bin').read_bytes())
    rng = random.Random(0x9199)
    cases = []
    for index in range(10):
        for attack in range(5):
            for depth in [14,16,18,30,38]:
                for sprite in [8,10,12]:
                    for choice in range(0,16,2):
                        cases.append(('P',[index,attack,rng.randrange(128),depth,sprite,
                                          rng.choice([1,4,8,12]),rng.randrange(128),0,0,choice]))
    for _ in range(1500):
        cases.append(('P',[rng.randrange(10),rng.randrange(5),rng.randrange(128),
                          rng.randrange(10,54,2),rng.choice([8,10,12]),rng.randrange(1,16),
                          rng.randrange(128),rng.randrange(256),rng.randrange(256),rng.randrange(256)]))
    for level in [1,4,8,12]:
        for count in range(min((level>>2)+2,4)+1):
            for available in [0,1,2,36]:
                for enabled,death,random_byte in [(0,0,0),(1,1,0),(1,0,0),(1,0,1),(1,0,128)]:
                    cases.append(('L',[count,available,level,enabled,death,0,random_byte]))
    with tempfile.TemporaryDirectory(prefix='gyrus-formation-oracle-') as temporary:
        folder=Path(temporary);source,binary=folder/'runner.cpp',folder/'runner'
        source.write_text(RUNNER)
        subprocess.run([args.compiler,'-std=c++20','-O2','-I'+str(root/'src/lib'),str(source),
                        str(root/'src/lib/Game/Formation.cpp'),str(root/'src/lib/Game/Enemies.cpp'),
                        str(root/'src/lib/Game/Polar.cpp'),'-o',str(binary)],check=True)
        output=iter(subprocess.check_output([str(binary)],text=True,
                    input=''.join(mode+' '+' '.join(map(str,v))+'\n'for mode,v in cases)).splitlines())
    counts=dict(P=0,L=0)
    for mode,values in cases:
        m=Memory(original);cpu=MPU(memory=m)
        def ret():cpu.pc=(cpu.stPopWord()+1)&65535
        def run(address,stop=0x7fff):
            cpu.pc,cpu.sp,cpu.p=address,0xfd,0x30
            m[0x1fe],m[0x1ff]=0xfe,0x7f
            for _ in range(40000):
                if cpu.pc==stop:return
                if cpu.pc==0x9b98:
                    cpu.p &= ~1
                    ret()
                else:cpu.step()
            raise RuntimeError(f'Formation routine did not finish at ${cpu.pc:04X}')
        if mode=='P':
            index,attack,angle,depth,sprite,level,player,ra,rb,rc=values
            m.random_bytes=[ra,rb,rc]
            m[0xdc+index],m[0x545+index],m[0x59d+index],m[0x6a5+index]=attack,angle,depth,sprite
            m[0x1a+index],m[0x24+index],m[0x51e]=33,255,player
            m[0xe7]=[31,15,15,7][min(level>>2,3)]
            point=(index&1)*128+(index>>1)+1
            m[0x6d6+point],m[0x7d6+point],m[0x8d6+point]=20,60,136
            cpu.x=index
            run(0x9199,0x9227)
            expected=[m[0x1a+index],m[0x24+index],m[0xdc+index],m[0x59d+index],
                      m[0x6d6+point],m[0x7d6+point],m[0x8d6+point],m.calls]
        else:
            count,available,level,enabled,death,player,random_byte=values
            m.random_bytes=[random_byte]
            for a in range(0x1a,0x38):m[a]=0
            for a in range(0xdc,0xe6):m[a]=0
            for a in range(0x6d6,0x9d6):m[a]=0
            for index in list(range(39,49))+list(range(52,88)):
                for base in [0x51e,0x576,0x626,0x67e,0x5ce,0x46e,0x4c6]:m[base+index]=0
            for i in range(36):
                index=i+52
                m[0x51e+index]=[116,106,96,84,74,64,52,42,32,20,10,0][i%12]
                m[0x576+index]=52-(i//12)*4
                m[0x67e+index]=22
                cpu.x=index;run(0x9a97)
                if i>=available:m[0x5ce+index]=0
            for i in range(count):m[0x5f5+i]=3
            m[0x99],m[0x96],m[0xe6],m[0xac],m[0x51e]=count,available,enabled,death,player
            m[0xe9]=[127,63,31,15][min(level>>2,3)]
            m[0xe8]=min((level>>2)+2,4)
            run(0x840b)
            expected=[(m[0x99]-count)&255,m[0x99],m[0x96],m.calls]
            expected.extend(m[0x1a:0x24]);expected.extend(m[0x24:0x2e]);expected.extend(m[0xdc:0xe6])
            for lane in range(2):
                for i in range(1,6):expected.extend(m[base+lane*128+i]for base in [0x6d6,0x7d6,0x8d6])
            for indices in [range(39,49),range(52,88)]:
                for index in indices:
                    expected.extend(m[base+index]for base in [0x51e,0x576,0x626,0x67e,0x5ce,0x46e,0x4c6])
                    expected.append(m[0x2e+index-39]if index<49 else 0)
        actual=list(map(int,next(output).split()))
        if actual!=expected:
            mismatch=[(i,a,b)for i,(a,b)in enumerate(zip(expected,actual))if a!=b]
            raise AssertionError(f'{mode} {values}: (index,6502,C++) {mismatch[:20]}')
        counts[mode]+=1
    if next(output,None)is not None:raise AssertionError('Unexpected native output')
    print(f'Formation oracle passed: dynamic programs={counts["P"]}, launch transitions={counts["L"]}; zero mismatches')
    integrated_trace(root,args.compiler,original)


if __name__=='__main__':main()
