#pragma once
#include "Formation.hpp"
#include "Turns.hpp"

namespace gyrus::game {
// Results of $92E6 initialization. Raster/audio/HUD actions are observational
// events; the caller applies them to its renderer and other typed controllers.
struct StageSetup {
  bool normalStage{}, resumed{};
  bool returnToFormation = false;
  std::uint8_t specialShotReload{}, enemyShotCap{}, enemyShotCooldownReload{};
  std::uint8_t upgradePhase{}, upgradeTargetPhase{}, upgradeTargetCount{};
  std::uint8_t forcedShooter{}, specialShotCountdown{}, transition{};
  std::uint8_t explosionCounter{}, pickupCounter{}; // $90, $B7
  std::uint8_t playerAngle = 0, playerDepth = 12, deathCountdown = 0;
  ProjectedPoint playerPosition{};
  std::uint8_t music{}, messageOffset{}, messageCountdown = 150;
  std::array<std::uint8_t, 3> playfieldColors{};
  std::uint8_t backgroundColor = 0x44, playerColor = 0x84, missileColor = 0x0e;
  std::uint8_t hudLevel{}, hudLives{};
  // $848A clears object width/state for every object and invalidates all stars.
  bool clearObjectVisibility = true, clearStars = true, clearCollisionLatch = true;
};

// Does not prepare a wave, reset grid coordinates, $56/$5B, path cursors,
// descriptor mutations, firing cooldown $A7, or upgrade timers $AF/$B0/$BB.
StageSetup initializeStage(Turns&, EnemyPaths&, EnemyState&, FormationControl&);
}
