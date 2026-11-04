#include <Game/FixedStep.hpp>
#include <gtest/gtest.h>
#include <limits>

TEST(FixedStepTest, SimulationRateIsIndependentOfDisplayRefresh) {
  for (int refresh : {30, 50, 60, 120, 144}) {
    gyrus::game::FixedStep clock;
    unsigned ticks = 0;
    for (int frame = 0; frame < refresh * 10; ++frame)
      clock.advance(1.0 / refresh, [&] { ++ticks; });
    EXPECT_EQ(ticks, 500u) << refresh;
  }
}

TEST(FixedStepTest, BackgroundResumeDoesNotSimulateMinutesOfMissedInput) {
  gyrus::game::FixedStep clock;
  unsigned ticks = 0;
  clock.advance(120.0, [&] { ++ticks; });
  EXPECT_EQ(ticks, 5u);
  clock.advance(-1.0, [&] { ++ticks; });
  clock.advance(std::numeric_limits<double>::quiet_NaN(), [&] { ++ticks; });
  EXPECT_EQ(ticks, 5u);
}
