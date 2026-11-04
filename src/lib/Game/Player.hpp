#pragma once

#include "Polar.hpp"

#include <array>
#include <cstdint>

namespace gyrus::game {

struct Shot {
  std::uint8_t angle = 0;
  std::uint8_t depth = 0;
  bool active = false;
  bool hit = false;
  ProjectedPoint position{0, 0, false};
  std::uint8_t width = 0;
};

struct PlayerInput {
  std::uint8_t joystick = 15;
  bool triggerLatched = false;
  std::uint8_t pendingFire = 0;
};

struct PlayerState {
  std::uint8_t angle = 0;
  std::uint8_t depth = 12;
  std::uint8_t aimAngle = 0;
  PlayerInput input;
  // Index 0 corresponds to original slot 1. Primary shots occupy indices 3..5.
  std::array<Shot, 6> shots{};
};

struct PlayerGates {
  std::uint8_t deathCountdown = 0; // $AC
  std::uint8_t transition = 0; // $B8, updated by updatePlayerMovement
  std::uint8_t messageCountdown = 0; // $9A
  std::uint8_t explosion = 0; // $D8
  bool dualShot = false; // $94 + active player
  bool suppressFireSound = false; // nonzero $D9
};

struct ShotEvents {
  bool fireSound = false;
  // Bit 0 corresponds to original slot 1; only successfully activated slots.
  std::uint8_t spawnedMask = 0;
};

// Joystick nibble is active-low Atari PORTA: bit0 up, bit1 down, bit2 left,
// bit3 right. Adapter/UI code supplies the selected player's four bits.
void sampleInput(PlayerState& player, std::uint8_t joystick, bool triggerPressed);
void updatePlayerMovement(PlayerState& player, std::uint8_t tick, PlayerGates& gates);
ShotEvents updatePlayerShots(PlayerState& player, const PlayerGates& gates);

} // namespace gyrus::game
