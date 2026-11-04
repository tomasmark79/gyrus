#include "../src/lib/Game/Starfield.hpp"
#include <gtest/gtest.h>
#include <algorithm>

using namespace gyrus::game;

TEST(Starfield, MatchesOriginalRoutineForThreeHundredTicks) {
    Starfield stars;
    std::uint32_t seed = 0x12345678u;
    unsigned reads = 0;
    const auto random = [&] {
        seed = seed * 1664525u + 1013904223u;
        ++reads;
        return static_cast<std::uint8_t>(seed >> 24);
    };
    std::uint32_t frameAggregate = 2166136261u;
    std::uint32_t stateAggregate = 2166136261u;
    for (unsigned tick = 0; tick < 300; ++tick) {
        stars.advance(tick & 255, tick >= 120 && tick < 180, tick >= 200, random);
        Framebuffer field{};
        stars.draw(field);
        std::uint32_t frameHash = 2166136261u;
        for (auto pixel : field) frameHash = (frameHash ^ pixel) * 16777619u;
        for (unsigned shift = 0; shift < 32; shift += 8)
            frameAggregate = (frameAggregate ^ ((frameHash >> shift) & 255)) * 16777619u;
        for (const auto& star : stars.stars()) {
            stateAggregate = (stateAggregate ^ unsigned(star.active)) * 16777619u;
            stateAggregate = (stateAggregate ^ (star.active ? star.angle : 0)) * 16777619u;
            stateAggregate = (stateAggregate ^ (star.active ? star.depth : 0)) * 16777619u;
        }
    }
    // Offline py65 $8D1E oracle: includes frame wrap, travel timing and wave2.
    EXPECT_EQ(frameAggregate, 2225442909u);
    EXPECT_EQ(stateAggregate, 1751748955u);
    EXPECT_EQ(reads, 1536u);
}

TEST(Starfield, NormalOddTicksDoNotConsumeRandomness) {
    Starfield stars;
    unsigned reads = 0;
    stars.advance(1, false, false, [&] { ++reads; return std::uint8_t{0}; });
    EXPECT_EQ(reads, 0u);
}

TEST(Starfield, SpawnsAllTwentySlotsWhenRandomAllows) {
    Starfield stars;
    unsigned reads = 0;
    stars.advance(2, false, false, [&] { ++reads; return std::uint8_t{0}; });
    EXPECT_EQ(reads, 40u);
    EXPECT_EQ(std::count_if(stars.stars().begin(), stars.stars().end(), [](const auto& star) { return star.active; }), 20);
    for (const auto& star : stars.stars()) {
        EXPECT_EQ(star.angle, 0);
        EXPECT_EQ(star.depth, 36);
    }
}

TEST(Starfield, IdenticalOverlappingStarsCancelAsOriginalXor) {
    Starfield stars;
    stars.advance(0, true, false, [] { return std::uint8_t{0}; });
    Framebuffer field{};
    stars.draw(field);
    EXPECT_TRUE(std::all_of(field.begin(), field.end(), [](auto value) { return value == 0; }));
}
