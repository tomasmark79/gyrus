#pragma once
#include "Graphics.hpp"

namespace gyrus::game {
struct HitExplosion {
    std::uint8_t status = 0, timer = 0, angle = 0, depth = 0;
    std::uint8_t kind = 0, depthStep = 0, angleStep = 0, x = 0, y = 0;
};

// Original slots12..16. Spawn with sprite kind (not doubled descriptor),
// advance once per game tick, then draw into a freshly composed framebuffer.
class HitExplosions {
public:
    void clear() noexcept { slots_ = {}; }
    bool spawn(std::uint8_t angle, std::uint8_t depth, unsigned enemyKind) noexcept;
    void advance(std::uint8_t frame);
    void draw(Framebuffer& frame) const;
    const std::array<HitExplosion, 5>& slots() const noexcept { return slots_; }
private:
    std::array<HitExplosion, 5> slots_{};
};

struct ScorePopup {
    std::uint8_t status = 0, timer = 0, angle = 0, depth = 0, label = 0;
    std::uint8_t x = 0, y = 0;
    bool visible = false;
};

// Original slots17..22. Labels0..7, depth0..63. Advance follows each call
// to $8C21; transition/expiry deliberately stops processing subsequent slots.
class ScorePopups {
public:
    void clear() noexcept { slots_ = {}; }
    bool spawn(std::uint8_t angle, std::uint8_t depth, std::uint8_t label,
               bool incomingCarry = false) noexcept;
    void advance();
    void draw(Framebuffer& frame) const;
    const std::array<ScorePopup, 3>& slots() const noexcept { return slots_; }
private:
    std::array<ScorePopup, 3> slots_{};
};

struct ExplosionParticle {
    std::uint8_t x = 0, y = 0, dx = 0, dy = 0;
};

// Original $A01D mode80 and $8CC4 player-death particles. Advance before
// drawing into a freshly composed playfield. Initial particles are invisible.
class PlayerExplosion {
public:
    void start(std::uint8_t angle, std::uint8_t x, std::uint8_t y) noexcept;
    void clear() noexcept { countdown_ = 0; visible_ = false; }
    void advance(std::uint8_t frame) noexcept;
    void draw(Framebuffer& frame) const;
    bool active() const noexcept { return countdown_ != 0; }
    std::uint8_t countdown() const noexcept { return countdown_; }
    const std::array<ExplosionParticle, 5>& particles() const noexcept { return particles_; }
private:
    std::array<ExplosionParticle, 5> particles_{};
    std::uint8_t countdown_ = 0;
    bool visible_ = false;
};
}
