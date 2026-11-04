#include <Game/GameSession.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include "PlayerShotRasterData.hpp"
using namespace gyrus::game;

TEST(GameSession, PlayerShotsMatchOriginalRasterAtEveryAngleAndTravelDepth) {
  GameSession game;
  Framebuffer baseline{}, player{};
  (void)game.draw(baseline, player);
  const auto rasterHash = [](const Framebuffer& frame) {
    std::uint32_t hash = 2166136261u;
    for (auto value : frame) hash = (hash ^ value) * 16777619u;
    return hash;
  };
  unsigned wrongHeadingCases = 0;
  for (unsigned angle = 0; angle < 128; angle += 2) {
    for (unsigned depth = 12; depth <= 54; depth += 2) {
      auto& shot = game.player.shots[0];
      shot.angle = angle;
      shot.depth = depth;
      shot.position = project(angle, depth);
      shot.active = !shot.position.clipped;
      Framebuffer actual{};
      (void)game.draw(actual, player);
      // Session's planet/background is unchanged; original XOR composition
      // lets us isolate precisely the player's software-shot raster.
      for (unsigned i = 0; i < actual.size(); ++i) actual[i] ^= baseline[i];
      const auto expected = gyrus::test::playerShotRasterHashes[(angle/2)*22+(depth-12)/2];
      EXPECT_EQ(rasterHash(actual), expected) << "angle=" << angle << " depth=" << depth;
      Framebuffer oldHeading{};
      if (shot.active)
        (void)xorSprite(oldHeading, 0, depth, angle/8, shot.position.x, shot.position.y);
      wrongHeadingCases += rasterHash(oldHeading) != expected;
    }
  }
  EXPECT_EQ(wrongHeadingCases, gyrus::test::wrongHeadingCases);
  EXPECT_GT(wrongHeadingCases, 0u);
}

TEST(GameSession, ReachesFormationCombatThroughFourWaves) {
  GameSession game;
  game.damageEnabled = false;
  game.begin();
  for (unsigned tick = 0; tick < 4000; ++tick) game.step(15, false);
  EXPECT_TRUE(game.formationPhase);
  EXPECT_EQ(game.paths.nextWave, 5);
  EXPECT_EQ(game.paths.collisionMask, 3);
  EXPECT_LE(game.enemies.formationCount, 36);
}

TEST(GameSession, SustainedMovementAndFirePreserveEnemyCounts) {
  for (unsigned seed : {1u, 42u, 0x12345678u}) {
    GameSession game;
    game.damageEnabled = false;
    game.begin();
    game.randomState = seed;
    for (unsigned tick = 0; tick < 10000; ++tick) {
      game.step((tick / 300) % 2 ? 7 : 11, tick % 8 == 0);
      ASSERT_EQ(game.enemies.formationCount,
                std::count_if(game.enemies.formation.begin(), game.enemies.formation.end(),
                              [](const auto& e) { return e.visible; })) << seed << ':' << tick;
      ASSERT_LE(game.paths.incomingCount, 10) << seed << ':' << tick;
      ASSERT_LE(game.enemyShots.activeCount, 5) << seed << ':' << tick;
      if ((tick & 15) == 0) {
        Framebuffer field{}, player{};
        (void)game.draw(field, player);
        ASSERT_LE(*std::max_element(field.begin(), field.end()), 3);
        ASSERT_LE(*std::max_element(player.begin(), player.end()), 6);
      }
    }
    EXPECT_GT(game.score.points(0), 0u);
  }
}

TEST(GameSession, RestartRestoresFirstWaveAndZeroScore) {
  GameSession game;
  game.begin();
  for (unsigned tick = 0; tick < 1000; ++tick) game.step(7, tick % 8 == 0);
  game.begin();
  EXPECT_EQ(game.score.points(0), 0u);
  EXPECT_EQ(game.paths.nextWave, 0);
  EXPECT_EQ(game.paths.scoreChain, 2); // Wave preparation follows the initial message.
  EXPECT_EQ(game.gates.messageCountdown, 150);
  EXPECT_EQ(game.player.angle, 0);
  EXPECT_FALSE(game.formationPhase);
}

TEST(GameSession, DeathConsumesLifeAndHandsTurnToSecondPlayer) {
  GameSession game;
  game.begin(2);
  game.gates.messageCountdown = 0;
  game.paths.collisionMask = 3;
  game.collisionLatch = 2; // Live GTIA fixture establishes PF1's collision bit.
  game.step(15, false);
  ASSERT_EQ(game.gates.deathCountdown, 90);
  ASSERT_TRUE(game.playerExplosion.active());
  for (unsigned i = 0; i < 90; ++i) game.step(15, false);
  EXPECT_EQ(game.score.players[0].lives, 4);
  EXPECT_EQ(game.turns.activePlayer, 1);
  EXPECT_EQ(game.gates.deathCountdown, 0);
  EXPECT_EQ(game.scene.message, SceneMessage::ReadyPlayer);
  EXPECT_EQ(game.gates.messageCountdown, 149);
  EXPECT_TRUE(game.playing);
}

TEST(GameSession, LastLifeReturnsToTitleState) {
  GameSession game;
  game.begin();
  game.score.players[0].lives = 1;
  game.gates.deathCountdown = 1;
  game.step(15, false);
  EXPECT_FALSE(game.playing);
  EXPECT_EQ(game.score.players[0].lives, 0);
}

TEST(GameSession, ShootingThroughNormalAndChanceStagesAdvancesPlanet) {
  GameSession game;
  game.damageEnabled = false; // Isolate progression from the already tested death path.
  game.begin();
  constexpr std::array<std::uint8_t, 8> directions{13,9,11,10,14,6,7,5};
  for (unsigned i = 0; i < 200000 && game.turns.players[0].level < 4; ++i)
    game.step(directions[(i / 40) % directions.size()], i % 4 == 0);
  EXPECT_GE(game.turns.players[0].level, 4);
  EXPECT_EQ(game.turns.players[0].planetOffset, 8);
  EXPECT_GT(game.score.points(0), 0u);
  EXPECT_TRUE(game.playing);
}

TEST(GameSession, HighDifficultySupportsConcurrentOrdinaryAndSpecialShots) {
  GameSession game;
  game.damageEnabled = false;
  game.begin();
  game.turns.players[0].level = 9;
  game.turns.players[0].planetOffset = 16;
  game.startStage();
  ASSERT_EQ(game.stage.enemyShotCap, 4);
  ASSERT_NE(game.stage.specialShotReload, 0);
  for (unsigned i = 0; i < 12000; ++i) {
    ASSERT_NO_THROW(game.step(i % 128 < 64 ? 7 : 11, i % 4 == 0)) << "tick=" << i;
    ASSERT_LE(game.enemyShots.activeCount, 5);
  }
}

TEST(GameSession, CompleteSixPlanetCircuitUsingOnlyMovementAndFire) {
  GameSession game;
  game.damageEnabled = false;
  game.begin();
  std::array<bool, 6> visited{};
  constexpr std::array<std::uint8_t, 8> directions{13,9,11,10,14,6,7,5};
  for (unsigned i = 0; i < 60000 && game.turns.players[0].level < 24; ++i) {
    visited[game.turns.players[0].planetOffset / 8] = true;
    ASSERT_NO_THROW(game.step(directions[(i / 40) % 8], i % 4 == 0)) << i;
  }
  EXPECT_TRUE(std::all_of(visited.begin(), visited.end(), [](bool seen) { return seen; }));
  EXPECT_EQ(game.turns.players[0].level, 24);
  EXPECT_EQ(game.turns.players[0].planetOffset, 0);
  EXPECT_GT(game.score.points(0), 60000u);
  EXPECT_GT(game.score.players[0].lives, 5); // Includes the original score-based extra life.
}
