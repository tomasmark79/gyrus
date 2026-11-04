#include "../src/lib/Game/SceneGraphics.hpp"
#include <gtest/gtest.h>
#include <algorithm>

using namespace gyrus::game;

namespace {
template<class Container> std::uint32_t hash(const Container& values) {
    std::uint32_t result = 2166136261u;
    for (auto value : values) result = (result ^ value) * 16777619u;
    return result;
}
}

TEST(SceneGraphics, PlanetBitmapMatchesOriginal6502Routine) {
    constexpr std::array<std::uint32_t, 6> expected{
        679454675u, 4274240108u, 1694023024u, 2875061037u, 3296397790u, 644556517u};
    for (unsigned planet = 0; planet < 6; ++planet) {
        Framebuffer field{};
        drawPlanet(field, planet);
        EXPECT_EQ(hash(field), expected[planet]);
        field.fill(0);
        drawPlanet(field, planet, true);
        EXPECT_EQ(hash(field), 3109025509u);
    }
}

TEST(SceneGraphics, OriginalMessageTemplatesAndSubstitutions) {
    HudState hud;
    SceneOptions options;
    options.message = SceneMessage::WarpsTo;
    EXPECT_EQ(messageCharacters(hud, options), (TextLine{0xc0,0xd3,0xc0,0xf7,0xe1,0xf2,0xf0,0xf3,0xc0,0xf4,0xef,0xc0,0xee,0xe5,0xf0,0xf4,0xf5,0xee,0xe5,0xc0}));
    options.message = SceneMessage::ReadyPlayer;
    EXPECT_EQ(messageCharacters(hud, options), (TextLine{0xc0,0xc0,0xc0,0xf2,0xe5,0xe1,0xe4,0xf9,0xc0,0xf0,0xec,0xe1,0xf9,0xe5,0xf2,0xc0,0xd1,0xc0,0xc0,0xc0}));
    options.message = SceneMessage::BonusCount;
    options.bonusCount = 12;
    EXPECT_EQ(messageCharacters(hud, options), (TextLine{0xc0,0xd1,0xd0,0xd0,0xc0,0xf8,0xc0,0xd1,0xd2,0xc0,0xc0,0xd1,0xd2,0xd0,0xd0,0xc0,0xf0,0xf4,0xf3,0xc0}));
}

TEST(SceneGraphics, TitleScoresMatchOriginalRoutine) {
    HudState hud;
    hud.scoreDigits = {{{0,0,1,2,3,4}, {0,5,6,7,8,9}}};
    EXPECT_EQ(hudCharacters(hud, true), (TextLine{0,0,0xd1,0xd2,0xd3,0xd4,0xd0,0,0,0,0,0,0,0,0xd5,0xd6,0xd7,0xd8,0xd9,0xd0}));
}

TEST(SceneGraphics, GameplayHudUsesReserveShipsAndTrailingZero) {
    HudState hud;
    hud.scoreDigits[0] = {0,0,1,2,3,4};
    const auto line = hudCharacters(hud, false);
    EXPECT_EQ(line[0], 0xff);
    EXPECT_EQ(line[1], 0xff);
    EXPECT_EQ(line[2], 0);
    EXPECT_EQ(line[6], 0xd1);
    EXPECT_EQ(line[7], 0xd2);
    EXPECT_EQ(line[8], 0xd3);
    EXPECT_EQ(line[9], 0xd4);
    EXPECT_EQ(line[10], 0xd0);
    EXPECT_EQ(line[18], 0xd1);
}

TEST(SceneGraphics, TitleAndPerfectUseOriginalScanlineLayouts) {
    Framebuffer field{};
    field.fill(1);
    SceneOptions options;
    options.title = true;
    const auto title = composeScene(field, {}, options);
    EXPECT_EQ(title[8 * sceneWidth], 0x04);
    EXPECT_TRUE(std::all_of(title.begin() + 192 * sceneWidth, title.end(), [](auto c) { return c == 0; }));
    options.title = false;
    options.message = SceneMessage::Perfect;
    const auto perfect = composeScene(field, {}, options);
    EXPECT_TRUE(std::all_of(perfect.begin() + 184 * sceneWidth, perfect.end(), [](auto c) { return c == 0; }));
}

TEST(SceneGraphics, PlanetDrawingUsesOriginalXorOperation) {
    Framebuffer field{};
    const auto before = field;
    drawPlanet(field, 0);
    EXPECT_NE(field, before);
    drawPlanet(field, 0);
    EXPECT_EQ(field, before);
}

TEST(SceneGraphics, SeparatePlayerLayerKeepsHardwarePositionOverMessages) {
    Framebuffer field{}, player{};
    player[70 * playfieldWidth + 20] = 4;
    player[62 * playfieldWidth + 1] = 5;
    SceneOptions options;
    options.message = SceneMessage::ReadyPlayer;
    PlayerColors colors{{0x0e,0x44,0x44}};
    const auto scene = composeScene(field, {}, options, colors, &player);
    EXPECT_EQ(scene[148 * sceneWidth + 20], 0x0e);
    EXPECT_EQ(scene[149 * sceneWidth + 20], 0x0e);
    EXPECT_EQ(scene[146 * sceneWidth + 20], 0);
    EXPECT_EQ(scene[132 * sceneWidth + 1], 0x44);
    EXPECT_EQ(scene[133 * sceneWidth + 1], 0x44);
}

TEST(SceneGraphics, EarthUsesOriginalPlayerAndMissileMasks) {
    SceneOptions options;
    options.title = true;
    const auto scene = composeScene({}, {}, options);
    EXPECT_EQ(scene[(8 + 48 * 2) * sceneWidth + 79], 0x98);
    EXPECT_EQ(scene[(8 + 48 * 2) * sceneWidth + 81], 0x98);
    EXPECT_EQ(scene[(8 + 50 * 2) * sceneWidth + 79], 0x28);
    EXPECT_EQ(scene[(8 + 50 * 2) * sceneWidth + 80], 0x28);
}

TEST(SceneGraphics, ClassicPaletteProfileHasExplicitKnownColors) {
    EXPECT_EQ(classicAtariRgb(0), (Rgb{0,0,0}));
    EXPECT_EQ(classicAtariRgb(14), (Rgb{255,255,255}));
    EXPECT_EQ(classicAtariRgb(0x44), (Rgb{165,50,166}));
    EXPECT_EQ(classicAtariRgb(0x45), classicAtariRgb(0x44));
}

TEST(SceneGraphics, FreshGameHudMatchesLiveAtari800Snapshot) {
    HudState hud;
    hud.lives = 5;
    // analysis/game-start.a8s, first8069 after start: original $3FD8..$3FEB.
    EXPECT_EQ(hudCharacters(hud, false), (TextLine{
        255,255,255,255,0,0,0,0,0,0,208,0,243,244,225,231,229,0,209,0}));
}

TEST(SceneGraphics, PlanetHardwareLayersStayHiddenUntilArrival) {
    SceneOptions options;
    options.planet = 5;
    const auto hidden = composeScene({}, {}, options);
    // Live game-start has zero bytes throughout bitmap and PM memory.
    EXPECT_TRUE(std::all_of(hidden.begin() + 8 * sceneWidth, hidden.end(),
                           [](auto value) { return value == 0; }));
    options.planetVisible = true;
    const auto arrival = composeScene({}, {}, options);
    EXPECT_EQ(arrival[(8 + 41 * 2) * sceneWidth + 79], 0x98);
    options.planetVisible = false;
    options.title = true;
    const auto title = composeScene({}, {}, options);
    EXPECT_EQ(title[(8 + 48 * 2) * sceneWidth + 79], 0x98);
}

TEST(SceneGraphics, PlayerContactMatchesLiveAtari800Fixture) {
    // probe_gtia.py: PM $2238/$22B8/$2338, HPOS88/90/92. Physical
    // scanline112 maps to bitmap row36. PF bytes $366E=48,$366F=C0.
    Framebuffer field{}, player{};
    for (unsigned channel = 0; channel < 3; ++channel) {
        const unsigned offset = 36 * playfieldWidth + 40 + 2 * channel;
        player[offset] = 4 + channel;
        for (unsigned index = 0; index < 4; ++index) {
            field[offset] = index;
            EXPECT_EQ(playerPlayfieldCollisions(field, player),
                      index ? 1u << (index - 1) : 0u);
        }
        field[offset] = 0;
    }
    field[36 * playfieldWidth + 40] = 1;
    field[36 * playfieldWidth + 42] = 2;
    field[36 * playfieldWidth + 44] = 3;
    EXPECT_EQ(playerPlayfieldCollisions(field, player), 7);
    // The original normal-game mask F9=3 ignores PF2 stars.
    EXPECT_EQ(playerPlayfieldCollisions(field, player) & 3, 3);
    player.fill(0);
    player[40 * playfieldWidth + 40] = 4;
    EXPECT_EQ(playerPlayfieldCollisions(field, player), 0);
}
