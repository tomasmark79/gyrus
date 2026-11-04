#include "Player.hpp"

#include <algorithm>
#include <array>

namespace gyrus::game {
namespace {
// Original $89E8..$89F7, including the original results for impossible opposing
// joystick combinations. Values >=128 mean no target.
constexpr std::array<std::uint8_t, 16> joystickTargets{
    254, 254, 253, 252, 1, 112, 80, 96, 255, 16, 48, 32, 255, 0, 64, 255};
// Nibbles from projectile descriptor $B5E5..$B5E8, one per eight depth units.
constexpr std::array<std::uint8_t, 8> shotWidths{3, 3, 3, 2, 1, 1, 1, 1};

void deactivate(Shot& shot) {
  shot.active = false;
  shot.hit = false;
  shot.width = 0;
}

void positionShot(Shot& shot) {
  // The original graphics routine also decides whether a shot survives clipping.
  // Retain that geometry without retaining the Atari bitmap drawing machinery.
  if (shot.depth >= 64) {
    deactivate(shot);
    return;
  }
  const auto position = project(shot.angle, shot.depth);
  if (position.clipped || position.x >= 160 || position.y >= 96) {
    deactivate(shot);
    return;
  }
  shot.position = position;
  shot.width = shotWidths[shot.depth >> 3];
  const auto left = std::max(0, int(position.x) - shot.width / 2);
  const auto top = std::max(0, int(position.y) - shot.width / 2);
  if (left + shot.width >= 160 || top + shot.width >= 96) {
    deactivate(shot);
    return;
  }
  shot.active = true;
}
} // namespace

void sampleInput(PlayerState& player, std::uint8_t joystick, bool triggerPressed) {
  player.input.joystick = joystick & 15;
  if (!triggerPressed) {
    player.input.triggerLatched = false;
  } else if (!player.input.triggerLatched) {
    ++player.input.pendingFire;
    player.input.triggerLatched = true;
  }
}

void updatePlayerMovement(PlayerState& player, std::uint8_t tick, PlayerGates& gates) {
  // $AC is a caller gate at $855F; $D8 is the routine's own gate at $8B6B.
  if (gates.deathCountdown || gates.explosion) return;
  if ((tick & 7) == 0 && gates.transition != 0) {
    const auto previous = gates.transition;
    if (previous < 128) {
      gates.transition = static_cast<std::uint8_t>(previous + 2);
      if (previous >= 60) gates.transition += 2;
    }
    player.depth = std::min<std::uint8_t>(previous, 60);
  }
  player.aimAngle = player.angle;
  if (tick & 1) return;
  const auto target = joystickTargets[player.input.joystick & 15];
  if (target >= 128) return;
  player.aimAngle = target;
  player.angle = static_cast<std::uint8_t>((player.angle + steeringDelta(player.angle, target)) & 127);
}

ShotEvents updatePlayerShots(PlayerState& player, const PlayerGates& gates) {
  for (auto& shot : player.shots) {
    if (!shot.active) continue;
    if (shot.hit) {
      deactivate(shot);
      continue;
    }
    const auto nextDepth = static_cast<std::uint8_t>(shot.depth + 2);
    if (nextDepth >= 56) {
      deactivate(shot);
      continue;
    }
    shot.depth = nextDepth;
    positionShot(shot);
  }

  ShotEvents events;
  if (!player.input.pendingFire) return events;
  player.input.pendingFire = 0;
  if (gates.deathCountdown || gates.transition || gates.messageCountdown) return events;
  events.fireSound = !gates.suppressFireSound;
  for (int index = 5; index >= 3; --index) {
    if (player.shots[index].active) continue;
    auto& primary = player.shots[index];
    primary.angle = static_cast<std::uint8_t>((player.angle - (gates.dualShot ? 2 : 0)) & 126);
    primary.depth = player.depth;
    primary.hit = false;
    positionShot(primary);
    if (primary.active) events.spawnedMask |= 1 << index;
    if (gates.dualShot) {
      auto& paired = player.shots[index - 3];
      paired.angle = (player.angle + 2) & 126;
      paired.depth = primary.depth;
      paired.hit = false;
      positionShot(paired);
      if (paired.active) events.spawnedMask |= 1 << (index - 3);
    }
    break;
  }
  return events;
}
} // namespace gyrus::game
