#pragma once

#include <cstdint>

namespace gyrus::game {

struct ProjectedPoint {
  std::uint8_t x;
  std::uint8_t y;
  bool clipped;
};

// Original 160x96 playfield coordinates. Angles wrap modulo 128; depth 0..63
// moves from outside the playfield toward its center. Odd depths share the
// preceding even depth's sample. Throws std::out_of_range for depth >= 64.
ProjectedPoint project(std::uint8_t angle, std::uint8_t depth);

// Original shortest-path steering, modulo 128. The exact opposite direction
// resolves to +2. This is one movement update, not a per-second velocity.
int steeringDelta(std::uint8_t angle, std::uint8_t target);

} // namespace gyrus::game
