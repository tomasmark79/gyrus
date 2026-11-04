#pragma once
#include "Player.hpp"
#include <cstdint>
#include <span>
#include <vector>

namespace gyrus::game {
struct CollisionTarget {
  std::uint8_t x{}, y{}, width{};
  bool active{}, hit{};
};
struct ShotHit {
  unsigned shot{}, target{};
};
bool shotOverlaps(std::uint8_t x, std::uint8_t y, const CollisionTarget& target);
// Targets must be supplied in original object order (slots37..87). A hit marks
// the target, the shot and any active paired shot; damage/score are later systems.
std::vector<ShotHit> resolveShotHits(PlayerState& player, std::span<CollisionTarget> targets);
}
