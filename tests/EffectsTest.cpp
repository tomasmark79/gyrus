#include "../src/lib/Game/Effects.hpp"
#include <gtest/gtest.h>
#include <algorithm>

using namespace gyrus::game;

TEST(Effects, ScorePopupsMatchOriginalAllocationAndLifetime) {
    std::uint32_t aggregate = 2166136261u;
    const auto add = [&](std::uint8_t value) { aggregate = (aggregate ^ value) * 16777619u; };
    for (auto angle : {0,15,32,63,96,127}) {
        for (auto depth : {0,18,40,58}) {
            ScorePopups popups;
            for (unsigned tick = 0; tick < 320; ++tick) {
                if (tick == 0 || tick == 1 || tick == 2 || tick == 3 ||
                    tick == 160 || tick == 161 || tick == 162)
                    popups.spawn(angle, depth, (tick + angle / 16) % 8, tick & 1);
                popups.advance();
                Framebuffer frame{};
                popups.draw(frame);
                for (auto value : frame) add(value);
                for (const auto& slot : popups.slots()) {
                    add(slot.status); add(slot.timer); add(slot.angle); add(slot.depth);
                    add(slot.label); add(slot.x); add(slot.y);
                }
            }
        }
    }
    EXPECT_EQ(aggregate, 3681590433u);
}

TEST(Effects, HitExplosionsMatchOriginalAllocationMotionAndAnimation) {
    std::uint32_t aggregate = 2166136261u;
    const auto add = [&](std::uint8_t value) { aggregate = (aggregate ^ value) * 16777619u; };
    for (auto angle : {0,15,32,63,96,127}) {
        for (auto depth : {18,28,40,58}) {
            HitExplosions explosions;
            for (unsigned tick = 0; tick < 100; ++tick) {
                if (tick < 6 || (tick >= 50 && tick < 56))
                    explosions.spawn(angle, depth, (tick + angle / 8) % 14);
                explosions.advance(tick);
                Framebuffer frame{};
                explosions.draw(frame);
                for (auto value : frame) add(value);
                for (const auto& slot : explosions.slots()) {
                    add(slot.status); add(slot.timer); add(slot.angle); add(slot.depth);
                    add(slot.kind); add(slot.depthStep); add(slot.angleStep); add(slot.x); add(slot.y);
                }
            }
        }
    }
    EXPECT_EQ(aggregate, 410647429u);
}

TEST(Effects, PopupTransitionsSkipLaterSlotsForThatTick) {
    ScorePopups popups;
    for (unsigned i = 0; i < 3; ++i) EXPECT_TRUE(popups.spawn(32, 40, i));
    EXPECT_FALSE(popups.spawn(32, 40, 3));
    for (unsigned tick = 0; tick < 30; ++tick) popups.advance();
    for (const auto& slot : popups.slots()) EXPECT_EQ(slot.status, 128);
    popups.advance();
    EXPECT_EQ(popups.slots()[0].status, 6);
    EXPECT_EQ(popups.slots()[1].status, 128);
    EXPECT_EQ(popups.slots()[2].status, 128);
    popups.advance();
    EXPECT_EQ(popups.slots()[0].timer, 119);
    EXPECT_EQ(popups.slots()[1].status, 6);
    EXPECT_EQ(popups.slots()[2].status, 128);
}

TEST(Effects, PlayerExplosionMovesBeforeFirstDrawAndExpiresAfterEightTicks) {
    PlayerExplosion effect;
    effect.start(0, 80, 48);
    Framebuffer frame{};
    effect.draw(frame);
    EXPECT_TRUE(std::all_of(frame.begin(), frame.end(), [](auto v) { return v == 0; }));
    effect.advance(1);
    EXPECT_EQ(effect.countdown(), 0x88);
    effect.advance(8);
    EXPECT_EQ(effect.particles()[0].x, 84);
    EXPECT_EQ(effect.countdown(), 7);
    effect.draw(frame);
    EXPECT_TRUE(std::any_of(frame.begin(), frame.end(), [](auto v) { return v != 0; }));
    for (unsigned tick = 2; tick <= 8; ++tick) effect.advance(tick * 8);
    EXPECT_FALSE(effect.active());
    frame.fill(0);
    effect.draw(frame);
    EXPECT_TRUE(std::all_of(frame.begin(), frame.end(), [](auto v) { return v == 0; }));
}

TEST(Effects, PlayerExplosionMatchesOriginalInitializationAndPersistentXorFrames) {
    std::uint32_t aggregate = 2166136261u;
    const auto add = [&](std::uint8_t value) { aggregate = (aggregate ^ value) * 16777619u; };
    for (unsigned angle = 0; angle < 128; ++angle) {
        for (const auto [x, y] : {std::pair{80,48}, std::pair{2,2}, std::pair{155,91}}) {
            PlayerExplosion effect;
            effect.start(angle, x, y);
            for (unsigned tick = 0; tick < 64; ++tick) {
                effect.advance(tick);
                Framebuffer frame{};
                effect.draw(frame);
                for (auto value : frame) add(value);
                add(effect.countdown());
                for (const auto& particle : effect.particles()) {
                    add(particle.x); add(particle.y); add(particle.dx); add(particle.dy);
                }
            }
        }
    }
    // tools/reverse/effects_oracle.py: 384 original explosions, 24576 frames.
    EXPECT_EQ(aggregate, 330969541u);
}
