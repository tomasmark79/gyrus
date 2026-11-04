#!/usr/bin/env python3
"""Compare typed upgrade state with original 6502; intercept only bitmap/event sinks."""
from pathlib import Path
import random, subprocess, tempfile
from py65.devices.mpu6502 import MPU
from enemy_shots_oracle import invoke, return_from_call
ROOT=Path(__file__).resolve().parents[2]
GLOBALS=(0xdb,0xad,0xb1,0xb3,0xb0,0xbb,0xaf,0x94)
SLOTS=tuple(range(23,29))+tuple(range(49,52))
FIELDS=(0x51e,0x576,0x67e,0x626,0x5ce,0x46e,0x4c6)
RUNNER=r'''
#include <Game/Upgrade.hpp>
#include <iostream>
using namespace gyrus::game;
int main(){char k;unsigned v;while(std::cin>>k){UpgradeState s;
for(auto p:{&s.phase,&s.targetPhase,&s.targetCount,&s.forcedShooter,&s.scoreChain,&s.animation,&s.spawnDelay,&s.dualShotLevel}){std::cin>>v;*p=v;}
for(unsigned i=0;i<9;++i){auto& a=i<6?s.particles[i].actor:s.targets[i-6];unsigned x[7];for(auto& z:x)std::cin>>z;a.angle=x[0];a.depth=x[1];a.spriteType=x[2];a.heading=x[3];a.visible=x[4]!=0;a.hit=x[4]&128;a.width=x[4]&127;a.position={static_cast<uint8_t>(x[5]),static_cast<uint8_t>(x[6]),false};}
for(auto& p:s.particles){std::cin>>v;p.drawn=v;}
UpgradeInputs in;for(auto p:{&in.playerAngle,&in.playerDepth,&in.tick,&in.formationCount,&in.deathCountdown,&in.levelRank}){std::cin>>v;*p=v;}std::cin>>v;in.returnToFormation=v;
unsigned r=0;EnemyEvents e;if(k=='S')r=static_cast<unsigned>(spawnUpgrade(s,in));else if(k=='P')r=updateUpgradeParticles(s,in);else e=updateUpgradeTargets(s,in);
std::cout<<r<<' '<<e.scoreUnits<<' '<<e.sounds.size();for(auto z:e.sounds)std::cout<<' '<<+z;std::cout<<' '<<e.bonuses.size();for(auto b:e.bonuses)std::cout<<' '<<+b.angle<<' '<<+b.depth<<' '<<+b.label<<' '<<b.incomingCarry;std::cout<<' '<<e.scoreAwards.size();for(auto award:e.scoreAwards)std::cout<<' '<<+award;
for(auto z:{s.phase,s.targetPhase,s.targetCount,s.forcedShooter,s.scoreChain,s.animation,s.spawnDelay,s.dualShotLevel})std::cout<<' '<<+z;
for(unsigned i=0;i<9;++i){const auto& a=i<6?s.particles[i].actor:s.targets[i-6];std::cout<<' '<<+a.angle<<' '<<+a.depth<<' '<<+a.spriteType<<' '<<+a.heading<<' '<<(a.visible?(a.width|(a.hit?128:0)):0)<<' '<<+a.position.x<<' '<<+a.position.y;}
for(const auto& p:s.particles)std::cout<<' '<<p.drawn;std::cout<<'\n';}}
'''
def blank():return [0,0,3,0,0,25,1,0]+[32,26,34,0,4,80,48]*6+[44,28,4,1,10,80,48,32,28,16,1,10,80,48,20,28,4,1,10,80,48]+[1]*6

def original(base,kind,values):
    state=values[:77];angle,depth,tick,count,death,rank,ret=values[77:]
    m=base.copy()
    for a,v in zip(GLOBALS,state[:8]):m[a]=v
    for i,slot in enumerate(SLOTS):
        for a,v in zip(FIELDS,state[8+7*i:15+7*i]):m[a+slot]=v
    for a,v in zip((0x51e,0x576,0x55,0x96,0xac,0xa3,0xe6,0x92),(angle,depth,tick,count,death,rank,ret,0)):m[a]=v
    drawn=state[71:77].copy();sounds=[];bonuses=[];score=[0];awards=[]
    def erase(c):
        if c.x in SLOTS[:6]:drawn[c.x-23]=0
        c.p&=~1;c.pc+=3
    def raster(c):
        if m[0x58] in SLOTS[:6]:drawn[m[0x58]-23]=1
        c.p&=~1;return_from_call(c)
    def clipped(c):
        if m[0x58] in SLOTS[:6]:drawn[m[0x58]-23]=0
        c.step()
    def skip(c):c.p&=~1;return_from_call(c)
    def award(c):score[0]+=c.a;awards.append(c.a);return_from_call(c)
    def bonus(c):bonuses.extend([m[0x4e],m[0x4f],c.y,c.p&1]);return_from_call(c)
    def sound(c):sounds.append(c.y);return_from_call(c)
    cpu=MPU(memory=m)
    invoke(cpu,{'S':0x82a7,'P':0x870c,'T':0x89f8}[kind],stop=0x831f if kind=='S' else 0x7fff,
           hooks={0x8729:erase,0x8770:erase,0x9af8:skip,0x9b98:raster,0x9b91:clipped,0x982f:award,0x8c91:bonus,0xb995:sound})
    result=0
    if kind=='S':result=2 if m[0xad]==1 and state[1]==0 else 1 if m[0xad]==255 and state[1]==0 else 0
    if kind=='P':result=int(m[0x94]!=state[7])
    out=[result,score[0],len(sounds)]+sounds+[len(bonuses)//4]+bonuses+[len(awards)]+awards+[m[a] for a in GLOBALS]
    for slot in SLOTS:out.extend(m[a+slot] for a in FIELDS)
    return out+drawn

def cases(base):
    rng=random.Random(0x870c)
    for kind in 'SPT':
        for n in range(5000):
            s=blank();inp=[rng.randrange(128),12,rng.choice([0,1,2,4,8]),rng.choice([0,2,3,5,6,20]),rng.randrange(2),rng.randrange(9),rng.randrange(2)]
            s[7]=rng.choice([0,1,255]);s[6]=rng.choice([0,1,2,255])
            if kind=='S':s[1]=rng.choice([0,0,0,1,255])
            if kind=='P':
                s[0]=rng.choice([0,1,2,12,0x8c,0x42,0x41,0xc2])
                for i in range(6):
                    s[8+7*i]=rng.randrange(64)*2
                    s[9+7*i]=12 if i<2 else 0 if s[0]&64 else 26
                if s[0]&128:s[71:77]=[0]*6
            if kind=='T':
                s[1]=n%256;s[2]=rng.choice([0,1,2,3]);s[4]=rng.choice([0,10,20]);s[5]=rng.randrange(256)
                for i in range(6,9):
                    s[8+7*i]=rng.randrange(64)*2;s[9+7*i]=rng.randrange(8,40)
                    s[12+7*i]=rng.choice([0,10,138])
            yield kind,s+inp
    # Consecutive source-state traces: spawn, warmup, middle hit, collection, exit.
    for dual in (0,1):
        s=blank();s[7]=dual;s[71:77]=[0]*6
        for i in range(6,9):s[12+7*i]=0
        inp=[32,12,0,3,0,0,1]
        yield 'S',s+inp
        s=original(base,'S',s+inp)[-77:]
        for frame in range(512):
            inp=[(frame//4*2)&126,12,frame&255,3,0,2,1]
            if frame==80:s[12+7*7]|=128
            for kind in 'TP':
                yield kind,s+inp
                s=original(base,kind,s+inp)[-77:]

def main():
    base=list((ROOT/'analysis/initialized-memory.bin').read_bytes());items=list(cases(base))
    with tempfile.TemporaryDirectory(prefix='gyrus-upgrade-') as tmp:
        p=Path(tmp);(p/'probe.cpp').write_text(RUNNER)
        subprocess.run(['g++','-std=c++20','-O2','-I',str(ROOT/'src/lib'),str(p/'probe.cpp'),str(ROOT/'src/lib/Game/Upgrade.cpp'),str(ROOT/'src/lib/Game/Polar.cpp'),'-o',str(p/'probe')],check=True)
        lines=subprocess.check_output([str(p/'probe')],input=''.join(k+' '+' '.join(map(str,v))+'\n' for k,v in items),text=True).splitlines()
    for index,((k,v),line) in enumerate(zip(items,lines)):
        exp=original(base,k,v);got=list(map(int,line.split()))
        if exp!=got:raise AssertionError(f'case {index} {k} {v}\n6502 {exp}\nC++  {got}\ndiffs {[(i,a,b) for i,(a,b) in enumerate(zip(exp,got)) if a!=b]}')
    print(f'Upgrade oracle passed: {len(items)} cases: 15000 isolated plus 2050 consecutive trace steps; zero mismatches')
if __name__=='__main__':main()
