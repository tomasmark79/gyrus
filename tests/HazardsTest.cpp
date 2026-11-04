#include <Game/Hazards.hpp>
#include <cassert>
#include <iostream>
using namespace gyrus::game;
int main(){
  Hazards s; HazardSpawnInputs in; in.level=5;
  unsigned reads=0; in.randomByte=[&](){++reads;return 0;};
  assert(spawnHazards(s,in) && reads==3 && s.count==255);
  assert(s.actors[0].angle==28 && s.actors[1].angle==14 && s.actors[2].angle==0);
  updateHazards(s,15); assert(!s.actors[0].visible && s.count==255);
  updateHazards(s,16); assert(s.count==3 && s.actors[0].angle==26);
  assert(s.actors[0].depth==48 && s.actors[2].visible);
  s.actors[2].hit=true;
  auto event=updateHazards(s,32);
  assert(event.scoreUnits==2 && s.count==1 && s.cooldown==180);
  assert(!s.actors[0].visible && !s.actors[2].visible && s.actors[1].visible);
  assert(event.sounds.empty());
  s.cooldown=2;reads=0;assert(!spawnHazards(s,in) && s.cooldown==1 && reads==0);
  std::cout<<"Hazard tests passed\n";
}
