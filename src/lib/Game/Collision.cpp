#include "Collision.hpp"
namespace gyrus::game {
bool shotOverlaps(std::uint8_t x, std::uint8_t y, const CollisionTarget& target) {
  if (!target.active || target.hit || target.width == 0 || target.width >= 128) return false;
  // $8146..$817B: unsigned byte comparisons are intentional. A wrapped edge
  // does not form a continuous interval across the viewport boundary.
  const auto axis = [&](std::uint8_t shot, std::uint8_t enemy) {
    const auto upper = static_cast<std::uint8_t>(shot + target.width / 2);
    const auto lower = static_cast<std::uint8_t>(upper - target.width);
    return upper >= enemy && lower < enemy;
  };
  return axis(x, target.x) && axis(y, target.y);
}

std::vector<ShotHit> resolveShotHits(PlayerState& player, std::span<CollisionTarget> targets) {
  std::vector<ShotHit> hits;
  hits.reserve(player.shots.size());
  for (unsigned shotIndex = 0; shotIndex < player.shots.size(); ++shotIndex) {
    auto& shot = player.shots[shotIndex];
    if (!shot.active || shot.hit) continue;
    for (unsigned targetIndex = 0; targetIndex < targets.size(); ++targetIndex) {
      auto& target = targets[targetIndex];
      if (!shotOverlaps(shot.position.x, shot.position.y, target)) continue;
      target.hit = true;
      shot.hit = true;
      const auto partner = shotIndex < 3 ? shotIndex + 3 : shotIndex - 3;
      if (player.shots[partner].active) player.shots[partner].hit = true;
      hits.push_back({shotIndex,targetIndex});
      break;
    }
  }
  return hits;
}
}
