#!/usr/bin/env python3
"""Compare complete packed 6502 framebuffer to C++ raster using draw snapshots.

Original bitmap instructions execute without interception. Covers particle depth
bucket crossings and formation seek-player programs which change depth after draw.
Requires py65 1.2.0 and initialized-memory.bin.
"""
from pathlib import Path
import subprocess,tempfile
from py65.devices.mpu6502 import MPU
from enemy_shots_oracle import invoke
ROOT=Path(__file__).resolve().parents[2]
RUNNER=r'''
#include <Game/Upgrade.hpp>
#include <Game/Formation.hpp>
#include <Game/Graphics.hpp>
#include <iostream>
using namespace gyrus::game;
void draw(Framebuffer& f,const EnemyActor& a){if(a.visible)xorSprite(f,a.renderedSpriteType/2,a.renderedDepth,a.renderedHeading,a.position.x,a.position.y);}
int main(){char mode;unsigned angle,depth,type,heading,slot;while(std::cin>>mode>>angle>>depth>>type>>heading>>slot){Framebuffer f{};unsigned after;
if(mode=='P'){UpgradeState s;s.phase=0x8c;auto& a=s.particles[slot].actor;a.angle=angle;a.depth=depth;a.heading=heading;updateUpgradeParticles(s,{});if(s.particles[slot].drawn)draw(f,a);after=a.depth;}
else {EnemyPaths p;EnemyState s;FormationControl c;c.attackState[0]=3;auto& a=s.incoming[0];a.angle=angle;a.depth=depth;a.spriteType=type;a.heading=heading;projectEnemyActor(a);EnemyInputs in;in.playerAngle=32;in.randomByte=[]{return uint8_t(0);};updateFormationProgram(p,s,c,0,in);draw(f,a);after=a.depth;}
std::cout<<after<<' ';for(auto pixel:f)std::cout<<char('0'+pixel);std::cout<<'\n';}}
'''
def reference(base,item):
    mode,angle,depth,kind,heading,slot=item;m=base.copy();m[0x3010:0x4000]=[0]*(0x4000-0x3010)
    cpu=MPU(memory=m)
    if mode=='P':
        m[0xdb]=0x8c;m[0x55]=0
        for i in range(23,29):m[0x576+i]=0;m[0x67e+i]=34
        obj=23+slot
    else:obj=39;m[0xdc]=3;m[0xe7]=31;m[0xd20a]=0;m[0x51e]=32
    for address,value in zip((0x51e,0x576,0x67e,0x626,0x5ce),(angle,depth,kind,heading,0)):m[address+obj]=value
    if mode=='P':invoke(cpu,0x870c)
    else:
        cpu.x=obj;invoke(cpu,0x9a97)
        cpu.x=0;invoke(cpu,0x9199,stop=0x9227)
    pixels=''.join(str((v>>shift)&3) for v in m[0x30c4:0x3fc4] for shift in (6,4,2,0))
    return str(m[0x576+obj])+' '+pixels

def main():
    base=list((ROOT/'analysis/initialized-memory.bin').read_bytes())
    cases=[]
    for a in range(0,128,8):
        for d in (7,8,15,16,23,24,31,32,39,40,47,48,55,56):
            for h in (0,1,3):
                for slot in (4,5):cases.append(('P',a,d,34,h,slot))
                for kind in (6,8,12,22):cases.append(('F',a,d,kind,h,0))
    with tempfile.TemporaryDirectory(prefix='gyrus-render-snapshot-') as tmp:
        p=Path(tmp);(p/'probe.cpp').write_text(RUNNER)
        subprocess.run(['g++','-std=c++20','-O2','-I',str(ROOT/'src/lib'),str(p/'probe.cpp')]+[str(ROOT/f'src/lib/Game/{name}.cpp') for name in ('Upgrade','Formation','Enemies','Polar','Graphics')]+['-o',str(p/'probe')],check=True)
        lines=subprocess.check_output([str(p/'probe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True).splitlines()
    assert len(lines)==len(cases)
    for item,line in zip(cases,lines):
        expected=reference(base,item)
        if expected!=line:
            diff=next(i for i,(a,b) in enumerate(zip(expected,line)) if a!=b)
            raise AssertionError(f'{item}: first difference {diff}; source prefix {expected[:8]}, C++ {line[:8]}')
    print(f'Render snapshot oracle passed: {len(cases)} complete 160x96 framebuffers and logical depths match; original raster executes without interception')
if __name__=='__main__':main()
