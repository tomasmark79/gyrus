#pragma once
#include "Enemies.hpp"
namespace gyrus::game {
struct Hazards {
  std::array<EnemyActor, 3> actors{}; // Original objects 36..38; center first.
  std::uint8_t direction = 0, cooldown = 0, count = 0; // $B5/$B6/$B7
};
struct HazardSpawnInputs {
  std::uint8_t level = 1, messageCountdown = 0, transition = 0;
  bool normalStage = true;
  std::function<std::uint8_t()> randomByte;
};
// $8353..$83AF. Counts down B6 even when another internal spawn gate blocks.
bool spawnHazards(Hazards&, const HazardSpawnInputs&);
// $8876..$8910. Caller applies active-game, pause and message gates, but no
// death gate. Outer objects update every16 VBI ticks; center moves every tick.
EnemyEvents updateHazards(Hazards&, std::uint8_t tick);
}
