#!/usr/bin/env python3
"""Compare portable score state with original $982F, without bitmap side effects."""
from pathlib import Path
import random
import subprocess
import tempfile
from py65.devices.mpu6502 import MPU

ROOT = Path(__file__).resolve().parents[2]

def original(memory, player, units, digits, lives):
    cpu = MPU(memory=list(memory))
    cpu.memory[0x2110:0x211c] = digits
    cpu.memory[0xaa:0xac] = lives
    cpu.memory[0x92] = player
    cpu.memory[0x57] = 1
    cpu.pc, cpu.a, cpu.x, cpu.sp, cpu.p = 0x982f, units, 0, 0xfd, 0x30
    cpu.memory[0x1fe:0x200] = [0xfe, 0x7f]
    for _ in range(2000):
        if cpu.pc == 0x98bc:  # life glyph update, preserving effect on X/Y via actual code
            pass
        if cpu.pc == 0x987c:  # score is complete; the remainder only draws HUD
            return bytes(cpu.memory[0x2110:0x211c] + cpu.memory[0xaa:0xac])
        cpu.step()
    raise RuntimeError('Original score routine failed to reach HUD update')

def main():
    memory = (ROOT / 'analysis/loaded-memory.bin').read_bytes()
    rng = random.Random(20261104)
    cases = []
    for player in (0,1):
        for units in range(256):
            for boundary in ([0]*12, [9]*12, [0,0,5,9,9,5]*2):
                cases.append((player,units,list(boundary),[5,5]))
    for _ in range(2048):
        cases.append((rng.randrange(2),rng.randrange(32),
                      [rng.randrange(10) for _ in range(12)],
                      [rng.randrange(256),rng.randrange(256)]))
    source = r'''
#include <Game/Score.hpp>
#include <iostream>
int main() {
  unsigned player,units;
  while (std::cin >> player >> units) {
    gyrus::game::ScoreBoard board;
    unsigned value;
    for (auto& p:board.players) for(auto& d:p.digits) {std::cin>>value;d=value;}
    for (auto& p:board.players) {std::cin>>value;p.lives=value;}
    board.add(player,units);
    for (auto& p:board.players) for(auto d:p.digits) std::cout.put(d);
    for (auto& p:board.players) std::cout.put(p.lives);
  }
}
'''
    with tempfile.TemporaryDirectory(prefix='gyrus-score-') as temp:
        path = Path(temp)
        (path/'runner.cpp').write_text(source)
        subprocess.run(['c++','-std=c++20','-O2','-I'+str(ROOT/'src/lib'),
                        str(path/'runner.cpp'),str(ROOT/'src/lib/Game/Score.cpp'),
                        '-o',str(path/'runner')],check=True)
        data=''.join(' '.join(map(str,[p,u,*d,*l]))+'\n' for p,u,d,l in cases)
        actual=subprocess.check_output([str(path/'runner')],input=data.encode())
    assert len(actual)==14*len(cases)
    for index,case in enumerate(cases):
        expected=original(memory,*case)
        got=actual[index*14:index*14+14]
        if got!=expected:
            raise AssertionError(f'{case}: native={list(got)}, original={list(expected)}')
    print(f'Score oracle passed: {len(cases)} cases, zero mismatches')

if __name__=='__main__': main()
