#include "Hazards.hpp"
#include <stdexcept>
namespace gyrus::game {
namespace {
void hide(EnemyActor& actor) { actor.visible=false; actor.hit=false; actor.width=0; }
bool eraseClipped(const EnemyActor& actor) {
  const auto half=actor.width/2;
  const auto left=actor.position.x>half ? actor.position.x-half : 0;
  const auto top=actor.position.y>half ? actor.position.y-half : 0;
  return !actor.width || left+actor.width>=160 || top+actor.width>=96;
}
}
bool spawnHazards(Hazards& state, const HazardSpawnInputs& in) {
  if (state.cooldown && --state.cooldown) return false;
  if (state.count || in.messageCountdown || in.transition || !in.normalStage || in.level<5)
    return false;
  if (!in.randomByte) throw std::invalid_argument("Hazard spawn requires random byte source");
  if (in.randomByte() || (in.randomByte()&7)) return false;
  auto angle=static_cast<std::uint8_t>(in.randomByte()&112);
  for (int i=2;i>=0;--i) {
    state.actors[i].angle=angle;
    state.actors[i].depth=50;
    angle=(angle+14)&126; // First ADC carries1, removed by the even-angle mask.
  }
  state.actors[0].spriteType=20;
  state.actors[1].spriteType=18;
  state.actors[2].spriteType=26;
  state.direction=2;
  --state.count;
  return true;
}
EnemyEvents updateHazards(Hazards& state, std::uint8_t tick) {
  EnemyEvents events;
  auto& center=state.actors[0];
  const auto remove=[&](EnemyActor& actor) {
    --state.count;state.cooldown=180;hide(actor);
  };
  if (!(tick&15)) {
    for (int i=2;i>=1;--i) {
      auto& actor=state.actors[i];
      if (!(state.count&128)) {
        if (!actor.visible) continue;
        if (eraseClipped(actor)) hide(actor); // Original erase clears width on clipping.
        if (actor.hit) {
          events.award(2);
          remove(actor);
          if (center.visible) center.hit=true;
          continue;
        }
      }
      actor.depth-=2;
      if (!(actor.depth&128)) {
        center.depth=actor.depth;
        if (projectEnemyActor(actor)) continue;
      }
      remove(actor);
      if (center.visible) center.hit=true;
    }
  }
  if (state.count&128) {
    if (tick&15) return events;
    state.count=3;
  } else {
    if (!center.visible) return events;
    if (eraseClipped(center) || center.hit) { remove(center);return events; }
  }
  if (state.direction<1 || state.direction>2)
    throw std::out_of_range("Hazard direction must be original index1 or2");
  center.angle=(center.angle+(state.direction==1?2:254))&126;
  if (center.angle==state.actors[state.direction].angle) state.direction^=3;
  if (!projectEnemyActor(center)) remove(center);
  return events;
}
}
