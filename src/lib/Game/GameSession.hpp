#pragma once
#include "Audio.hpp"
#include "Collision.hpp"
#include "Enemies.hpp"
#include "EnemyShots.hpp"
#include "Formation.hpp"
#include "Score.hpp"
#include "SceneGraphics.hpp"
#include "Starfield.hpp"
#include "Upgrade.hpp"
#include "Stage.hpp"
#include "Hazards.hpp"
#include "Effects.hpp"

namespace gyrus::game {
// Portable 50 Hz session coordinator; frontend owns timing and device output.
struct GameSession {
  Audio audio;
  HudState hud;
  EnemyPaths paths;
  EnemyState enemies;
  ScoreBoard score;
  FormationControl formation;
  Starfield stars;
  EnemyShots enemyShots;
  UpgradeState upgrade;
  Turns turns;
  StageSetup stage;
  Hazards hazards;
  PlayerExplosion playerExplosion;
  HitExplosions hitExplosions;
  ScorePopups popups;
  SceneOptions scene;
  bool playing = false, damageEnabled = true, bonusAwarded = false;
  bool bonusPending = false;
  std::uint8_t collisionLatch{};
  bool formationPhase{};
  std::uint32_t randomState = 0x12345678;
  PlayerState player;
  PlayerGates gates;
  std::uint8_t tick{};
  void begin(unsigned players = 1);
  void startStage();
  void applyEvents(const EnemyEvents& events);
  void step(std::uint8_t joystick, bool triggerPressed);
  PlayerColors draw(Framebuffer& field, Framebuffer& playerLayer) const;
};
}
