#!/usr/bin/env python3
"""Compare the typed enemy wave decoder with original $9634 (py65 1.2.0).

No graphics/audio interception is needed for preparation. Both writable ATR
descriptors and ignored cartridge-ROM writes are tested across repeated calls.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile

from py65.devices.mpu6502 import MPU

RUNNER = r'''
#include <Game/Enemies.hpp>
#include <iostream>
using namespace gyrus::game;
int main() {
  unsigned stage,wave,normal,random,writable,repetitions;
  while (std::cin>>stage>>wave>>normal>>random>>writable>>repetitions) {
    EnemyPaths p;
    for (unsigned n=0;n<repetitions;++n) {
      prepareWave(p,{static_cast<uint8_t>(stage),static_cast<uint8_t>(wave),bool(normal),
                    static_cast<uint8_t>(random),bool(writable)});
      std::cout << +p.incomingCount << ' ' << +p.nextWave << ' ' << +p.programLength
                << ' ' << +p.appearanceDelay << ' ' << +p.cadence << ' ' << +p.evenTimer
                << ' ' << +p.oddTimer << ' ' << +p.collisionMask << ' ' << +p.forcedSprite
                << ' ' << +p.scoreChain << ' ' << +p.preparedCount;
      for(auto v:p.cursors) std::cout << ' ' << +v;
      for(auto v:p.phases) std::cout << ' ' << +v;
      for(const auto& lane:p.tracks)
        for(const auto& point:lane)
          std::cout << ' ' << +point.depth << ' ' << +point.angle << ' ' << +point.style;
      for(auto v:p.descriptors) std::cout << ' ' << +v;
      std::cout << '\n';
    }
  }
}
'''


class Memory(list):
    writable = True

    def __setitem__(self, index, value):
        if not self.writable and isinstance(index, int) and 0x945f <= index < 0x951f:
            return
        super().__setitem__(index, value)


TRAJECTORY_RUNNER = r'''
#include <Game/Enemies.hpp>
#include <iostream>
using namespace gyrus::game;
int main() {
  unsigned stage,wave,normal,level,retreat,hits,schedule;
  while(std::cin>>stage>>wave>>normal>>level>>retreat>>hits>>schedule) {
    EnemyPaths p; EnemyState s; p.scoreChain=2;
    prepareWave(p,{static_cast<uint8_t>(stage),static_cast<uint8_t>(wave),bool(normal)});
    for(unsigned tick=0;tick<260;++tick) {
      if(hits && tick%17==0)
        for(auto& a:s.incoming) if(a.visible) { a.hit=true; break; }
      EnemyInputs input;
      input.normalStage=normal; input.levelNumber=level; input.playerAngle=(tick*2)&127;
      input.deathCountdown=retreat && tick>=40 && tick<90;
      const auto e=schedule?updateEnemyEntries(p,s,input):updateIncomingLane(p,s,tick&1,input);
      unsigned sounds=0,bonuses=0,awards=0,carryBits=0;
      for(auto v:e.sounds) sounds=sounds*16+v+1;
      for(auto b:e.bonuses) {bonuses=bonuses*65536+(b.angle<<8)+(b.depth<<1)+b.label;carryBits=carryBits*2+b.incomingCarry;}
      for(auto units:e.scoreAwards) awards=awards*257+units+1;
      std::cout << +p.incomingCount << ' ' << +s.formationCount << ' ' << +s.nextFormationSlot
                << ' ' << +s.remainingBonusEnemies << ' ' << +p.scoreChain << ' ' << +p.collisionMask
                << ' ' << +p.evenTimer << ' ' << +p.oddTimer << ' ' << e.scoreUnits
                << ' ' << sounds << ' ' << bonuses << ' ' << +p.appearanceDelay << ' ' << awards << ' ' << carryBits;
      for(auto v:p.cursors) std::cout << ' ' << +v;
      for(auto v:p.phases) std::cout << ' ' << +v;
      for(const auto& group:{s.incoming.data(),s.formation.data()}) {
        const unsigned count=group==s.incoming.data()?10:36;
        for(unsigned i=0;i<count;++i) {
          const auto& a=group[i];
          std::cout << ' ' << +a.angle << ' ' << +a.depth << ' ' << +a.heading << ' ' << +a.spriteType
                    << ' ' << (a.visible ? a.width|(a.hit?128:0):0)
                    << ' ' << +a.position.x << ' ' << +a.position.y << ' ' << +a.formationSlot;
        }
      }
      std::cout << '\n';
    }
  }
}
'''


def trajectory_oracle(root, compiler):
    original = list((root/'analysis/initialized-memory.bin').read_bytes())
    cases = []
    for stage in [0, 16, 40]:
        for wave in range(4):
            for normal in range(2):
                for situation in range(3):
                    cases.append([stage,wave,normal,1 if situation<2 else 9,
                                  int(situation==1),int(situation==2),0])
    cases.extend([[0,0,1,1,0,0,1],[40,3,1,9,0,1,1],[16,1,0,1,1,0,1]])
    with tempfile.TemporaryDirectory(prefix='gyrus-enemy-path-oracle-') as temporary:
        folder = Path(temporary)
        source,binary=folder/'runner.cpp',folder/'runner'
        source.write_text(TRAJECTORY_RUNNER)
        subprocess.run([compiler,'-std=c++20','-O2','-I'+str(root/'src/lib'),str(source),
                        str(root/'src/lib/Game/Enemies.cpp'),str(root/'src/lib/Game/Polar.cpp'),
                        '-o',str(binary)],check=True)
        lines=iter(subprocess.check_output([str(binary)],text=True,
                   input=''.join(' '.join(map(str,c))+'\n'for c in cases)).splitlines())
    checked=0
    for stage,wave,normal,level,retreat,hits,schedule in cases:
        m=original.copy()
        for a in range(0x1a,0x2e):m[a]=0
        for a in range(0x6d6,0x9d6):m[a]=0
        for index in range(39,88):
            for base in [0x46e,0x4c6,0x51e,0x576,0x5ce,0x626,0x67e]:m[base+index]=0
        for i in range(36):
            m[0x552+i]=[116,106,96,84,74,64,52,42,32,20,10,0][i%12]
            m[0x5aa+i]=52-(i//12)*4
            m[0x6b2+i]=22
        for a in range(0xdc,0xe6):m[a]=0
        m[0x92],m[0x9d],m[0xa0],m[0xd9],m[0xcd]=0,stage,wave,normal,2
        m[0x99],m[0x96],m[0xf5],m[0xda],m[0xe6],m[0xa3]=0,0,0,40,0,level
        m[0x57]=1
        m[0x210b:0x2117]=[0]*12
        cpu=MPU(memory=m)
        score,sounds,bonuses,awards,carry_bits=0,0,0,0,0

        def ret():cpu.pc=(cpu.stPopWord()+1)&65535

        def run(address,hooks=False,stop=0x7fff):
            nonlocal score,sounds,bonuses,awards,carry_bits
            cpu.pc,cpu.sp,cpu.p=address,0xfd,0x30
            m[0x1fe],m[0x1ff]=0xfe,0x7f
            for _ in range(50000):
                if cpu.pc==stop:return
                if hooks and cpu.pc==0x9af8:
                    cpu.p &= ~1
                    ret()
                elif hooks and cpu.pc==0x9b98:
                    cpu.p &= ~1
                    ret()
                elif hooks and cpu.pc==0x982f:
                    score += cpu.a
                    awards=(awards*257+cpu.a+1)&0xffffffff
                    cpu.step() # Execute score code to preserve returned carry for $900D.
                elif hooks and cpu.pc==0xb995:
                    sounds=(sounds*16+cpu.y+1)&0xffffffff
                    ret()
                elif hooks and cpu.pc==0x8c91:
                    bonuses=(bonuses*65536+(m[0x4e]<<8)+(m[0x4f]<<1)+cpu.y)&0xffffffff
                    carry_bits=(carry_bits*2+(cpu.p&1))&0xffffffff
                    ret()
                else:cpu.step()
            raise RuntimeError(f'Enemy update stuck at ${cpu.pc:04X}')
        run(0x9634)
        for tick in range(260):
            if hits and tick%17==0:
                for index in range(10):
                    if m[0x5f5+index]:
                        m[0x5f5+index] |= 128
                        break
            m[0x51e]=(tick*2)&127
            m[0xac]=int(retreat and 40<=tick<90)
            score,sounds,bonuses,awards,carry_bits=0,0,0,0,0
            if schedule:run(0x85ff,True,0x861a)
            else:run(0x8f8e if tick&1 else 0x8f96,True)
            expected=[m[a]for a in [0x99,0x96,0xf5,0xda,0xcd,0xf9,0xef,0xf1]]
            expected.extend([score,sounds,bonuses,m[0xa5],awards,carry_bits])
            expected.extend(m[0x1a:0x24]);expected.extend(m[0x24:0x2e])
            for indices in [range(39,49),range(52,88)]:
                for index in indices:
                    expected.extend(m[base+index]for base in [0x51e,0x576,0x626,0x67e,0x5ce,0x46e,0x4c6])
                    expected.append(m[0x2e+index-39]if index<49 else 0)
            actual=list(map(int,next(lines).split()))
            if expected!=actual:
                mismatch=[(i,a,b)for i,(a,b)in enumerate(zip(expected,actual))if a!=b]
                raise AssertionError(f'trajectory stage={stage},wave={wave},normal={normal},level={level},'
                                     f'retreat={retreat},hits={hits},schedule={schedule},tick={tick}: '
                                     f'(index,6502,C++) {mismatch[:30]}')
            checked+=1
    if next(lines,None)is not None:raise AssertionError('Unexpected trajectory output')
    print(f'Enemy path-consumer oracle passed: {checked} group updates, zero mismatches')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--memory', type=Path, default=Path('analysis/loaded-memory.bin'))
    parser.add_argument('--compiler', default='c++')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    original = list(args.memory.read_bytes())
    if len(original) != 65536:
        raise ValueError('Expected full extracted address space')
    cases = []
    for stage in range(0, 48, 8):
        for wave in range(4):
            for normal in range(2):
                for writable in range(2):
                    cases.append([stage, wave, normal, 0, writable, 3])
    for random in range(24):
        for normal in range(2):
            for writable in range(2):
                cases.append([40, 4, normal, random, writable, 3])
    with tempfile.TemporaryDirectory(prefix='gyrus-enemy-oracle-') as temporary:
        folder = Path(temporary)
        source, binary = folder/'runner.cpp', folder/'runner'
        source.write_text(RUNNER)
        subprocess.run([args.compiler, '-std=c++20', '-O2', '-I'+str(root/'src/lib'),
                        str(source), str(root/'src/lib/Game/Enemies.cpp'),
                        str(root/'src/lib/Game/Polar.cpp'), '-o', str(binary)], check=True)
        output = iter(subprocess.check_output([str(binary)], text=True,
                      input=''.join(' '.join(map(str,c))+'\n' for c in cases)).splitlines())
    tested = 0
    for stage, wave, normal, random, writable, repetitions in cases:
        memory = Memory(original)
        memory.writable = bool(writable)
        cpu = MPU(memory=memory)
        for repetition in range(repetitions):
            memory[0x92], memory[0x9d], memory[0xa0], memory[0xd9] = 0, stage, wave, normal
            memory[0xd20a] = random
            cpu.pc, cpu.sp, cpu.p = 0x9634, 0xfd, 0x30
            memory[0x1fe], memory[0x1ff] = 0xfe, 0x7f
            for _ in range(30000):
                cpu.step()
                if cpu.pc == 0x7fff:
                    break
            else:
                raise RuntimeError(f'Wave failed to terminate at ${cpu.pc:04X}')
            expected = [memory[a] for a in [0x99,0xa0,0xcb,0xa5,0xf0,0xef,0xf1,0xf9,0xea,0xcd,0x9f]]
            expected.extend(memory[0x1a:0x24])
            expected.extend(memory[0x24:0x2e])
            for lane in range(2):
                for index in range(128):
                    expected.extend(memory[base+lane*128+index] for base in [0x6d6,0x7d6,0x8d6])
            expected.extend(memory[0x945f:0x951f])
            actual = list(map(int, next(output).split()))
            if expected != actual:
                mismatch = [(i,a,b)for i,(a,b)in enumerate(zip(expected,actual)) if a!=b]
                raise AssertionError(f'stage={stage}, wave={wave}, normal={normal}, random={random}, '
                                     f'writable={writable}, repeat={repetition}: '
                                     f'(index,6502,C++) {mismatch[:20]}')
            tested += 1
    if next(output, None) is not None:
        raise AssertionError('Unexpected extra native output')
    print(f'Enemy wave preparation oracle passed: {tested} calls, zero mismatches')
    trajectory_oracle(root,args.compiler)


if __name__ == '__main__':
    main()
