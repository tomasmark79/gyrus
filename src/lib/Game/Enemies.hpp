#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

#include "Polar.hpp"

namespace gyrus::game {

struct EnemyPathPoint {
  std::uint8_t depth = 0;
  std::uint8_t angle = 0;
  std::uint8_t style = 0; // high nibble heading, low nibble sprite; 0xFF end
};

struct WaveSettings {
  std::uint8_t stageOffset = 0; // original $9D: 0,8,16,24,32,40
  std::uint8_t waveIndex = 0; // original $A0: 0..4
  bool normalStage = true; // original $D9 != 0
  std::uint8_t randomWave = 0; // accepted RANDOM&31 value 0..23 when waveIndex=4
  bool writableDescriptors = true; // ATR loads the cartridge image into RAM
};

struct EnemyPaths {
  EnemyPaths();
  std::array<std::uint8_t, 192> descriptors{};
  // Even enemy indices use track 0; odd indices use track 1.
  std::array<std::array<EnemyPathPoint, 128>, 2> tracks{};
  std::array<std::uint8_t, 10> cursors{}; // original $1A..$23, negative = delay
  std::array<std::uint8_t, 10> phases{}; // original $24..$2D, 0 = unused
  std::uint8_t incomingCount = 0;
  std::uint8_t nextWave = 0;
  std::uint8_t programLength = 0; // index of terminal 0xFF style
  std::uint8_t appearanceDelay = 0;
  std::uint8_t cadence = 0;
  std::uint8_t evenTimer = 0;
  std::uint8_t oddTimer = 0;
  std::uint8_t collisionMask = 0;
  std::uint8_t forcedSprite = 0;
  std::uint8_t scoreChain = 0;
  std::uint8_t preparedCount = 0; // original $9F, incremented each preparation
};

// Exact decoder of $9634..$9807. No emulated instruction execution.
// Throws std::out_of_range for settings outside the original observed domain.
void prepareWave(EnemyPaths& paths, const WaveSettings& settings);

struct EnemyActor {
  std::uint8_t angle = 0, depth = 0, heading = 0, spriteType = 0;
  std::uint8_t width = 0, formationSlot = 0;
  bool visible = false, hit = false;
  bool formationSlotReserved = false;
  ProjectedPoint position{0, 0, false};
  // Snapshot at the last successful draw; programs may advance logical state afterwards.
  std::uint8_t renderedDepth = 0, renderedSpriteType = 0, renderedHeading = 0;
};

struct EnemyState {
  EnemyState();
  std::array<EnemyActor, 10> incoming{}; // original object slots 39..48
  std::array<EnemyActor, 36> formation{}; // original object slots 52..87
  std::uint8_t formationCount = 0, nextFormationSlot = 0;
  std::uint8_t remainingBonusEnemies = 40; // original $DA
};

struct EnemyInputs {
  std::uint8_t playerAngle = 0, deathCountdown = 0, levelNumber = 1;
  bool normalStage = true;
  bool returnToFormation = false; // original $E6 != 0
  std::function<std::uint8_t()> randomByte;
  std::function<void(unsigned)> afterIncomingDraw;
  // Opt-in port repair: original RAM aliases other objects after formation slot36.
  // The strict/default profile retains the original sequential allocation.
  bool reuseVacantFormationSlots = false;
};

struct EnemyBonus {
  std::uint8_t angle, depth, label;
  bool incomingCarry = false; // Carry on entry to $8C91, before ADC #4.
};

struct EnemyEvents {
  unsigned scoreUnits = 0; // units of 50 displayed points, as original $982F
  std::vector<std::uint8_t> sounds;
  std::vector<EnemyBonus> bonuses;
  std::vector<std::uint8_t> scoreAwards; // Ordered original $982F calls, including zero.
  void award(std::uint8_t units) {
    scoreUnits += units;
    scoreAwards.push_back(units);
  }
};

// One original $8F96 (even) or $8F8E (odd) group update. Handles initial-wave
// paths, their dive markers, retreat, hits, and formation arrivals. Formation
// re-launch and its dynamic $DC steering programs are a separate subsystem.
EnemyEvents updateIncomingLane(EnemyPaths& paths, EnemyState& enemies,
                               unsigned parity, const EnemyInputs& inputs);

// Original $85FF..$8617 countdown scheduling for the two groups. Call once per
// unpaused gameplay update, after other gameplay gates have been applied.
EnemyEvents updateEnemyEntries(EnemyPaths& paths, EnemyState& enemies,
                               const EnemyInputs& inputs);

// Original projection, descriptor size/orientation and clipping, without drawing.
bool projectEnemyActor(EnemyActor& actor);

} // namespace gyrus::game
