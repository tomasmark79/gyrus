#include <Game/Collision.hpp>
#include <gtest/gtest.h>

using namespace gyrus::game;
TEST(Collision, OriginalSquareHasExclusiveLowerAndInclusiveUpperEdge) {
  CollisionTarget target{81,49,3,true,false};
  EXPECT_TRUE(shotOverlaps(80,48,target));
  target.x = 78;
  EXPECT_FALSE(shotOverlaps(80,48,target));
  target.x = 79;
  EXPECT_TRUE(shotOverlaps(80,48,target));
  target.y = 50;
  EXPECT_FALSE(shotOverlaps(80,48,target));
}
TEST(Collision, ConsumesFirstTargetAndMarksDualShotPartner) {
  PlayerState player;
  player.shots[0].active = true;
  player.shots[0].position = {80,48,false};
  player.shots[3].active = true;
  player.shots[3].position = {80,48,false};
  std::array<CollisionTarget,2> targets{{{80,48,3,true,false},{80,48,3,true,false}}};
  const auto hits = resolveShotHits(player,targets);
  ASSERT_EQ(hits.size(),1u);
  EXPECT_EQ(hits[0].target,0u);
  EXPECT_TRUE(player.shots[0].hit);
  EXPECT_TRUE(player.shots[3].hit);
  EXPECT_TRUE(targets[0].hit);
  EXPECT_FALSE(targets[1].hit);
}
