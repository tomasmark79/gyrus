#!/usr/bin/env python3
"""Original $8353 spawn and $8876 update, with only XOR raster/score side effects intercepted."""
from pathlib import Path
import random, subprocess, tempfile
from py65.devices.mpu6502 import MPU
ROOT=Path(__file__).resolve().parents[2]
RUNNER=r'''
#include <Game/Hazards.hpp>
#include <iostream>
using namespace gyrus::game;
void output(const Hazards& s,unsigned score){std::cout<<score<<' '<<+s.direction<<' '<<+s.cooldown<<' '<<+s.count;
for(auto& a:s.actors)std::cout<<' '<<+a.angle<<' '<<+a.depth<<' '<<+a.spriteType<<' '<<+a.heading<<' '<<(a.visible?(a.width|(a.hit?128:0)):0)<<' '<<+a.position.x<<' '<<+a.position.y;
std::cout<<'\n';}
int main(){char mode;while(std::cin>>mode){Hazards s;HazardSpawnInputs in;unsigned calls=0,r0,r1,r2;
if(mode=='S'){unsigned cool,count,msg,transition,normal,level;
std::cin>>cool>>count>>msg>>transition>>normal>>level>>r0>>r1>>r2;
s.cooldown=cool;s.count=count;in.messageCountdown=msg;in.transition=transition;in.normalStage=normal;in.level=level;
in.randomByte=[&](){unsigned r[]={r0,r1,r2};return uint8_t(r[calls++]);};
auto spawned=spawnHazards(s,in);std::cout<<spawned<<' '<<calls<<' '<<+s.direction<<' '<<+s.cooldown<<' '<<+s.count;
for(auto& a:s.actors)std::cout<<' '<<+a.angle<<' '<<+a.depth<<' '<<+a.spriteType;std::cout<<'\n';
}else{unsigned angle,hit;std::cin>>angle>>hit;in.level=5;in.randomByte=[&](){return uint8_t(++calls==3?angle:0);};spawnHazards(s,in);
for(unsigned tick=0;tick<512;++tick){if(hit && tick==64)s.actors[hit-1].hit=true;
auto e=updateHazards(s,uint8_t(tick));output(s,e.scoreUnits);}}}}
'''
class Memory(list):
    def __init__(self,data,values):super().__init__(data);self.values=values;self.calls=0
    def __getitem__(self,key):
      if key==0xd20a:
        value=self.values[self.calls%len(self.values)];self.calls+=1;return value
      return super().__getitem__(key)
def main():
    data=list((ROOT/'analysis/initialized-memory.bin').read_bytes())
    commands=[]; expected=[]; rng=random.Random(0x8876)
    def machine(values):
      mem=Memory(data,values);cpu=MPU(memory=mem)
      for base in [0x46e,0x4c6,0x51e,0x576,0x5ce,0x626,0x67e]:
        for i in range(36,39):mem[base+i]=0
      for a in range(0x6d6,0x9d6):mem[a]=0
      for a in [0xb5,0xb6,0xb7,0x9a,0xb8,0x92]:mem[a]=0
      mem[0xd9]=1;mem[0xa3]=5
      return cpu,mem
    def run(cpu,start,end):
      m=cpu.memory;cpu.pc=start;score=0
      def ret():cpu.sp=(cpu.sp+1)&255;lo=m[0x100+cpu.sp];cpu.sp=(cpu.sp+1)&255;cpu.pc=((m[0x100+cpu.sp]<<8)|lo)+1
      if end is None:cpu.sp=253;m[0x1fe]=0xfe;m[0x1ff]=0x7f;end=0x7fff
      for _ in range(10000):
        if cpu.pc==end:return score
        if cpu.pc==0x9b98:cpu.p&=~1;ret()
        elif cpu.pc==0x982f:score+=cpu.a;ret()
        else:cpu.step()
      raise RuntimeError(hex(cpu.pc))
    cases=[]
    for _ in range(400):cases.append((rng.choice([0,1,2,180]),rng.choice([0,0,3,255]),rng.choice([0,0,150]),rng.choice([0,0,60]),rng.randrange(2),rng.choice([1,4,5,8,255]),rng.choice([0,0,1]),rng.choice([0,8,1]),rng.randrange(256)))
    for angle in range(0,128,16):cases.append((0,0,0,0,1,5,0,0,angle))
    for case in cases:
      cool,count,msg,transition,normal,level,*randoms=case;cpu,m=machine(randoms)
      for a,v in [(0xb6,cool),(0xb7,count),(0x9a,msg),(0xb8,transition),(0xd9,normal),(0xa3,level)]:m[a]=v
      run(cpu,0x8353,0x83b0)
      row=[int(m[0xb7]!=count),m.calls,m[0xb5],m[0xb6],m[0xb7]]
      for i in range(36,39):row += [m[base+i] for base in [0x51e,0x576,0x67e]]
      expected.append(row);commands.append('S '+' '.join(map(str,case)))
    for angle in range(0,128,16):
     for hit in range(4):
      cpu,m=machine([0,0,angle]);run(cpu,0x8353,0x83b0)
      commands.append(f'T {angle} {hit}')
      for tick in range(512):
        m[0x55]=tick&255
        if hit and tick==64:m[0x5ce+35+hit]|=128
        score=run(cpu,0x8876,None)
        row=[score,m[0xb5],m[0xb6],m[0xb7]]
        for i in range(36,39):row += [m[base+i]for base in [0x51e,0x576,0x67e,0x626,0x5ce,0x46e,0x4c6]]
        expected.append(row)
    with tempfile.TemporaryDirectory() as temp:
      p=Path(temp);(p/'run.cpp').write_text(RUNNER)
      subprocess.run(['g++','-std=c++20','-O2','-I'+str(ROOT/'src/lib'),str(p/'run.cpp'),*[str(ROOT/f'src/lib/Game/{n}.cpp')for n in ['Hazards','Enemies','Polar']],'-o',str(p/'run')],check=True)
      actual=subprocess.check_output([str(p/'run')],input=('\n'.join(commands)+'\n').encode()).decode().splitlines()
    assert len(actual)==len(expected)
    for index,(line,want) in enumerate(zip(actual,expected)):
      got=list(map(int,line.split()))
      if got!=want:raise AssertionError((index,[(i,a,b)for i,(a,b)in enumerate(zip(got,want))if a!=b]))
    print(f'Hazards oracle passed: {len(cases)} spawn attempts + {len(expected)-len(cases)} updates, zero mismatches')
if __name__=='__main__':main()
