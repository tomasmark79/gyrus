#include "Stage.hpp"
#include <algorithm>
#include <stdexcept>
namespace gyrus::game {
StageSetup initializeStage(Turns& turns, EnemyPaths& paths, EnemyState& enemies,
                           FormationControl& formation) {
  if (turns.activePlayer > 1) throw std::out_of_range("Invalid active player");
  auto& player = turns.players[turns.activePlayer];
  if ((turns.deathCountdown & 128) && player.savedFormation > 36)
    throw std::out_of_range("Saved formation exceeds the original object pool");
  StageSetup setup;
  setup.resumed = (turns.deathCountdown & 128) != 0;
  setup.normalStage = player.warps != 0;
  // Last LSR's carry is included by ADC at $92FA (normally offset is /8).
  paths.scoreChain = (player.planetOffset >> 3) + 2 + ((player.planetOffset >> 2) & 1);
  paths.phases.fill(0);
  paths.incomingCount = paths.preparedCount = 0;
  paths.nextWave = player.savedWave;
  formation.attackState.fill(0);
  const auto difficulty = formationControl(player.level);
  formation.randomTurnMask = difficulty.randomTurnMask;
  formation.launchMask = difficulty.launchMask;
  formation.maxIncoming = difficulty.maxIncoming;
  // Original CPU $9421 table, loaded from GYRUSS.atr.
  constexpr std::array<std::uint8_t, 4> reload{0, 255, 63, 31};
  setup.specialShotReload = reload[std::min<unsigned>(player.level >> 2, 3)];
  // Original $9343 reads player ONE's level even when player two is active.
  constexpr std::array<std::array<std::uint8_t, 3>, 4> colors{{
    {70,40,68}, {120,40,116}, {42,40,36}, {200,40,196}}}; // $8DD7
  setup.playfieldColors = colors[turns.players[0].level & 3];
  setup.music = (player.warps >> 1) + ((player.warps & 1) ? 4 : 2);
  setup.upgradeTargetCount = 3;
  if (!setup.normalStage) player.special = 3;
  if (player.special) setup.upgradeTargetCount = 0;
  setup.upgradeTargetPhase = player.special;
  setup.enemyShotCap = std::min<unsigned>(static_cast<std::uint8_t>(player.level + 1) >> 1, 4);
  setup.enemyShotCooldownReload = 66 - setup.enemyShotCap * 8;
  enemies.formationCount = setup.resumed ? player.savedFormation : 0;
  enemies.nextFormationSlot = enemies.formationCount;
  enemies.remainingBonusEnemies = 40;
  for (auto& actor : enemies.incoming) {
    actor.visible = actor.hit = actor.formationSlotReserved = false;
    actor.width = 0;
  }
  for (unsigned i = 0; i < enemies.formation.size(); ++i) {
    auto& actor = enemies.formation[i];
    actor.visible = actor.hit = false;
    actor.width = 0;
    if (i < enemies.formationCount) {
      actor.spriteType = 22; // $87AF selects the fixed $AD7C descriptor.
      projectEnemyActor(actor);
    }
  }
  setup.messageOffset = setup.resumed ? 20 : (setup.normalStage ? 0 : 40);
  setup.playerPosition = project(0, 12);
  setup.hudLevel = player.level;
  setup.hudLives = player.lives;
  turns.deathCountdown = 0;
  turns.incomingCount = 0;
  turns.formationCount = enemies.formationCount;
  turns.waveIndex = paths.nextWave;
  turns.specialCount = setup.upgradeTargetCount;
  return setup;
}
}
