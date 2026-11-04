#include "Score.hpp"
#include <stdexcept>
namespace gyrus::game {
void ScoreBoard::add(unsigned player, std::uint8_t units) {
  if (player >= players.size()) throw std::out_of_range("Invalid score player");
  // $982F: six decimal columns per player, followed by an implicit display zero.
  // Preserve the original one-column-at-a-time carry, including byte-sized
  // increments and its unusual overflow from player 2 into player 1's columns.
  auto digit = [&](unsigned column) -> std::uint8_t& {
    return players[column / 6].digits[column % 6];
  };
  unsigned cursor = player * 6 + 5;
  unsigned carry = 0;
  if (units & 1) {
    unsigned tens = digit(cursor) + 5;
    carry = tens >= 10;
    digit(cursor) = static_cast<std::uint8_t>(tens - (carry ? 10 : 0));
  }
  unsigned amount = units >> 1;
  while (cursor != 0) {
    auto& column = digit(cursor - 1);
    const auto total = static_cast<std::uint8_t>(amount + column + carry);
    if (total == 6) {
      if (cursor == 3) ++players[0].lives;
      if (cursor == 9) ++players[1].lives;
    }
    carry = total >= 10;
    column = static_cast<std::uint8_t>(total - (carry ? 10 : 0));
    if (!carry) break;
    amount = 0;
    --cursor;
  }
}

std::uint32_t ScoreBoard::points(unsigned player) const {
  if (player >= players.size()) throw std::out_of_range("Invalid score player");
  std::uint32_t value = 0;
  for (auto digit : players[player].digits) value = value * 10 + digit;
  return value * 10;
}
}
