#include "Turns.hpp"
#include <stdexcept>

namespace gyrus::game {
TurnEvent advanceDeath(Turns& state) {
  if (state.activePlayer > 1) throw std::out_of_range("Invalid active player");
  if (!state.deathCountdown) return TurnEvent::None;
  if (--state.deathCountdown) return TurnEvent::None;
  if (state.incomingCount) {
    state.deathCountdown = state.incomingCount;
    return TurnEvent::None;
  }
  auto& player = state.players[state.activePlayer];
  state.specialCount = 3;
  if (player.special) {
    if (player.protection) {
      player.protection = 0;
      player.special = 0;
    } else {
      player.special = 0x80;
      state.specialCount = 0;
    }
  }
  player.savedWave = state.waveIndex;
  player.savedFormation = state.formationCount;
  if (player.protection) --player.protection;
  --player.lives;
  const auto other = state.activePlayer ^ 1;
  if (state.players[other].lives) state.activePlayer = other;
  else if (!player.lives) {
    state.playing = false;
    return TurnEvent::GameOver;
  }
  state.players[state.activePlayer].dualShot = false;
  state.deathCountdown = 255;
  return TurnEvent::ResumePlayer;
}
void advanceStage(Turns& state) {
  if (state.activePlayer > 1) throw std::out_of_range("Invalid active player");
  auto& player = state.players[state.activePlayer];
  const unsigned tier = player.level >> 2;
  player.protection = tier < 5 ? 5 - tier : 2;
  player.savedWave = 0;
  player.special = 0;
  ++player.level;
  --player.warps;
  if (player.warps & 128) {
    player.warps = 3;
    player.planetOffset += 8;
    if (player.planetOffset == 48) player.planetOffset = 0;
  }
}
} // namespace gyrus::game
