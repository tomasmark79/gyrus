#!/usr/bin/env python3
"""Reference tests for original shot-vs-object geometry and hit marking."""
from pathlib import Path
from py65.devices.mpu6502 import MPU
import random
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[2]

def marking_cases(cpu):
    rng=random.Random(817)
    inputs=bytearray(); expected=bytearray()
    for _ in range(512):
        shots=[]
        for slot in range(6):
            shots.append((rng.randrange(40,120),rng.randrange(20,80),
                          rng.choice([0,3,3,3,131])))
        targets=[]
        for slot in range(51):
            sx,sy,_=shots[rng.randrange(6)]
            targets.append(((sx+rng.randrange(-5,6))&255,(sy+rng.randrange(-5,6))&255,
                            rng.choice([0,3,5,9,133])))
        for slot,(x,y,flags) in enumerate(shots,1):
            cpu.memory[0x46e+slot]=x;cpu.memory[0x4c6+slot]=y;cpu.memory[0x5ce+slot]=flags
        for slot,(x,y,flags) in enumerate(targets,37):
            cpu.memory[0x46e+slot]=x;cpu.memory[0x4c6+slot]=y;cpu.memory[0x5ce+slot]=flags
        # Occupy explosion slots: this preserves hit selection/marking while
        # bypassing visual-explosion allocation, which is a different subsystem.
        cpu.memory[0x5da:0x5df]=[1]*5
        cpu.pc,cpu.sp,cpu.p=0x813a,0xff,0x30
        for _ in range(60000):
            if cpu.pc==0x821f:break
            cpu.step()
        else:raise RuntimeError('Original hit scan failed to complete')
        for item in shots+targets:inputs.extend(item)
        for slot in list(range(1,7))+list(range(37,88)):
            expected.append(cpu.memory[0x5ce+slot]>>7)
    return inputs,expected

def main():
    cpu=MPU(memory=list((ROOT/'analysis/loaded-memory.bin').read_bytes()))
    rng=random.Random(813)
    cases=[(x,48,target,48,w) for w in (1,2,3,5,10,15)
           for x in range(256) for target in range(256)]
    cases += [tuple(rng.randrange(256) for _ in range(4))+(rng.randrange(1,128),)
              for _ in range(8192)]
    expected=bytearray()
    for x,y,tx,ty,w in cases:
        cpu.pc,cpu.x,cpu.p=0x8146,1,0x30
        for addr,value in ((0x46f,x),(0x4c7,y),(0x493,tx),(0x4eb,ty),(0x5f3,w)):
            cpu.memory[addr]=value
        for _ in range(100):
            if cpu.pc in (0x8168,0x817d):
                expected.append(cpu.pc==0x817d)
                break
            cpu.step()
        else: raise RuntimeError('Geometry did not reach hit/miss boundary')
    source=r'''
#include <Game/Collision.hpp>
#include <iostream>
int main(int argc,char**) {
  if(argc>1) {
    unsigned char data[57*3];
    while(std::cin.read(reinterpret_cast<char*>(data),sizeof(data))) {
      gyrus::game::PlayerState player;
      std::array<gyrus::game::CollisionTarget,51> targets;
      for(unsigned i=0;i<6;++i) {
        player.shots[i].position={data[i*3],data[i*3+1],false};
        player.shots[i].active=data[i*3+2]!=0;
        player.shots[i].hit=data[i*3+2]&128;
      }
      for(unsigned i=0;i<51;++i) {
        const auto* p=data+(i+6)*3;
        targets[i]={p[0],p[1],static_cast<std::uint8_t>(p[2]&127),p[2]!=0,(p[2]&128)!=0};
      }
      (void)gyrus::game::resolveShotHits(player,targets);
      for(const auto& shot:player.shots)std::cout.put(shot.hit);
      for(const auto& target:targets)std::cout.put(target.hit);
    }
    return 0;
  }
  unsigned char bytes[5];
  while(std::cin.read(reinterpret_cast<char*>(bytes),5)) {
    gyrus::game::CollisionTarget target{bytes[2],bytes[3],bytes[4],true,false};
    std::cout.put(gyrus::game::shotOverlaps(bytes[0],bytes[1],target));
  }
}
'''
    with tempfile.TemporaryDirectory(prefix='gyrus-collision-') as temp:
        p=Path(temp);(p/'runner.cpp').write_text(source)
        subprocess.run(['c++','-std=c++20','-O2','-I'+str(ROOT/'src/lib'),str(p/'runner.cpp'),
                        str(ROOT/'src/lib/Game/Collision.cpp'),'-o',str(p/'runner')],check=True)
        actual=subprocess.check_output([str(p/'runner')],input=b''.join(bytes(c) for c in cases))
        marking_input,marking_expected=marking_cases(cpu)
        marking_actual=subprocess.check_output([str(p/'runner'),'resolve'],input=marking_input)
    if actual!=expected:
        first=next(i for i,(a,b) in enumerate(zip(actual,expected)) if a!=b)
        raise AssertionError(f'{cases[first]}: C++ {actual[first]}, original {expected[first]}')
    print(f'Collision geometry oracle passed: {len(cases)} cases, zero mismatches')
    if marking_actual!=marking_expected:
        first=next(i for i,(a,b) in enumerate(zip(marking_actual,marking_expected)) if a!=b)
        raise AssertionError(f'Hit scan case{first//57}, slot{first%57}: native{marking_actual[first]}, original{marking_expected[first]}')
    print('Collision hit marking passed: 512 complete six-shot/51-target scans')

if __name__=='__main__':main()
