#!/usr/bin/env python3
"""Differential tests for enemy-shot movement/spawns and the player-death gate.

Original projection/descriptor/clipping executes; bitmap erase/raster writes
are intercepted. Allocation outside slots7..11 is an explicit boundary result.
Requires py65==1.2.0 and the captured initialized-memory.bin.
"""
from pathlib import Path
import random
import subprocess
import tempfile
from py65.devices.mpu6502 import MPU

ROOT=Path(__file__).resolve().parents[2]
RUNNER=r'''
#include <Game/EnemyShots.hpp>
#include <iostream>
using namespace gyrus::game;
void readState(EnemyShots& state) {
    unsigned a,b,c,d,e,f,g;
    std::cin>>a>>b>>c>>d;
    state.activeCount=a;state.cooldown=b;state.forcedShooter=c;state.specialCountdown=d;
    for(auto& shot:state.shots) {
        std::cin>>a>>b>>c>>d>>e>>f>>g;
        shot.angle=a;shot.depth=b;shot.spriteType=c;shot.heading=d;
        shot.active=e!=0;shot.pendingDraw=e&128;shot.width=e&127;
        shot.position={static_cast<unsigned char>(f),static_cast<unsigned char>(g),false};
    }
}
void writeState(const EnemyShots& state) {
    std::cout<<+state.activeCount<<' '<<+state.cooldown<<' '<<+state.forcedShooter<<' '<<+state.specialCountdown;
    for(const auto& s:state.shots)
        std::cout<<' '<<+s.angle<<' '<<+s.depth<<' '<<+s.spriteType<<' '<<+s.heading<<' '
                 <<(s.active ? s.width | (s.pendingDraw?128:0):0)<<' '<<+s.position.x<<' '<<+s.position.y;
}
int main() {
    char kind;
    while(std::cin>>kind) {
        unsigned a,b,c,d,e,f,g;
        if(kind=='H') {
            std::cin>>a>>b>>c>>d>>e>>f>>g;
            unsigned normal,mask;std::cin>>normal>>mask;
            PlayerGates gates;gates.deathCountdown=a;gates.transition=b;gates.explosion=c;
            PlayerHitInputs input{{static_cast<unsigned char>(d),static_cast<unsigned char>(e),static_cast<unsigned char>(f)},
                                 static_cast<unsigned char>(mask),bool(normal)};
            gates.messageCountdown=g;
            const auto result=resolvePlayerHit(gates,input);
            std::cout<<result.collisionLatchCleared<<' '<<result.deathStarted<<' '
                     <<+gates.deathCountdown<<' '<<+gates.explosion<<'\n';
            continue;
        }
        EnemyShots state;readState(state);
        unsigned result=0,slot=255,reads=0;
        if(kind=='U') {std::cin>>a;result=updateEnemyShots(state,a);}
        else if(kind=='S') {std::cin>>a>>b>>c>>d;result=spawnSpecialEnemyShot(state,a,b,c,d);}
        else {
            std::cin>>a>>b>>c>>d>>e>>f;
            std::array<EnemyShooter,49> sources{};
            for(auto& source:sources) {
                unsigned angle,depth,active,hit;std::cin>>angle>>depth>>active>>hit;
                source={static_cast<unsigned char>(angle),static_cast<unsigned char>(depth),bool(active),bool(hit)};
            }
            EnemyShotSpawnInputs input{static_cast<unsigned char>(a),static_cast<unsigned char>(b),
                static_cast<unsigned char>(c),bool(d),[&]{return static_cast<unsigned char>(reads++==0?e:f);}};
            const auto spawned=spawnEnemyShot(state,sources,input);
            result=static_cast<unsigned>(spawned.status);slot=spawned.index;
        }
        std::cout<<result<<' '<<slot<<' '<<reads<<' ';writeState(state);std::cout<<'\n';
    }
}
'''


def return_from_call(cpu): cpu.pc=(cpu.stPopWord()+1)&65535


def invoke(cpu,address,stop=0x7fff,hooks=None):
    cpu.pc,cpu.sp,cpu.p=address,0xfd,0x30
    cpu.memory[0x1fe:0x200]=[0xfe,0x7f]
    for _ in range(30000):
        if cpu.pc==stop:return
        if hooks and cpu.pc in hooks:hooks[cpu.pc](cpu)
        else:cpu.step()
    raise RuntimeError(f'Original routine {address:04x} failed at {cpu.pc:04x}')


def blank_state():
    return [0,0,0,0]+[0,0,0x26,0,0,80,48]*5


def state_to_memory(state,m):
    for address,value in zip((0xa9,0xa7,0xb3,0xb4),state[:4]):m[address]=value
    for slot in range(7,12):
        values=state[4+(slot-7)*7:11+(slot-7)*7]
        for address,value in zip((0x51e,0x576,0x67e,0x626,0x5ce,0x46e,0x4c6),values):m[address+slot]=value


def memory_state(m):
    result=[m[address] for address in (0xa9,0xa7,0xb3,0xb4)]
    for slot in range(7,12):
        result.extend(m[address+slot] for address in (0x51e,0x576,0x67e,0x626,0x5ce,0x46e,0x4c6))
    return result


def original(base,kind,values):
    cpu=MPU(memory=base.copy());m=cpu.memory
    if kind=='H':
        death,transition,explosion,p0,p1,p2,message,normal,mask=values
        m[0xac],m[0xb8],m[0xd8],m[0x9a],m[0xd9],m[0xf9]=death,transition,explosion,message,normal,mask
        m[0xd004:0xd007]=[p0,p1,p2]
        events=[False,False]
        def latch(c):events[0]=True;c.step()
        def sound(c):
            if c.y!=0:raise AssertionError('Unexpected death sound')
            events[1]=True;return_from_call(c)
        def explode(c):
            if c.y!=128:raise AssertionError('Unexpected explosion mode')
            c.memory[0xd8]=0x88;return_from_call(c)
        if death==0:invoke(cpu,0x85c8,0x85fc,{0x85d9:latch,0xb995:sound,0xa01d:explode})
        return [*map(int,events),m[0xac],m[0xd8]]
    state_to_memory(values[:39],m)
    args=values[39:];result=0;slot=255;reads=0
    if kind=='U':
        tick=args[0];m[0x56]=tick
        active=[bool(m[0x5ce+s]) for s in range(7,12)]
        def raster(c):c.p&=~1;return_from_call(c)
        invoke(cpu,0x8913,hooks={0x9af8:return_from_call,0x9b98:raster})
        result=sum(1<<i for i,s in enumerate(range(7,12)) if active[i] and not m[0x5ce+s])
    elif kind=='S':
        reload,angle,transition,normal=args
        m[0xf6],m[0x51e],m[0xb8],m[0xd9]=reload,angle,transition,normal
        def spawned(c):
            nonlocal result
            result=1;c.step()
        invoke(cpu,0x831f,0x8353,{0x834e:spawned})
    else:
        cap,reload,aim,normal,r0,r1=args[:6]
        m[0xa6],m[0xa8],m[0xb2],m[0xd9]=cap,reload,aim,normal
        for i in range(49):
            angle,depth,active,hit=args[6+i*4:10+i*4]
            m[0x51e+39+i]=angle;m[0x576+39+i]=depth;m[0x5ce+39+i]=(3|(128 if hit else 0)) if active else 0
        def random(c):
            nonlocal reads
            m[0xd20a]=r0 if reads==0 else r1;reads+=1;c.step()
        def allocated(c):
            nonlocal result,slot
            result=1;slot=c.y-7;c.step()
        def boundary(c):
            nonlocal result
            if c.y==12:result=2;c.pc=0x82a7
            else:c.step()
        invoke(cpu,0x821f,0x82a7,{0x8269:random,0x827a:random,0x829c:allocated,0x8272:boundary})
    return [result,slot,reads,*memory_state(m)]


def cases():
    rng=random.Random(0x8913)
    for angle in range(128):
        for depth in range(0,64,2):
            for pending in (False,True):
                state=blank_state();state[0]=1
                state[4:11]=[angle,depth,0x26,99,128 if pending else 3,80,48]
                yield 'U',state+[rng.choice([0,4,8,12])]
    for _ in range(2048):
        state=blank_state();state[0]=rng.randrange(7)
        for i in range(5):
            state[4+i*7:11+i*7]=[rng.randrange(128),rng.randrange(64),rng.choice([0x18,0x24,0x26]),
                rng.randrange(8),rng.choice([0,2,3,4,128]),rng.randrange(160),rng.randrange(96)]
        yield 'U',state+[rng.randrange(256)]
    for angle in range(128):
        for delta in (-7,-4,-3,-2,-1,0,1,2,3,4,7):
            for random_offset in (0,2,4,6):
                state=blank_state()
                sources=[angle,40,1,0]+[0,0,0,0]*48
                yield 'N',state+[4,17,(angle+delta)&127,1,0,random_offset]+sources
    for _ in range(2048):
        state=blank_state();state[:4]=[rng.randrange(7),rng.choice([0,0,7]),rng.choice([0,0,1,2,3]),0]
        for i in range(5):state[8+i*7]=rng.choice([0,0,3])
        aim=rng.randrange(128)
        sources=[]
        for _ in range(49):sources.extend([(aim+rng.randrange(-3,4))&127,rng.randrange(10,64),rng.randrange(2),rng.randrange(2)])
        yield 'N',state+[rng.randrange(5),rng.randrange(256),aim,rng.randrange(2),rng.randrange(256),rng.randrange(256)]+sources
    for countdown in range(256):
        for reload in (0,1,2,31,255):
            for flags in range(8):
                state=blank_state();state[0]=rng.randrange(7);state[3]=countdown;state[36]=3 if flags&1 else 0
                yield 'S',state+[reload,rng.randrange(128),1 if flags&2 else 0,1 if flags&4 else 0]
    for combined in range(16):
        for mask in range(16):
            for gates in range(8):
                # Split bits across all three collision inputs, preserving their OR.
                yield 'H',[1 if gates&1 else 0,1 if gates&2 else 0,rng.randrange(256),
                           combined&5,combined&6,combined&12,rng.randrange(2),1 if gates&4 else 0,mask]


def main():
    base=list((ROOT/'analysis/initialized-memory.bin').read_bytes())
    if len(base)!=65536:raise ValueError('Expected initialized 64KB reference image')
    test_cases=list(cases())
    data=''.join(kind+' '+' '.join(map(str,values))+'\n' for kind,values in test_cases)
    with tempfile.TemporaryDirectory(prefix='gyrus-enemy-shots-') as tmp:
        source=Path(tmp)/'probe.cpp';source.write_text(RUNNER);binary=Path(tmp)/'probe'
        subprocess.run(['g++','-std=c++20','-O2','-Wall','-Wextra','-Werror','-I',str(ROOT/'src/lib'),str(source),
                        str(ROOT/'src/lib/Game/EnemyShots.cpp'),str(ROOT/'src/lib/Game/Polar.cpp'),'-o',str(binary)],check=True)
        lines=subprocess.check_output([str(binary)],input=data,text=True).splitlines()
    if len(lines)!=len(test_cases):raise AssertionError('Result count mismatch')
    counts={kind:0 for kind in 'UNSH'}
    for (kind,values),line in zip(test_cases,lines):
        expected=original(base,kind,values);actual=list(map(int,line.split()))
        if expected!=actual:raise AssertionError(f'{kind} {values}\n6502 {expected}\nC++  {actual}')
        counts[kind]+=1
    print(f'Enemy-shot oracle passed: movement={counts["U"]}, normal spawn={counts["N"]}, '
          f'special spawn={counts["S"]}, player death gate={counts["H"]}; zero mismatches')
    print('Raster writes and explosion setup intercepted; player pixel-collision generation and pool overflow remain outside scope')


if __name__=='__main__':main()
