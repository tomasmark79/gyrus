#include "EnemyShots.hpp"
#include "EnemyData.hpp"
#include <stdexcept>

namespace gyrus::game {
namespace {
bool aligned(std::uint8_t angle, std::uint8_t aim) {
    // $824B..$8260 deliberately preserves carry from each comparison.
    if (angle == aim) return true;
    const auto below = static_cast<std::uint8_t>(angle - 1 - (angle < aim ? 1 : 0)) & 126;
    if (below == aim) return true;
    return ((below + 4 + (below >= aim ? 1 : 0)) & 126) == aim;
}

void spawn(EnemyShot& shot, std::uint8_t angle, std::uint8_t depth, std::uint8_t sprite) {
    shot.angle = angle;
    shot.depth = depth;
    shot.spriteType = sprite;
    shot.width = 0;
    shot.active = true;
    shot.pendingDraw = true;
}

bool place(EnemyShot& shot) {
    if (shot.depth >= 64) return false;
    if (shot.spriteType >= 58 || (shot.spriteType & 1))
        throw std::out_of_range("Enemy shot descriptor outside original sprite table");
    const auto point = project(shot.angle, shot.depth);
    if (point.clipped || point.x >= 160 || point.y >= 96) return false;
    shot.position = point;
    const auto& profile = enemy_data::spriteProfiles[shot.spriteType / 2];
    const auto packed = profile[1 + (shot.depth >> 4)];
    shot.width = ((shot.depth & 8) ? packed : packed >> 4) & 15;
    if (profile[0] < 128) shot.heading = profile[0] ? shot.angle >> (profile[0] + 2) : 0;
    const unsigned left = point.x > shot.width / 2 ? point.x - shot.width / 2 : 0;
    const unsigned top = point.y > shot.width / 2 ? point.y - shot.width / 2 : 0;
    return shot.width && left + shot.width < 160 && top + shot.width < 96;
}
} // namespace

EnemyShotSpawnResult spawnEnemyShot(EnemyShots& shots,
    std::span<const EnemyShooter,49> sources, const EnemyShotSpawnInputs& inputs) {
    unsigned shooter = 0;
    bool forced = false;
    if (shots.activeCount < inputs.cap) {
        if (shots.forcedShooter) {
            if (shots.forcedShooter > 3)
                throw std::out_of_range("Forced enemy shooter must select original slots 49..51");
            shooter = 9 + shots.forcedShooter;
            shots.forcedShooter = 0;
            forced = true;
        } else if (shots.cooldown || !inputs.normalStage) {
            return {};
        }
    } else if (shots.activeCount == inputs.cap) {
        return {};
    }
    const auto random = [&inputs] {
        if (!inputs.randomByte) throw std::invalid_argument("Enemy shot spawn requires a random-byte source");
        return inputs.randomByte();
    };
    if (!forced) {
        for (; shooter < sources.size(); ++shooter) {
            const auto& source = sources[shooter];
            if (source.active && !source.hit && source.depth >= 0x1a && aligned(source.angle, inputs.aimAngle)) break;
        }
        if (shooter == sources.size() || (random() & 3)) return {};
    }
    unsigned slot = 0;
    while (slot < shots.shots.size() && shots.shots[slot].active) ++slot;
    if (slot == shots.shots.size()) return {EnemyShotSpawnStatus::outsideShotPool, 255};
    int offset = (random() & 6) - 2;
    if (offset == 4) offset = 0;
    const auto& source = sources[shooter];
    const auto angle = static_cast<std::uint8_t>((source.angle + offset) & 126);
    spawn(shots.shots[slot], angle, source.depth, 0x26);
    ++shots.activeCount;
    shots.cooldown = inputs.cooldownReload;
    return {EnemyShotSpawnStatus::spawned, static_cast<std::uint8_t>(slot)};
}

bool spawnSpecialEnemyShot(EnemyShots& shots, std::uint8_t reload,
    std::uint8_t playerAngle, std::uint8_t transition, bool normalStage) {
    if (shots.specialCountdown == 0) {
        if (shots.shots[4].active) return false;
        shots.specialCountdown = reload;
        if (reload == 0) return false;
    }
    if (--shots.specialCountdown != 0 || transition || !normalStage) return false;
    spawn(shots.shots[4], playerAngle ^ 2, 0x32, 0x18);
    ++shots.activeCount;
    return true;
}

std::uint8_t updateEnemyShots(EnemyShots& shots, std::uint8_t tick) {
    if (tick & 3) return 0;
    std::uint8_t removed = 0;
    for (unsigned slot = 0; slot < shots.shots.size(); ++slot) {
        auto& shot = shots.shots[slot];
        if (!shot.active) continue;
        const bool initial = shot.pendingDraw;
        shot.pendingDraw = false;
        if (!initial) {
            if ((tick & 4) && slot != 4) shot.spriteType ^= 2;
            shot.depth = static_cast<std::uint8_t>(shot.depth - 2);
        }
        if ((!initial && (shot.depth & 128)) || !place(shot)) {
            --shots.activeCount;
            shot.active = false;
            shot.width = 0;
            shot.pendingDraw = false;
            removed |= static_cast<std::uint8_t>(1U << slot);
        }
    }
    return removed;
}

PlayerHitEvents resolvePlayerHit(PlayerGates& gates, const PlayerHitInputs& inputs) {
    if (gates.deathCountdown || gates.transition || !inputs.normalStage) return {};
    const auto collision = inputs.playfieldCollisionBits[0] | inputs.playfieldCollisionBits[1]
        | inputs.playfieldCollisionBits[2];
    if ((collision & inputs.collisionMask) == 0) return {true, false};
    gates.deathCountdown = 0x5a;
    gates.explosion = 0x88; // $A076, after the original five-piece explosion setup.
    return {true, true};
}
} // namespace gyrus::game
