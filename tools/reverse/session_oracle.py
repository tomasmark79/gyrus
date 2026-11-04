#!/usr/bin/env python3
"""Sequential original DLI + main-loop versus portable GameSession.
Run `--ticks N`; first mismatch is reported with CPU-addressed sentinel names.
This profile holds joystick neutral/fire released and disables player damage.
"""
import argparse, subprocess, tempfile
from pathlib import Path
from py65.devices.mpu6502 import MPU
ROOT=Path(__file__).resolve().parents[2]
RUNNER=r'''
#include <Game/GameSession.hpp>
#include <iostream>
using namespace gyrus::game;
int main(int argc,char**argv){GameSession s;s.damageEnabled=false;s.begin();
for(int i=0;i<std::stoi(argv[1]);++i){s.step(15,false);
for(unsigned v:{unsigned(s.tick),unsigned(s.formation.frameCounter),unsigned(s.formation.gridPhase),unsigned(s.gates.messageCountdown),unsigned(s.paths.nextWave),unsigned(s.paths.incomingCount),unsigned(s.enemies.formationCount),unsigned(s.enemies.nextFormationSlot),unsigned(s.paths.collisionMask),unsigned(s.paths.scoreChain),unsigned(s.paths.appearanceDelay),unsigned(s.paths.evenTimer),unsigned(s.paths.oddTimer),unsigned(s.enemyShots.activeCount),unsigned(s.enemyShots.cooldown),unsigned(s.upgrade.spawnDelay),unsigned(s.upgrade.targetPhase),unsigned(s.upgrade.targetCount),unsigned(s.upgrade.phase),unsigned(s.player.angle),unsigned(s.player.depth),unsigned(s.player.aimAngle),unsigned(s.gates.transition),unsigned(s.gates.deathCountdown),unsigned(s.paths.preparedCount)})std::cout<<v<<' ';
std::cout<<s.randomState<<' ';
for(auto v:s.paths.cursors)std::cout<<+v<<' ';for(auto v:s.paths.phases)std::cout<<+v<<' ';for(auto v:s.formation.attackState)std::cout<<+v<<' ';
for(auto& a:s.enemies.incoming)std::cout<<+a.angle<<' '<<+a.depth<<' '<<+a.spriteType<<' '<<(a.visible?(a.width|(a.hit?128:0)):0)<<' ';
for(auto& a:s.enemies.formation)std::cout<<+a.angle<<' '<<+a.depth<<' '<<(a.visible?(a.width|(a.hit?128:0)):0)<<' ';
std::cout<<'\n';}}
'''
class Memory(list):
 def __init__(self,data):super().__init__(data);self.rng=0x12345678;self.calls=0;self.reads=[]
 def __getitem__(self,key):
  if key==0xd20a:self.rng=(self.rng*1664525+1013904223)&0xffffffff;self.calls+=1;self.reads.append((self.cpu.pc,self.rng>>24));return self.rng>>24
  if key==0xd300:return 255
  if key in [0xd010,0xd011]:return 1
  if key==0xd01f:return 7
  if key==0xd40b:return 128
  if key in [0xd004,0xd005,0xd006]:return 0
  return super().__getitem__(key)
def main():
 parser=argparse.ArgumentParser();parser.add_argument('--ticks',type=int,default=5000);args=parser.parse_args()
 with tempfile.TemporaryDirectory(prefix='gyrus-session-')as temp:
  p=Path(temp);(p/'run.cpp').write_text(RUNNER)
  subprocess.run(['g++','-std=c++20','-O2','-I'+str(ROOT/'src/lib'),str(p/'run.cpp'),*[str(f)for f in (ROOT/'src/lib/Game').glob('*.cpp')],'-o',str(p/'run')],check=True)
  actual=subprocess.check_output([str(p/'run'),str(args.ticks)],text=True).splitlines()
 data=list((ROOT/'analysis/initialized-memory.bin').read_bytes());loaded=(ROOT/'analysis/loaded-memory.bin').read_bytes();data[0x6000:0xc000]=loaded[0x6000:0xc000]
 m=Memory(data);cpu=MPU(memory=m);m.cpu=cpu
 for a in range(256):m[a]=0
 for a in range(0x46e,0x9d6):m[a]=0
 for i in range(36):m[0x552+i]=[116,106,96,84,74,64,52,42,32,20,10,0][i%12];m[0x5aa+i]=52-(i//12)*4;m[0x6b2+i]=22
 for a,v in {0x57:5,0xa3:1,0xa4:1,0x9b:2,0x9c:2,0xaa:5,0xab:0,0xf7:5,0xf8:5}.items():m[a]=v
 def run(start,stop):
  cpu.pc=start;cpu.sp=253;m[0x1fe]=0xfe;m[0x1ff]=0x7f
  for _ in range(300000):
   if cpu.pc==stop:return
   if cpu.pc in [0x9b98,0x9b9c,0xb901,0xb995,0xbc16,0x8405]:
    if cpu.pc in [0x9b98,0x9b9c]:cpu.p&=~1
    cpu.pc=(cpu.stPopWord()+1)&65535
   else:cpu.step()
  raise RuntimeError(f'CPU failed to reach {stop:04X}, at {cpu.pc:04X}')
 run(0x92e6,0xa01d)
 addresses=[0x55,0x56,0x5b,0x9a,0xa0,0x99,0x96,0xf5,0xf9,0xcd,0xa5,0xef,0xf1,0xa9,0xa7,0xaf,0xad,0xb1,0xdb,0x51e,0x576,0xb2,0xb8,0xac,0x9f]
 names=[f'${a:04X}'for a in addresses]+['RNG state']
 names += [f'${a:04X}'for a in range(0x1a,0x2e)]+[f'${a:04X}'for a in range(0xdc,0xe6)]
 names += [f'object{i}/${base+i:04X}'for i in range(39,49)for base in [0x51e,0x576,0x67e,0x5ce]]
 names += [f'object{i}/${base+i:04X}'for i in range(52,88)for base in [0x51e,0x576,0x5ce]]
 waves=[]
 for tick,line in enumerate(actual,1):
  run(0x8520,0x86a0);run(0x806f,0x8069)
  want=[m[a]for a in addresses]+[m.rng]+m[0x1a:0x2e]+m[0xdc:0xe6]
  want += [m[base+i]for i in range(39,49)for base in [0x51e,0x576,0x67e,0x5ce]]
  want += [m[base+i]for i in range(52,88)for base in [0x51e,0x576,0x5ce]]
  got=list(map(int,line.split()));assert len(got)==len(want)
  if not waves or waves[-1][1]!=m[0xa0]:waves.append((tick,m[0xa0]))
  if got!=want:
   diff=[f'{names[i]}: C++={a}, CPU={b}'for i,(a,b)in enumerate(zip(got,want))if a!=b]
   print('recent RNG reads',m.reads[-40:]);raise AssertionError(f'First mismatch tick {tick}, RNG reads={m.calls}; '+ '; '.join(diff[:15]))
 print(f'Session oracle passed: {args.ticks} DLI/main ticks, {len(names)} sentinels/tick; waves={waves}; RNG reads={m.calls}')
if __name__=='__main__':main()
