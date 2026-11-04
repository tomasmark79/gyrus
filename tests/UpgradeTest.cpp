#include <Game/Upgrade.hpp>
#include <Game/Score.hpp>
#include <gtest/gtest.h>

using namespace gyrus::game;

TEST(UpgradeTest, SpawnInitializesSixPartsAndTheThreeOriginalTargetSlots) {
    UpgradeState state;
    UpgradeInputs input{32,12,0,3,0,1,true};
    ASSERT_EQ(spawnUpgrade(state,input),UpgradeSpawnResult::spawned);
    EXPECT_EQ(state.phase,0x8c);
    EXPECT_EQ(state.targetPhase,1);
    constexpr std::array<unsigned,6> angles{120,72,6,58,32,32};
    constexpr std::array<unsigned,6> depths{26,26,32,32,42,18};
    for(unsigned i=0;i<6;++i) {
        EXPECT_EQ(state.particles[i].actor.angle,angles[i]);
        EXPECT_EQ(state.particles[i].actor.depth,depths[i]);
    }
    EXPECT_EQ(state.targets[0].angle,44);
    EXPECT_EQ(state.targets[1].angle,32);
    EXPECT_EQ(state.targets[2].angle,20);
    EXPECT_EQ(state.targets[1].spriteType,0x10);
}

TEST(UpgradeTest, TooFewFormationEnemiesAbandonsThisSpecialGroup) {
    UpgradeState state;
    UpgradeInputs input{0,12,0,2,0,0,true};
    EXPECT_EQ(spawnUpgrade(state,input),UpgradeSpawnResult::abandoned);
    EXPECT_EQ(state.targetPhase,255);
    EXPECT_EQ(state.targetCount,0);
}

TEST(UpgradeTest, TargetWarmupRequestsForcedShotsBeforeItsInitialAppearance) {
    UpgradeState state;
    state.targetPhase=1;
    UpgradeInputs input{};
    updateUpgradeTargets(state,input);
    EXPECT_EQ(state.targetPhase,2);
    EXPECT_EQ(state.forcedShooter,1);
    state.targetPhase=5;
    updateUpgradeTargets(state,input);
    EXPECT_EQ(state.forcedShooter,2);
    state.targetPhase=7;
    for(auto& target:state.targets) {target.depth=50;target.angle=32;target.spriteType=4;}
    updateUpgradeTargets(state,input);
    EXPECT_EQ(state.targetPhase,8);
    EXPECT_EQ(state.animation,25);
    EXPECT_TRUE(state.targets[0].visible);
    EXPECT_EQ(state.targets[0].depth,28);
}

TEST(UpgradeTest, KillingMiddleTargetReleasesTheTwoCollectibleParts) {
    UpgradeState state;
    state.targetPhase=8;
    auto& middle=state.targets[1];
    middle.visible=true;middle.hit=true;middle.width=10;middle.angle=32;middle.depth=28;middle.spriteType=16;
    UpgradeInputs input{32,12,0,0,0,1,false};
    const auto events=updateUpgradeTargets(state,input);
    EXPECT_EQ(state.phase,0xc2);
    EXPECT_EQ(state.targetCount,2);
    EXPECT_EQ(state.particles[0].actor.angle,44);
    EXPECT_EQ(state.particles[1].actor.angle,18); // Original carry produces -14, not -12.
    EXPECT_EQ(state.particles[0].actor.depth,12);
    EXPECT_EQ(events.scoreUnits,10);
    ASSERT_EQ(events.sounds.size(),1);
    EXPECT_EQ(events.sounds[0],9);
}

TEST(UpgradeTest, CollectionUsesPostMovementAngleBucketAndRequiresBothParts) {
    UpgradeState state;
    state.phase=0xc2;
    state.particles[0].actor.angle=28;
    state.particles[1].actor.angle=36;
    state.particles[0].actor.depth=12;
    state.particles[1].actor.depth=12;
    UpgradeInputs input{32,12,0,0,0,1,false};
    EXPECT_TRUE(updateUpgradeParticles(state,input));
    EXPECT_EQ(state.dualShotLevel,1);
    EXPECT_EQ(state.phase,0);
    EXPECT_EQ(state.particles[0].actor.depth,0);
    EXPECT_EQ(state.particles[1].actor.depth,0);
}

TEST(UpgradeTest, FinalCountdownErasesPartsWithoutMovingTheirStoredCoordinates) {
    UpgradeState state;
    state.phase=1;
    auto& part=state.particles[0];
    part.actor.depth=26;part.actor.angle=32;part.actor.visible=true;part.actor.width=4;part.drawn=true;
    EXPECT_FALSE(updateUpgradeParticles(state,UpgradeInputs{}));
    EXPECT_EQ(state.phase,0);
    EXPECT_FALSE(part.drawn);
    EXPECT_EQ(part.actor.angle,32);
    EXPECT_EQ(part.actor.width,4); // Original bitmap erase does not clear width metadata.
}

TEST(UpgradeTest, ParticleRenderingRetainsDepthBeforeMovementAcrossBucketBoundary) {
    UpgradeState state;
    state.phase=0x8c;
    state.particles[5].actor.angle=32;
    state.particles[5].actor.depth=31;
    updateUpgradeParticles(state,{});
    const auto& actor=state.particles[5].actor;
    ASSERT_TRUE(state.particles[5].drawn);
    EXPECT_EQ(actor.depth,32);
    EXPECT_EQ(actor.renderedDepth,31);
    EXPECT_EQ(actor.renderedSpriteType,0x22);
    EXPECT_EQ(actor.renderedHeading,0);
}

TEST(UpgradeTest, ThreeHitsPreserveScoreCallOrderAndExtraLifeCrossing) {
    UpgradeState state;
    state.targetPhase=8;
    for(unsigned i=0;i<3;++i) {
        auto& target=state.targets[i];
        target.angle=32;target.depth=28;target.spriteType=i==1?16:4;
        target.width=10;target.visible=true;target.hit=true;
    }
    const auto events=updateUpgradeTargets(state,{});
    EXPECT_EQ(events.scoreAwards,(std::vector<std::uint8_t>{10,0,10,10,10,10,10}));
    EXPECT_EQ(events.scoreUnits,60);
    ScoreBoard score;
    score.players[0].digits={0,0,5,8,0,0}; // 58,000 points, before the 60,000 award.
    for(auto units:events.scoreAwards) score.add(0,units);
    EXPECT_EQ(score.points(0),61000);
    EXPECT_EQ(score.players[0].lives,6);
    for(auto digit:score.players[0].digits) EXPECT_LE(digit,9);
    ASSERT_EQ(events.bonuses.size(),3);
    EXPECT_FALSE(events.bonuses[0].incomingCarry);
    EXPECT_TRUE(events.bonuses[1].incomingCarry);
    EXPECT_FALSE(events.bonuses[2].incomingCarry);
}
