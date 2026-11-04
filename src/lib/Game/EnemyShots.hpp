#pragma once

#include "Player.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>

namespace gyrus::game {

struct EnemyShot {
    std::uint8_t angle{}, depth{}, spriteType{0x26}, heading{}, width{};
    bool active{}, pendingDraw{};
    ProjectedPoint position{0, 0, false};
};

struct EnemyShots {
    std::array<EnemyShot, 5> shots{}; // Original object slots 7..11.
    std::uint8_t activeCount{};      // $A9, includes the independent special shot.
    std::uint8_t cooldown{};         // $A7, decremented separately at $8529.
    std::uint8_t forcedShooter{};    // $B3: 1..3 selects original source slots 49..51.
    std::uint8_t specialCountdown{}; // $B4.
};

struct EnemyShooter {
    std::uint8_t angle{}, depth{};
    bool active{}, hit{};
};

struct EnemyShotSpawnInputs {
    std::uint8_t cap{}, cooldownReload{}, aimAngle{}; // $A6, $A8, $B2.
    bool normalStage{true}; // $D9 != 0.
    std::function<std::uint8_t()> randomByte;
};

enum class EnemyShotSpawnStatus { none, spawned, outsideShotPool };
struct EnemyShotSpawnResult {
    EnemyShotSpawnStatus status{EnemyShotSpawnStatus::none};
    std::uint8_t index{255}; // 0..4 corresponds to original slots 7..11.
};

/// $821F..$82A6. Sources must be in original slot order 39..87, including 49..51.
/// An unbounded original allocation beyond slot 11 is reported, never guessed.
EnemyShotSpawnResult spawnEnemyShot(EnemyShots& shots,
    std::span<const EnemyShooter, 49> sources, const EnemyShotSpawnInputs& inputs);

/// $831F..$8352. The independent slot 11 countdown and exact XOR 2 aim.
bool spawnSpecialEnemyShot(EnemyShots& shots, std::uint8_t reload,
    std::uint8_t playerAngle, std::uint8_t transition, bool normalStage);

/// $8913, one gameplay tick $56 (not VBI counter $55). Returns removed-slot bits.
/// Caller applies original active-game/message/pause gates.
std::uint8_t updateEnemyShots(EnemyShots& shots, std::uint8_t tick);

struct PlayerHitInputs {
    std::array<std::uint8_t, 3> playfieldCollisionBits{}; // Original P0PF/P1PF/P2PF.
    std::uint8_t collisionMask{}; // $F9.
    bool normalStage{true};
};
struct PlayerHitEvents {
    bool collisionLatchCleared{};
    bool deathStarted{}; // Requests SFX 0, player explosion mode $80, and hides player.
};

/// $85C5..$85F9 death gate; requires independently established collision bits.
/// Does not invent a geometric player collision or implement the later lives/turn scheduler.
PlayerHitEvents resolvePlayerHit(PlayerGates& gates, const PlayerHitInputs& inputs);

} // namespace gyrus::game
