#pragma once

#include "Graphics.hpp"
#include <array>
#include <cstdint>
#include <functional>

namespace gyrus::game {

struct Star {
    bool active = false;
    std::uint8_t angle = 0;
    std::uint8_t depth = 0;
    std::uint8_t x = 0;
    std::uint8_t y = 0;
};

// Original twenty-slot $8D1E starfield, independent of the random generator.
class Starfield {
public:
    using RandomByte = std::function<std::uint8_t()>;
    void clear() noexcept { stars_ = {}; }
    // Normal motion runs on even ticks; travel runs every tick. stopNearCenter
    // corresponds to original wave-state2, removing stars below depth18.
    void advance(std::uint8_t frame, bool travel, bool stopNearCenter,
                 const RandomByte& random);
    void draw(Framebuffer& frame) const noexcept;
    const std::array<Star, 20>& stars() const noexcept { return stars_; }
private:
    std::array<Star, 20> stars_{};
};

} // namespace gyrus::game
