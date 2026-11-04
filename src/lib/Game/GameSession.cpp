#include "GameSession.hpp"
#include <stdexcept>

namespace gyrus::game {
void GameSession::begin(unsigned players) {
  if (players < 1 || players > 2) throw std::out_of_range("Player count must be one or two");
  tick = 0;
  player = {};
  gates = {};
  paths = EnemyPaths{};
  enemies = EnemyState{};
  score = {};
  turns = {};
  if (players == 1) turns.players[1].lives = score.players[1].lives = 0;
  formation = {};
  enemyShots = {};
  upgrade = {};
  hazards = {};
  randomState = 0x12345678;
  audio = Audio{};
  playing = true;
  startStage();
  (void)audio.startMusic(0); // New-game introduction overrides stage music.
}

void GameSession::startStage() {
  stage = initializeStage(turns, paths, enemies, formation);
  player.angle = stage.playerAngle;
  player.depth = stage.playerDepth;
  player.shots = {};
  gates = {};
  gates.messageCountdown = stage.messageCountdown;
  gates.suppressFireSound = stage.normalStage;
  gates.dualShot = turns.players[turns.activePlayer].dualShot;
  enemyShots.shots = {};
  enemyShots.activeCount = 0;
  enemyShots.forcedShooter = stage.forcedShooter;
  enemyShots.specialCountdown = stage.specialShotCountdown;
  upgrade.phase = stage.upgradePhase;
  upgrade.targetPhase = stage.upgradeTargetPhase;
  upgrade.targetCount = stage.upgradeTargetCount;
  upgrade.forcedShooter = 0;
  upgrade.dualShotLevel = gates.dualShot;
  for (auto& actor : upgrade.targets) { actor.visible = actor.hit = false; actor.width = 0; }
  for (auto& particle : upgrade.particles) {
    particle.drawn = false;
    particle.actor.visible = particle.actor.hit = false;
    particle.actor.width = 0;
  }
  hazards.count = stage.pickupCounter;
  for (auto& actor : hazards.actors) { actor.visible = actor.hit = false; actor.width = 0; }
  stars.clear();
  playerExplosion.clear();
  hitExplosions.clear();
  popups.clear();
  formationPhase = stage.returnToFormation;
  collisionLatch = 0;
  bonusAwarded = false;
  bonusPending = false;
  const auto& active = turns.players[turns.activePlayer];
  scene = {};
  scene.planet = active.planetOffset / 8;
  scene.warps = active.warps;
  scene.useStageColors = true;
  scene.stageColors = stage.playfieldColors;
  scene.message = stage.resumed ? SceneMessage::ReadyPlayer :
                  stage.normalStage ? SceneMessage::WarpsTo : SceneMessage::ChanceStage;
  hud = {};
  hud.activePlayer = turns.activePlayer;
  hud.stage = stage.hudLevel;
  hud.lives = stage.hudLives;
  for (unsigned i = 0; i < 2; ++i) hud.scoreDigits[i] = score.players[i].digits;
  (void)audio.startMusic(stage.music);
}

void GameSession::applyEvents(const EnemyEvents& events) {
  // Preserve each original $982F call. The decimal adder subtracts ten only
  // once per column; combining several awards can leave a non-decimal digit
  // and skip the 60,000-point extra-life crossing. scoreUnits is only a total.
  for (auto amount : events.scoreAwards) score.add(turns.activePlayer, amount);
  for (auto sound : events.sounds) (void)audio.startEffect(sound);
  for (const auto& bonus : events.bonuses) (void)popups.spawn(bonus.angle, bonus.depth, bonus.label);
}

void GameSession::step(std::uint8_t joystick, bool triggerPressed) {
  if (!playing) return;
  if (bonusPending) {
    bonusPending = false;
    bonusAwarded = true;
    const auto remaining = enemies.remainingBonusEnemies & 127;
    if (!remaining) {
      for (unsigned i = 0; i < 10; ++i) score.add(turns.activePlayer, 20);
      scene.message = SceneMessage::Perfect;
      (void)audio.startMusic(2);
    } else {
      for (unsigned i = remaining; i < 40; ++i) score.add(turns.activePlayer, 2);
      scene.message = SceneMessage::BonusCount;
      scene.bonusCount = 40 - remaining;
    }
    gates.messageCountdown = 255;
  }
  if (damageEnabled) {
    Framebuffer field{}, layer{};
    (void)draw(field, layer);
    collisionLatch |= playerPlayfieldCollisions(field, layer);
  }
  ++tick;
  const auto randomByte = [&]() -> std::uint8_t {
    // Repeatable host source; not cycle-timed POKEY RANDOM.
    randomState = randomState * 1664525u + 1013904223u;
    return randomState >> 24;
  };
  const auto finish = [&] {
    for (unsigned i = 0; i < 2; ++i) hud.scoreDigits[i] = score.players[i].digits;
    hud.lives = score.players[turns.activePlayer].lives;
    audio.tick(tick);
  };
  playerExplosion.advance(tick);
  if (enemyShots.cooldown) --enemyShots.cooldown;
  sampleInput(player, joystick, triggerPressed);
  stars.advance(tick, gates.transition != 0, paths.collisionMask == 2, randomByte);
  UpgradeInputs upgradeInputs;
  upgradeInputs.playerAngle = player.angle;
  upgradeInputs.playerDepth = player.depth;
  upgradeInputs.tick = tick;
  upgradeInputs.formationCount = enemies.formationCount;
  upgradeInputs.deathCountdown = gates.deathCountdown;
  upgradeInputs.levelRank = turns.players[0].level;
  upgradeInputs.returnToFormation = formationPhase;
  EnemyInputs inputs;
  inputs.playerAngle = player.angle;
  inputs.deathCountdown = gates.deathCountdown;
  inputs.levelNumber = turns.players[turns.activePlayer].level;
  inputs.normalStage = stage.normalStage;
  inputs.randomByte = randomByte;
  inputs.returnToFormation = formationPhase;
  inputs.reuseVacantFormationSlots = true;
  // DLI order: shots, special targets, hazards, pickup particles, launch.
  if (!gates.messageCountdown) {
    (void)updateEnemyShots(enemyShots, formation.frameCounter);
    applyEvents(updateUpgradeTargets(upgrade, upgradeInputs));
    applyEvents(updateHazards(hazards, tick));
    (void)updateUpgradeParticles(upgrade, upgradeInputs);
    (void)attemptFormationLaunch(paths, enemies, formation, inputs);
  }
  gates.dualShot = upgrade.dualShotLevel != 0;
  hitExplosions.advance(tick);
  popups.advance();
  bool resumed = false;
  if (gates.deathCountdown) {
    for (unsigned i = 0; i < 2; ++i) turns.players[i].lives = score.players[i].lives;
    turns.players[turns.activePlayer].special = upgrade.targetPhase;
    turns.players[turns.activePlayer].dualShot = gates.dualShot;
    turns.deathCountdown = gates.deathCountdown;
    turns.incomingCount = paths.incomingCount;
    turns.formationCount = enemies.formationCount;
    turns.waveIndex = paths.nextWave;
    const auto event = advanceDeath(turns);
    gates.deathCountdown = turns.deathCountdown;
    for (unsigned i = 0; i < 2; ++i) score.players[i].lives = turns.players[i].lives;
    if (event == TurnEvent::GameOver) {
      playing = false;
      finish();
      return;
    }
    if (event == TurnEvent::ResumePlayer) {
      startStage();
      resumed = true;
    }
  } else {
    updatePlayerMovement(player, tick, gates);
    // $8C03 consumes eleven RANDOM reads for the ship's thruster mask.
    // Preserve shared RNG sequencing even before that raster effect is drawn.
    if (!(tick & 1) && !gates.transition && !gates.explosion)
      for (unsigned i = 0; i < 11; ++i) (void)randomByte();
    PlayerHitInputs hit;
    hit.playfieldCollisionBits[0] = collisionLatch;
    hit.collisionMask = paths.collisionMask;
    hit.normalStage = stage.normalStage;
    const auto event = resolvePlayerHit(gates, hit);
    if (event.collisionLatchCleared) collisionLatch = 0;
    if (event.deathStarted) {
      (void)audio.startEffect(0);
      const auto position = project(player.angle, player.depth);
      playerExplosion.start(player.angle, position.x, position.y);
    }
  }
  if (!resumed) {
  const auto shots = updatePlayerShots(player, gates); // $8956 after movement.
  if (shots.fireSound) (void)audio.startEffect(3);
  inputs.playerAngle = player.angle;
  inputs.deathCountdown = gates.deathCountdown;
  applyEvents(updateEnemyEntriesWithFormation(paths, enemies, formation, inputs));
  applyEvents(updateFormationGrid(enemies, formation));
  if (gates.deathCountdown) { finish(); return; }
  }

  // Main loop runs after the complete DLI, including incoming/grid updates.
  if (gates.messageCountdown && --gates.messageCountdown == 0 &&
      scene.message != SceneMessage::Planet) scene.message = SceneMessage::None;
  if (!paths.incomingCount && !gates.messageCountdown) {
    paths.preparedCount = 0;
    if (!formationPhase) {
      if (paths.nextWave < 4 ||
          (paths.nextWave == 4 && stage.normalStage && enemies.formationCount < 6)) {
        WaveSettings wave;
        wave.stageOffset = turns.players[turns.activePlayer].planetOffset;
        wave.waveIndex = paths.nextWave;
        wave.normalStage = stage.normalStage;
        if (wave.waveIndex == 4)
          do { wave.randomWave = randomByte() & 31; } while (wave.randomWave >= 24);
        prepareWave(paths, wave);
      } else {
        if (paths.nextWave == 4) ++paths.nextWave;
        paths.collisionMask = 3;
        formationPhase = true;
      }
    }
    if (formationPhase && !upgrade.spawnDelay) upgrade.spawnDelay = (randomByte() & 63) | 16;
    const bool clear = formationPhase && !enemies.formationCount && !enemyShots.activeCount &&
      !upgrade.targetCount && !upgrade.phase && !hazards.count &&
      !popups.slots()[0].status && !popups.slots()[1].status && !popups.slots()[2].status;
    if (clear && upgrade.targetPhase) {
      if (gates.transition & 128) {
        turns.players[turns.activePlayer].lives = score.players[turns.activePlayer].lives;
        turns.players[turns.activePlayer].dualShot = gates.dualShot;
        advanceStage(turns);
        startStage();
        finish();
        return;
      }
      if (!gates.transition) {
        if (!stage.normalStage && !bonusAwarded) {
          bonusPending = true;
        } else {
          gates.transition = player.depth;
          if (turns.players[turns.activePlayer].warps == 1) {
            scene.message = SceneMessage::Planet;
            gates.messageCountdown = 150;
            scene.usePlanetPalette = true;
            (void)audio.startMusic(1);
          } else {
            audio.stopMusic();
            (void)audio.startEffect(6);
          }
        }
      }
    }
  }
  std::array<CollisionTarget, 51> targets{};
  std::array<EnemyActor*, 51> actors{};
  actors[0] = &hazards.actors[1]; actors[1] = &hazards.actors[2];
  for (unsigned i = 0; i < 10; ++i) actors[i + 2] = &enemies.incoming[i];
  for (unsigned i = 0; i < 3; ++i) actors[i + 12] = &upgrade.targets[i];
  for (unsigned i = 0; i < 36; ++i) actors[i + 15] = &enemies.formation[i];
  for (unsigned i = 0; i < actors.size(); ++i) {
    const auto& e = *actors[i];
    targets[i] = {e.position.x, e.position.y, e.width, e.visible, e.hit};
  }
  for (const auto& hit : resolveShotHits(player, targets)) {
    auto& target = *actors[hit.target];
    target.hit = true;
    (void)hitExplosions.spawn(target.angle, target.depth, target.spriteType / 2);
  }
  std::array<EnemyShooter, 49> shooters{};
  for (unsigned i = 0; i < shooters.size(); ++i) {
    const auto& e = *actors[i + 2];
    shooters[i] = {e.angle, e.depth, e.visible, e.hit};
  }
  EnemyShotSpawnInputs firing;
  firing.cap = stage.enemyShotCap;
  firing.cooldownReload = stage.enemyShotCooldownReload;
  firing.aimAngle = player.aimAngle;
  firing.normalStage = stage.normalStage;
  firing.randomByte = randomByte;
  enemyShots.forcedShooter = upgrade.forcedShooter;
  const auto spawn = spawnEnemyShot(enemyShots, shooters, firing);
  upgrade.forcedShooter = enemyShots.forcedShooter;
  if (spawn.status == EnemyShotSpawnStatus::outsideShotPool)
    throw std::runtime_error("Original enemy shot allocation exceeded the reconstructed pool");
  upgradeInputs.playerAngle = player.angle;
  upgradeInputs.playerDepth = player.depth;
  upgradeInputs.formationCount = enemies.formationCount;
  upgradeInputs.deathCountdown = gates.deathCountdown;
  upgradeInputs.returnToFormation = formationPhase;
  (void)spawnUpgrade(upgrade, upgradeInputs);
  (void)spawnSpecialEnemyShot(enemyShots, stage.specialShotReload, player.angle,
                              gates.transition, stage.normalStage);
  (void)spawnHazards(hazards, {turns.players[turns.activePlayer].level,
      gates.messageCountdown, gates.transition, stage.normalStage, randomByte});
  finish();
}

PlayerColors GameSession::draw(Framebuffer& field, Framebuffer& playerLayer) const {
  field.fill(0); playerLayer.fill(0);
  drawPlanet(field, scene.planet);
  stars.draw(field);
  const auto drawEnemy = [&](const EnemyActor& enemy) {
    if (enemy.visible)
      (void)xorSprite(field, enemy.renderedSpriteType / 2, enemy.renderedDepth,
                      enemy.renderedHeading, enemy.position.x, enemy.position.y);
  };
  for (const auto& enemy : enemies.formation) drawEnemy(enemy);
  for (const auto& enemy : enemies.incoming) drawEnemy(enemy);
  for (const auto& enemy : upgrade.targets) drawEnemy(enemy);
  for (const auto& enemy : hazards.actors) drawEnemy(enemy);
  for (const auto& particle : upgrade.particles) if (particle.drawn) drawEnemy(particle.actor);
  for (const auto& shot : enemyShots.shots)
    if (shot.active && !shot.pendingDraw)
      (void)xorSprite(field, shot.spriteType / 2, shot.depth, shot.heading,
                      shot.position.x, shot.position.y);
  for (const auto& shot : player.shots)
    if (shot.active)
      // Descriptor $B5E4 has fixed heading zero; angle/8 corrupts three quadrants.
      (void)xorSprite(field, 0, shot.depth, 0, shot.position.x, shot.position.y);
  hitExplosions.draw(field);
  popups.draw(field);
  playerExplosion.draw(field);
  if (gates.deathCountdown) return {};
  return drawPlayer(playerLayer, player.angle, player.depth);
}
}
