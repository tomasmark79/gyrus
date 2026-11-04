#include <Game/Polar.hpp>

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace {
void require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
}

int main() {
  using namespace gyrus::game;
  // Recorded execution of the original $A0CB routine, depth 12.
  constexpr std::array<std::array<unsigned, 3>, 8> orbit{{
      {0, 80, 88}, {16, 43, 78}, {32, 25, 48}, {48, 43, 18},
      {64, 80, 8}, {80, 117, 18}, {96, 135, 48}, {112, 117, 78}}};
  for (const auto& sample : orbit) {
    const auto point = project(static_cast<std::uint8_t>(sample[0]), 12);
    require(point.x == sample[1] && point.y == sample[2] && !point.clipped,
            "original outer orbit changed");
  }
  const auto center = project(0, 58);
  require(center.x == 80 && center.y == 50 && !center.clipped,
          "depth must move inward through the original perspective table");
  const auto outside = project(0, 0);
  require(outside.x == 255 && outside.y == 255 && outside.clipped,
          "out-of-field sentinel lost");
  for (unsigned angle = 0; angle < 128; ++angle) {
    for (unsigned target = 0; target < 128; ++target) {
      const int expected = angle == target ? 0 :
          (((angle + 128 - target) & 127) < 64 ? -2 : 2);
      require(steeringDelta(angle, target) == expected, "steering differs");
    }
    for (unsigned depth = 0; depth < 64; ++depth) {
      const auto point = project(angle, depth);
      const auto wrapped = project(angle + 128, depth);
      require(point.x == wrapped.x && point.y == wrapped.y &&
                  point.clipped == wrapped.clipped, "angle normalization differs");
      const auto paired = project(angle, depth ^ 1);
      require(point.x == paired.x && point.y == paired.y &&
                  point.clipped == paired.clipped, "depth low bit must be ignored");
    }
  }
  bool rejected = false;
  try { (void)project(0, 64); } catch (const std::out_of_range&) { rejected = true; }
  require(rejected, "undefined original depth must be rejected");
  std::cout << "Polar regression checks passed\n";
}
