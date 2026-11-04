#include "Effects.hpp"
#include "Polar.hpp"
#include <algorithm>

namespace gyrus::game {
bool HitExplosions::spawn(std::uint8_t angle, std::uint8_t depth,
                          unsigned enemyKind) noexcept {
    if (depth >= 64) return false;
    constexpr std::array<std::uint8_t,11> radial{2,0,254,0,0,0,254,0,0,0,254};
    constexpr std::array<std::uint8_t,11> angular{0,2,0,254,0,0,0,0,0,0,0};
    // $81E1 LSR clears carry for the even descriptor, so SBC2 subtracts3.
    const unsigned index = enemyKind >= 3 && enemyKind < 14 ? enemyKind - 3 : 4;
    for (auto& slot : slots_) {
        if (slot.status) continue;
        slot.angle = angle;
        slot.depth = depth;
        slot.kind = 14;
        slot.depthStep = radial[index];
        slot.angleStep = angular[index];
        slot.status = 128;
        return true;
    }
    return false;
}

void HitExplosions::advance(std::uint8_t frame) {
    if ((frame & 3) != 2) return;
    for (auto& slot : slots_) {
        if (!slot.status) continue;
        if (slot.status & 128) {
            slot.status = 10;
            slot.timer = 10;
        } else {
            if ((slot.timer & 3) == 1) {
                const auto depth = static_cast<std::uint8_t>(slot.depth + slot.depthStep);
                if (depth >= 20) slot.depth = depth;
                slot.angle = (slot.angle + slot.angleStep) & 126;
            }
            slot.kind = slot.kind == 16 ? 14 : slot.kind + 1;
            if (!--slot.timer) {
                slot.status = 0;
                continue;
            }
        }
        if (slot.depth >= 64) {
            slot.status = 0;
            continue;
        }
        const auto point = project(slot.angle, slot.depth);
        if (point.clipped) {
            slot.status = 0;
            continue;
        }
        slot.x = point.x;
        slot.y = point.y;
        const unsigned size = spriteImage(slot.kind, slot.depth, 0).height;
        slot.status = size;
        const unsigned left = std::max(0, static_cast<int>(slot.x) - static_cast<int>(size/2));
        const unsigned top = std::max(0, static_cast<int>(slot.y) - static_cast<int>(size/2));
        if (left + size >= 160 || top + size >= 96) slot.status = 0;
    }
}

void HitExplosions::draw(Framebuffer& frame) const {
    for (const auto& slot : slots_) {
        if (slot.status && !(slot.status & 128))
            xorSprite(frame, slot.kind, slot.depth, 0, slot.x, slot.y);
    }
}

bool ScorePopups::spawn(std::uint8_t angle, std::uint8_t depth,
                        std::uint8_t label, bool incomingCarry) noexcept {
    if (label > 7 || depth >= 64) return false;
    for (unsigned i = 0; i < slots_.size(); ++i) {
        auto& slot = slots_[i];
        if (slot.status) continue;
        // CPX in the allocator clears carry while seeking slots18/19.
        slot.angle = (angle + 4 + (i == 0 && incomingCarry)) & 126;
        slot.depth = std::max<std::uint8_t>(depth, 18);
        slot.label = label;
        slot.status = 0x9e;
        slot.visible = false;
        return true;
    }
    return false;
}

void ScorePopups::advance() {
    for (auto& slot : slots_) {
        if (!slot.status) continue;
        if (!(slot.status & 128)) {
            if (--slot.timer) continue;
            slot.status = 0;
            slot.visible = false;
            return; // $8C65 tail-call skips later slots on this update.
        }
        if (--slot.status & 128) continue;
        slot.timer = 120;
        const auto point = project(slot.angle, slot.depth);
        if (!point.clipped) {
            slot.x = point.x;
            slot.y = point.y;
            slot.status = 6;
        }
        slot.visible = true;
        return;
    }
}

void ScorePopups::draw(Framebuffer& frame) const {
    for (const auto& slot : slots_) {
        if (!slot.visible) continue;
        if (slot.status == 6)
            xorSprite(frame, 21 + slot.label, 0, 0, slot.x, slot.y);
        xorSprite(frame, 20, 0, 0, static_cast<std::uint8_t>(slot.x + 8), slot.y);
    }
}

namespace {
// ATR code-data at $A095..$A0BC, two four-facing banks of five vectors.
constexpr std::array<std::uint8_t, 20> velocityX{
    252,253,0,3,4, 253,0,3,4,3, 0,3,4,3,0, 3,4,3,0,253};
constexpr std::array<std::uint8_t, 20> velocityY{
    0,253,252,253,0, 253,252,253,0,3, 252,253,0,3,4, 253,0,3,4,3};
}

void PlayerExplosion::start(std::uint8_t angle, std::uint8_t x,
                            std::uint8_t y) noexcept {
    const unsigned facing = (((angle >> 3) + 1) >> 1) & 7;
    const bool negate = facing >= 4;
    for (unsigned slot = 0; slot < 5; ++slot) {
        // Initialization visits object slots33 down to29.
        const unsigned index = (facing & 3) * 5 + 4 - slot;
        const auto dx = velocityX[index], dy = velocityY[index];
        particles_[slot] = {x, y,
            static_cast<std::uint8_t>(negate ? -dx : dx),
            static_cast<std::uint8_t>(negate ? -dy : dy)};
    }
    countdown_ = 0x88;
    visible_ = false;
}

void PlayerExplosion::advance(std::uint8_t frame) noexcept {
    if (!active() || (frame & 7)) return;
    countdown_ = (countdown_ - 1) & 127;
    visible_ = countdown_ != 0;
    if (!visible_) return;
    for (auto& particle : particles_) {
        particle.x += particle.dx;
        particle.y += particle.dy;
    }
}

void PlayerExplosion::draw(Framebuffer& frame) const {
    if (!visible_) return;
    for (const auto& particle : particles_) {
        // $8D0C rejects unsigned wrapped coordinates before the XOR blitter.
        if (particle.x < 157 && particle.y < 93)
            xorSprite(frame, 0, 0, 0, particle.x, particle.y);
    }
}
}
