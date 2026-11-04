#include <Game/Stage.hpp>
#include <cassert>
#include <iostream>
using namespace gyrus::game;
int main() {
  Turns t; EnemyPaths p; EnemyState e; FormationControl f;
  f.frameCounter=79; f.gridPhase=3;
  e.formation[0].angle=60; e.formation[0].depth=50;
  t.deathCountdown=255; t.players[0].savedFormation=12;
  t.players[0].savedWave=4;
  auto s=initializeStage(t,p,e,f);
  assert(s.resumed && s.music==3 && s.messageOffset==20);
  assert(p.scoreChain==2 && p.nextWave==4 && e.formationCount==12);
  assert(e.nextFormationSlot==12 && e.formation[0].angle==60);
  assert(e.formation[0].depth==50 && e.formation[0].visible);
  assert(!e.formation[12].visible && f.frameCounter==79 && f.gridPhase==3);
  assert(s.enemyShotCap==1 && s.enemyShotCooldownReload==58);
  t.players[0].warps=0; t.players[0].special=0;
  s=initializeStage(t,p,e,f);
  assert(!s.normalStage && s.messageOffset==40 && s.music==2);
  assert(t.players[0].special==3 && s.upgradeTargetCount==0);
  std::cout<<"Stage setup tests passed\n";
}
