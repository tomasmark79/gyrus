#include "Formation.hpp"

#include <algorithm>
#include <stdexcept>

namespace gyrus::game {
namespace {
std::uint8_t random(const EnemyInputs& inputs) {
  if (!inputs.randomByte) throw std::invalid_argument("Formation attack requires a random byte source");
  return inputs.randomByte();
}

EnemyPathPoint& dynamicPoint(EnemyPaths& paths, unsigned index) {
  const auto cursor = static_cast<std::uint8_t>((index >> 1) + 1);
  paths.cursors[index] = cursor;
  return paths.tracks[index & 1][cursor];
}

void seekPlayer(EnemyPaths& paths, EnemyState& enemies, FormationControl& control,
                unsigned index, const EnemyInputs& inputs, EnemyPathPoint& point) {
  auto& actor = enemies.incoming[index];
  actor.depth -= 2;
  if (actor.depth < 17) {
    control.attackState[index] = 0;
    paths.phases[index] = 64;
    return;
  }
  auto delta = steeringDelta(actor.angle, inputs.playerAngle);
  if (!delta) delta = 2;
  const auto sprite = static_cast<std::uint8_t>(10 - delta);
  control.attackState[index] = (sprite >> 2) + 1;
  point.angle = actor.angle;
  point.style = static_cast<std::uint8_t>(((actor.angle << 1) & 240) | sprite);
  point.depth = actor.depth;
}

void chooseProgram(EnemyPaths& paths, EnemyState& enemies, FormationControl& control,
                   unsigned index, const EnemyInputs& inputs) {
  paths.phases[index] = 255;
  const auto choice = (random(inputs) & 15) ? 0 : (random(inputs) & 14);
  auto& point = dynamicPoint(paths, index);
  if (choice == 0) {
    seekPlayer(paths, enemies, control, index, inputs, point);
  } else if (choice == 12) {
    paths.phases[index] = enemies.incoming[index].depth;
    control.attackState[index] = 0;
  } else {
    control.attackState[index] = (choice == 4 || choice == 8) ? 0 : 1;
    paths.phases[index] = 64;
  }
}

EnemyInputs withPrograms(EnemyPaths& paths, EnemyState& enemies, FormationControl& control,
                         const EnemyInputs& inputs) {
  auto hooked = inputs;
  hooked.afterIncomingDraw = [&](unsigned index) {
    updateFormationProgram(paths, enemies, control, index, inputs);
    if (inputs.afterIncomingDraw) inputs.afterIncomingDraw(index);
  };
  return hooked;
}
} // namespace

FormationControl formationControl(std::uint8_t levelNumber) {
  FormationControl control;
  const auto quarter = levelNumber >> 2;
  const auto index = std::min(quarter, 3);
  constexpr std::array<std::uint8_t, 4> turnMasks{31, 15, 15, 7};
  constexpr std::array<std::uint8_t, 4> launchMasks{127, 63, 31, 15};
  control.randomTurnMask = turnMasks[index];
  control.launchMask = launchMasks[index];
  control.maxIncoming = std::min(quarter + 2, 4);
  return control;
}

EnemyEvents updateFormationGrid(EnemyState& enemies, FormationControl& control) {
  EnemyEvents events;
  const auto previous = control.frameCounter++;
  if (previous & 15) return events;
  ++control.gridPhase;
  for (auto& actor : enemies.formation) {
    if (control.gridPhase & 1) actor.angle = (actor.angle + 2) & 126;
    else actor.depth ^= 2;
  }
  for (auto& actor : enemies.formation) {
    if (!actor.visible) continue;
    if (actor.hit) {
      --enemies.formationCount;
      events.award(1);
      actor.visible = actor.hit = false;
      actor.width = 0;
    } else projectEnemyActor(actor);
  }
  return events;
}

unsigned attemptFormationLaunch(EnemyPaths& paths, EnemyState& enemies,
                                 FormationControl& control, const EnemyInputs& inputs) {
  if (!inputs.returnToFormation) return 0;
  if ((random(inputs) & control.launchMask) || inputs.deathCountdown) return 0;
  unsigned launched = 0;
  do {
    if (paths.incomingCount == control.maxIncoming) return launched;
    const auto source = std::find_if(enemies.formation.begin(), enemies.formation.end(),
                                    [](const auto& actor) { return actor.visible && !actor.hit; });
    if (source == enemies.formation.end()) return launched;
    projectEnemyActor(*source); // original erase path also recomputes geometry
    source->visible = source->hit = false;
    source->width = 0;
    --enemies.formationCount;
    ++paths.incomingCount;
    const auto destination = std::find_if(enemies.incoming.begin(), enemies.incoming.end(),
                                         [](const auto& actor) { return !actor.visible; });
    if (destination == enemies.incoming.end())
      throw std::out_of_range("Original launch has no free incoming actor slot");
    const auto index = static_cast<unsigned>(destination - enemies.incoming.begin());
    destination->formationSlot = static_cast<std::uint8_t>(source - enemies.formation.begin() + 1);
    destination->formationSlotReserved = true;
    destination->angle = source->angle;
    destination->depth = 38;
    auto& point = dynamicPoint(paths, index);
    seekPlayer(paths, enemies, control, index, inputs, point);
    paths.phases[index] = 255;
    destination->spriteType = point.style & 15;
    destination->heading = point.style >> 4;
    projectEnemyActor(*destination);
    ++launched;
  } while (paths.incomingCount & 1);
  return launched;
}

void updateFormationProgram(EnemyPaths& paths, EnemyState& enemies, FormationControl& control,
                            unsigned index, const EnemyInputs& inputs) {
  if (index >= enemies.incoming.size()) throw std::out_of_range("Invalid incoming actor index");
  auto& attack = control.attackState[index];
  if (!attack) return;
  auto& point = dynamicPoint(paths, index);
  auto& actor = enemies.incoming[index];
  if (attack > 2) {
    if (!(random(inputs) & control.randomTurnMask)) {
      chooseProgram(paths, enemies, control, index, inputs);
    } else {
      const auto sprite = point.style & 15;
      point.angle = static_cast<std::uint8_t>((actor.angle + 10 - sprite) & 127);
      point.style = static_cast<std::uint8_t>(((point.angle << 1) & 240) | sprite);
    }
    return;
  }
  if (attack == 1) {
    if (actor.depth != 16) return;
    ++attack;
    if (actor.spriteType == 10) {
      attack = 0;
      return;
    }
  }
  paths.phases[index] = 255;
  if (actor.depth == 30) {
    chooseProgram(paths, enemies, control, index, inputs);
    return;
  }
  const int delta = 10 - actor.spriteType;
  point.depth = actor.depth + 2;
  point.angle = static_cast<std::uint8_t>((actor.angle + delta) & 127);
  const auto heading = ((point.angle >> 3) - delta) & 15;
  point.style = static_cast<std::uint8_t>((heading << 4) | actor.spriteType);
}

EnemyEvents updateEnemyEntriesWithFormation(EnemyPaths& paths, EnemyState& enemies,
                                            FormationControl& control, const EnemyInputs& inputs) {
  return updateEnemyEntries(paths, enemies, withPrograms(paths, enemies, control, inputs));
}

EnemyEvents updateIncomingLaneWithFormation(EnemyPaths& paths, EnemyState& enemies,
                                            FormationControl& control, unsigned parity,
                                            const EnemyInputs& inputs) {
  return updateIncomingLane(paths, enemies, parity, withPrograms(paths, enemies, control, inputs));
}
} // namespace gyrus::game
