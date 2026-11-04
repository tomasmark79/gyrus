#pragma once
#include <array>
#include <cstdint>

namespace gyrus::game {
struct PlayerScore {
  std::array<std::uint8_t, 6> digits{};
  std::uint8_t lives = 5;
};
struct ScoreBoard {
  std::array<PlayerScore, 2> players{};
  void add(unsigned player, std::uint8_t units);
  [[nodiscard]] std::uint32_t points(unsigned player) const;
};
}
