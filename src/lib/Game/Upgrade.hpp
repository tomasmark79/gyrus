#pragma once

#include "Enemies.hpp"
#include <array>
#include <cstdint>

namespace gyrus::game {

struct UpgradeParticle {
    EnemyActor actor;
    bool drawn{}; // Raster visibility differs from the original retained width flag.
};

struct UpgradeState {
    UpgradeState();
    std::array<UpgradeParticle, 6> particles{}; // Original slots 23..28.
    std::array<EnemyActor, 3> targets{};        // Original slots 49..51.
    std::uint8_t phase{};          // $DB: initial/countdown or collection flags.
    std::uint8_t targetPhase{};    // Active player's $AD/$AE.
    std::uint8_t targetCount{3};   // $B1, initialized by the stage scheduler.
    std::uint8_t forcedShooter{};  // $B3, consumed by EnemyShots.
    std::uint8_t scoreChain{};     // $B0.
    std::uint8_t animation{};      // $BB.
    std::uint8_t spawnDelay{};     // $AF.
    std::uint8_t dualShotLevel{};  // Active player's $94/$95 (byte, not a bool).
};

struct UpgradeInputs {
    std::uint8_t playerAngle{}, playerDepth{12}, tick{};
    std::uint8_t formationCount{}, deathCountdown{}, levelRank{};
    bool returnToFormation{};
};

enum class UpgradeSpawnResult { none, abandoned, spawned };

// Original $82A7..$831D; the main-loop caller applies active-game/pause/death gates.
UpgradeSpawnResult spawnUpgrade(UpgradeState& state, const UpgradeInputs& inputs);
// Original $89F8..$8B6A, once per VBI; internal cadence is eight VBI ticks.
EnemyEvents updateUpgradeTargets(UpgradeState& state, const UpgradeInputs& inputs);
// Original $870C..$8789. Returns true if the active player's dual-shot byte increased.
// Caller applies the original message/active-game/pause gates.
bool updateUpgradeParticles(UpgradeState& state, const UpgradeInputs& inputs);

} // namespace gyrus::game
