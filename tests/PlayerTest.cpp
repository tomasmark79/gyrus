#include <Game/Player.hpp>

#include <iostream>
#include <stdexcept>

namespace {
void require(bool value, const char* message) {
  if (!value) throw std::runtime_error(message);
}
}

int main() {
  using namespace gyrus::game;
  PlayerState player;
  PlayerGates gates;
  sampleInput(player, 7, true);
  require(player.input.pendingFire == 1, "new trigger press must latch fire");
  sampleInput(player, 7, true);
  require(player.input.pendingFire == 1, "held trigger must not repeat");
  updatePlayerMovement(player, 1, gates);
  require(player.angle == 0, "odd update must not rotate player");
  updatePlayerMovement(player, 2, gates);
  require(player.angle == 126 && player.aimAngle == 96, "right input must rotate toward right");
  auto events = updatePlayerShots(player, gates);
  require(events.fireSound && events.spawnedMask == 32 && player.shots[5].active,
          "first primary shot must occupy original slot 6");
  require(player.shots[5].angle == 126 && player.shots[5].depth == 12 &&
              player.shots[5].width == 3, "shot initial geometry changed");
  require(player.input.pendingFire == 0, "fire request must be consumed");
  events = updatePlayerShots(player, gates);
  require(events.spawnedMask == 0 && player.shots[5].depth == 14,
          "held trigger must not spawn another shot");
  gates.dualShot = true;
  sampleInput(player, 15, false);
  sampleInput(player, 15, true);
  events = updatePlayerShots(player, gates);
  require(events.spawnedMask == 18, "dual shot must use original slots 5 and 2");
  require(player.shots[4].angle == 124 && player.shots[1].angle == 0,
          "dual shot angles must straddle player and wrap");
  player.shots[4].hit = true;
  player.shots[5].depth = 54;
  updatePlayerShots(player, gates);
  require(!player.shots[4].active && !player.shots[5].active,
          "hit and center-reaching shots must deactivate");
  sampleInput(player, 15, false);
  sampleInput(player, 15, true);
  gates.messageCountdown = 1;
  events = updatePlayerShots(player, gates);
  require(!events.fireSound && !events.spawnedMask && !player.input.pendingFire,
          "message gate must consume suppressed fire");
  gates.messageCountdown = 0;
  gates.transition = 14;
  updatePlayerMovement(player, 8, gates);
  require(player.depth == 14 && gates.transition == 16, "transition depth uses old counter");
  gates.transition = 60;
  updatePlayerMovement(player, 16, gates);
  require(player.depth == 60 && gates.transition == 64, "late transition must accelerate");
  gates.explosion = 1;
  updatePlayerMovement(player, 24, gates);
  require(gates.transition == 64, "explosion must freeze movement transition");
  std::cout << "Player regression checks passed\n";
}
