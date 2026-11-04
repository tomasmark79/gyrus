#pragma once
#include <array>
#include <cstdint>

namespace gyrus::game {
struct TurnPlayer {
  std::uint8_t lives = 5;
  std::uint8_t protection = 5; // $F7 + player
  std::uint8_t special = 0; // $AD + player
  std::uint8_t savedWave = 0, savedFormation = 0;
  bool dualShot = false;
  std::uint8_t level = 1, warps = 2, planetOffset = 0;
};
struct Turns {
  std::array<TurnPlayer, 2> players{};
  std::uint8_t activePlayer = 0;
  std::uint8_t deathCountdown = 0;
  std::uint8_t incomingCount = 0, formationCount = 0, waveIndex = 0;
  std::uint8_t specialCount = 0; // $B1
  bool playing = true;
};
enum class TurnEvent { None, ResumePlayer, GameOver };
// $8561..$85C2. ResumePlayer leaves countdown=$FF as the input to stage init.
// The caller then restores the selected player's stage and clears countdown.
TurnEvent advanceDeath(Turns& state);
// $9425..$945E, stopping before the new stage's initialization at $92E6.
void advanceStage(Turns& state);
}
