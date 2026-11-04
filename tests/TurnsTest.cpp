#include <Game/Turns.hpp>
#include <gtest/gtest.h>
using namespace gyrus::game;

TEST(Turns, WaitsForIncomingEnemiesBeforeConsumingLife) {
  Turns state;
  state.deathCountdown = 1;
  state.incomingCount = 3;
  EXPECT_EQ(advanceDeath(state), TurnEvent::None);
  EXPECT_EQ(state.deathCountdown, 3);
  EXPECT_EQ(state.players[0].lives, 5);
}
TEST(Turns, AlternatesPlayersAndPreservesResumeState) {
  Turns state;
  state.deathCountdown = 1;
  state.waveIndex = 3;
  state.formationCount = 17;
  state.players[1].dualShot = true;
  EXPECT_EQ(advanceDeath(state), TurnEvent::ResumePlayer);
  EXPECT_EQ(state.activePlayer, 1);
  EXPECT_EQ(state.players[0].lives, 4);
  EXPECT_EQ(state.players[0].savedWave, 3);
  EXPECT_EQ(state.players[0].savedFormation, 17);
  EXPECT_FALSE(state.players[1].dualShot);
  EXPECT_EQ(state.deathCountdown, 255);
}
TEST(Turns, SinglePlayerEndsWhenLastLifeIsLost) {
  Turns state;
  state.players[0].lives = 1;
  state.players[1].lives = 0;
  state.deathCountdown = 1;
  EXPECT_EQ(advanceDeath(state), TurnEvent::GameOver);
  EXPECT_FALSE(state.playing);
}
TEST(Turns, StageProgressCyclesThroughChanceStageAndSixPlanets) {
  Turns state;
  state.players[0].level = 19;
  state.players[0].warps = 1;
  state.players[0].planetOffset = 40;
  advanceStage(state);
  EXPECT_EQ(state.players[0].warps, 0);
  EXPECT_EQ(state.players[0].protection, 1);
  advanceStage(state);
  EXPECT_EQ(state.players[0].warps, 3);
  EXPECT_EQ(state.players[0].planetOffset, 0);
  EXPECT_EQ(state.players[0].protection, 2);
  EXPECT_EQ(state.players[1].level, 1);
}
