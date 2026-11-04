#include "../src/lib/Game/Graphics.hpp"
#include <gtest/gtest.h>
#include <algorithm>

using namespace gyrus::game;

namespace {
std::uint32_t hash(const Framebuffer& frame) {
    std::uint32_t value = 2166136261u;
    for (auto pixel : frame) value = (value ^ pixel) * 16777619u;
    return value;
}
}

// Captured by the offline py65 oracle running original $9AF8 / $9C14 on
// initialized RAM, not generated from the C++ decoding implementation.
// Player raster origin is independently calibrated with live Atari800 GTIA
// collision probes: PM$2238 overlaps bitmaprow36, not40. The older oracle
// omitted ANTIC's initial8 scanlines and was corrected from that evidence.
TEST(Graphics, SoftwarePixelsMatchOriginal6502Routine) {
    struct Sample { unsigned kind, depth, orientation; int x, y; std::uint32_t expected; };
    const Sample samples[]{
        {3,8,0,80,48,4270920212u}, {3,8,7,81,48,4142040857u},
        {3,8,13,82,48,2218108397u}, {9,24,3,83,48,2338948019u},
        {12,48,9,1,1,1601490373u}, {0,40,0,80,48,1388997254u},
        {3,8,0,80,0,1630892882u}, {2,8,1,80,1,2847800552u},
    };
    for (const auto& sample : samples) {
        Framebuffer frame{};
        ASSERT_TRUE(xorSprite(frame, sample.kind, sample.depth, sample.orientation, sample.x, sample.y));
        EXPECT_EQ(hash(frame), sample.expected) << sample.kind << ':' << sample.orientation;
    }
}

TEST(Graphics, PlayerMasksMatchOriginal6502Routine) {
    struct Sample { std::uint8_t angle, depth; std::uint32_t expected; };
    const Sample samples[]{
        {0,12,2465570634u}, {16,12,3646575519u}, {32,12,237386243u},
        {48,12,215101279u}, {64,12,1972972338u}, {80,12,1914363439u},
        {96,12,3134107349u}, {112,12,749849343u}, {7,20,3824230458u},
        {57,28,3300566657u}, {95,40,3213360622u}, {13,58,2513555049u},
    };
    for (const auto& sample : samples) {
        Framebuffer frame{};
        drawPlayer(frame, sample.angle, sample.depth);
        EXPECT_EQ(hash(frame), sample.expected) << +sample.angle << ':' << +sample.depth;
    }
}

TEST(Graphics, DecodesOriginalTitleDimensions) {
    const auto image = titleImage();
    EXPECT_EQ(image.width, 104);
    EXPECT_EQ(image.height, 34);
    EXPECT_EQ(image.pixels.size(), 3536u);
}

TEST(Graphics, AllPlayerAnglesMatchOriginalAcrossSixDepths) {
    std::uint32_t aggregate = 2166136261u;
    for (std::uint8_t depth : {6,12,20,28,40,58}) {
        for (unsigned angle = 0; angle < 128; ++angle) {
            Framebuffer frame{};
            drawPlayer(frame, angle, depth);
            const auto value = hash(frame);
            for (unsigned shift = 0; shift < 32; shift += 8)
                aggregate = (aggregate ^ ((value >> shift) & 255)) * 16777619u;
        }
    }
    EXPECT_EQ(aggregate, 335323119u);
}

TEST(Graphics, SpriteDrawHasOriginalXorErasure) {
    Framebuffer frame{};
    for (std::size_t i = 0; i < frame.size(); ++i) frame[i] = i % 4;
    const auto original = frame;
    EXPECT_TRUE(xorSprite(frame, 3, 8, 0, 80, 48));
    EXPECT_NE(frame, original);
    EXPECT_TRUE(xorSprite(frame, 3, 8, 0, 80, 48));
    EXPECT_EQ(frame, original);
}

TEST(Graphics, RejectsSpriteCrossingOriginalRightAndBottomLimits) {
    Framebuffer frame{};
    const auto original = frame;
    EXPECT_FALSE(xorSprite(frame, 3, 8, 0, 159, 48));
    EXPECT_FALSE(xorSprite(frame, 3, 8, 0, 80, 95));
    EXPECT_EQ(frame, original);
}

TEST(Graphics, PlayerUsesOriginalMaskPlanes) {
    Framebuffer frame{};
    drawPlayer(frame, 0, 12);
    EXPECT_GT(std::count_if(frame.begin(), frame.end(), [](auto p) { return p >= 4; }), 0);
}
