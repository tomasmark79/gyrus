#include <Game/EnemyShots.hpp>
#include <gtest/gtest.h>

using namespace gyrus::game;

TEST(EnemyShotsTest, SpawnUsesFirstAlignedShooterAndExactRandomOffset) {
    EnemyShots shots;
    std::array<EnemyShooter,49> sources{};
    sources[0] = {32,40,true,false};
    sources[1] = {32,50,true,false};
    unsigned reads=0;
    EnemyShotSpawnInputs input{4,17,32,true,[&] { ++reads; return std::uint8_t{0}; }};
    const auto result=spawnEnemyShot(shots,sources,input);
    ASSERT_EQ(result.status,EnemyShotSpawnStatus::spawned);
    EXPECT_EQ(result.index,0);
    EXPECT_EQ(shots.shots[0].angle,30);
    EXPECT_EQ(shots.shots[0].depth,40);
    EXPECT_TRUE(shots.shots[0].pendingDraw);
    EXPECT_EQ(shots.cooldown,17);
    EXPECT_EQ(reads,2);
}

TEST(EnemyShotsTest, EqualityCapBlocksButGreaterCountRetainsOriginalBranchBehavior) {
    EnemyShots shots;
    std::array<EnemyShooter,49> sources{};
    sources[0]={32,40,true,false};
    EnemyShotSpawnInputs input{2,7,32,false,[] { return std::uint8_t{0}; }};
    shots.activeCount=2;
    EXPECT_EQ(spawnEnemyShot(shots,sources,input).status,EnemyShotSpawnStatus::none);
    shots.activeCount=3;
    shots.cooldown=20;
    EXPECT_EQ(spawnEnemyShot(shots,sources,input).status,EnemyShotSpawnStatus::spawned);
}

TEST(EnemyShotsTest, FullPoolReportsTheUnresolvedOriginalAllocationBoundary) {
    EnemyShots shots;
    for(auto& shot:shots.shots) shot.active=true;
    shots.activeCount=5;
    std::array<EnemyShooter,49> sources{};
    sources[0]={32,40,true,false};
    EnemyShotSpawnInputs input{4,7,32,true,[] { return std::uint8_t{0}; }};
    EXPECT_EQ(spawnEnemyShot(shots,sources,input).status,EnemyShotSpawnStatus::outsideShotPool);
}

TEST(EnemyShotsTest, FirstDrawPreservesDepthThenMovesOutwardEveryFourthTick) {
    EnemyShots shots;
    shots.activeCount=1;
    auto& shot=shots.shots[0];
    shot.angle=32;shot.depth=40;shot.active=true;shot.pendingDraw=true;
    EXPECT_EQ(updateEnemyShots(shots,1),0);
    EXPECT_TRUE(shot.pendingDraw);
    EXPECT_EQ(updateEnemyShots(shots,4),0);
    EXPECT_FALSE(shot.pendingDraw);
    EXPECT_EQ(shot.depth,40);
    EXPECT_EQ(shot.spriteType,0x26);
    EXPECT_EQ(updateEnemyShots(shots,8),0);
    EXPECT_EQ(shot.depth,38);
    EXPECT_EQ(updateEnemyShots(shots,12),0);
    EXPECT_EQ(shot.depth,36);
    EXPECT_EQ(shot.spriteType,0x24);
}

TEST(EnemyShotsTest, SpecialShotUsesIndependentTimerAndXorAim) {
    EnemyShots shots;
    EXPECT_FALSE(spawnSpecialEnemyShot(shots,2,31,0,true));
    EXPECT_EQ(shots.specialCountdown,1);
    EXPECT_TRUE(spawnSpecialEnemyShot(shots,2,31,0,true));
    EXPECT_EQ(shots.shots[4].angle,29);
    EXPECT_EQ(shots.shots[4].depth,50);
    EXPECT_EQ(shots.shots[4].spriteType,0x18);
    EXPECT_EQ(shots.activeCount,1);
    EXPECT_FALSE(spawnSpecialEnemyShot(shots,2,31,0,true));
}

TEST(EnemyShotsTest, ConfirmedCollisionStartsDeathAndHonorsOriginalGates) {
    PlayerGates gates;
    PlayerHitInputs input{{0,2,0},2,true};
    auto events=resolvePlayerHit(gates,input);
    EXPECT_TRUE(events.collisionLatchCleared);
    EXPECT_TRUE(events.deathStarted);
    EXPECT_EQ(gates.deathCountdown,90);
    EXPECT_EQ(gates.explosion,0x88);
    EXPECT_FALSE(resolvePlayerHit(gates,input).deathStarted);
    gates.deathCountdown=0;gates.transition=1;
    EXPECT_FALSE(resolvePlayerHit(gates,input).collisionLatchCleared);
    gates.transition=0;input.normalStage=false;
    EXPECT_FALSE(resolvePlayerHit(gates,input).deathStarted);
}
