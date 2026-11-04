#include <Game/Formation.hpp>

#include <iostream>
#include <stdexcept>

namespace {
void require(bool value, const char* message) {
  if (!value) throw std::runtime_error(message);
}
}

int main() {
  using namespace gyrus::game;
  auto control = formationControl(12);
  require(control.randomTurnMask == 7 && control.launchMask == 15 && control.maxIncoming == 4,
          "original formation difficulty must be selected by level");
  EnemyPaths paths;
  EnemyState enemies;
  control = formationControl(1);
  for (auto& actor : enemies.formation) projectEnemyActor(actor);
  enemies.formationCount = 36;
  EnemyInputs inputs;
  inputs.returnToFormation = true;
  inputs.randomByte = [] { return std::uint8_t(0); };
  require(attemptFormationLaunch(paths, enemies, control, inputs) == 2,
          "original formation launcher should launch a pair");
  require(paths.incomingCount == 2 && enemies.formationCount == 34,
          "launch must transfer counts");
  require(enemies.incoming[0].formationSlot == 1 && enemies.incoming[1].formationSlot == 2,
          "launch must choose first live formation and first empty incoming slots");
  require(enemies.incoming[0].depth == 36 && paths.phases[0] == 255 && control.attackState[0] != 0,
          "launch must prepare original outward attack program");
  require(attemptFormationLaunch(paths, enemies, control, inputs) == 0,
          "incoming cap must prevent another pair");
  auto& actor = enemies.incoming[0];
  actor.depth = 16;
  actor.spriteType = 10;
  control.attackState[0] = 1;
  updateFormationProgram(paths, enemies, control, 0, inputs);
  require(control.attackState[0] == 0, "straight dive must release one-shot steering at depth16");
  actor.depth = 18;
  actor.angle = 126;
  actor.spriteType = 8;
  control.attackState[0] = 2;
  updateFormationProgram(paths, enemies, control, 0, inputs);
  require(paths.tracks[0][1].angle == 0 && paths.tracks[0][1].depth == 20,
          "dynamic path must wrap angle and move inward by2");
  enemies.formation[2].hit = true;
  auto events = updateFormationGrid(enemies, control);
  require(events.scoreUnits == 1 && enemies.formationCount == 33 &&
              !enemies.formation[2].visible, "formation hit must award50 points and remove target");
  require(enemies.formation[2].angle == 98 && control.frameCounter == 1 && control.gridPhase == 1,
          "first formation update must rotate even inactive/hit slots");
  for (unsigned i = 0; i < 15; ++i) updateFormationGrid(enemies, control);
  require(enemies.formation[3].depth == 52, "formation must not oscillate between scheduled updates");
  updateFormationGrid(enemies, control);
  require(enemies.formation[3].depth == 54 && control.gridPhase == 2,
          "next scheduled formation update must toggle depth bit1");
  std::cout << "Formation regression checks passed\n";
}
