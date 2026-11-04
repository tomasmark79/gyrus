#include <Game/Score.hpp>
#include <gtest/gtest.h>

TEST(Score, AddsOriginalFiftyPointUnitsWithDecimalCarries) {
  gyrus::game::ScoreBoard board;
  board.add(0, 1);
  EXPECT_EQ(board.points(0), 50u);
  board.add(0, 19);
  EXPECT_EQ(board.points(0), 1000u);
  EXPECT_EQ(board.points(1), 0u);
}

TEST(Score, AwardsOriginalSixtyThousandBonus) {
  gyrus::game::ScoreBoard board;
  board.players[0].digits = {0,0,5,9,9,5}; // 59,950, trailing zero implicit.
  board.add(0, 1);
  EXPECT_EQ(board.points(0), 60000u);
  EXPECT_EQ(board.players[0].lives, 6);
  board.add(0, 1);
  EXPECT_EQ(board.players[0].lives, 6);
  board.players[1].digits = {0,0,5,9,9,5};
  board.add(1, 1);
  EXPECT_EQ(board.players[1].lives, 6);
}
