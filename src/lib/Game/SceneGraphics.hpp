#pragma once

#include "Graphics.hpp"
#include <array>
#include <cstdint>

namespace gyrus::game {

inline constexpr int sceneWidth = 160;
inline constexpr int sceneHeight = 200;
// One color-clock horizontally, one scanline vertically: display at 320x200.
// Values are raw Atari color codes, unlike the indexed 160x96 playfield.
using SceneFrame = std::array<std::uint8_t, sceneWidth * sceneHeight>;
using TextLine = std::array<std::uint8_t, 20>;

struct HudState {
    std::array<std::array<std::uint8_t, 6>, 2> scoreDigits{};
    unsigned activePlayer = 0;
    unsigned lives = 3;
    unsigned stage = 1;
};

enum class SceneMessage { None, WarpsTo, ReadyPlayer, ChanceStage, Planet, BonusCount, Perfect };

struct SceneOptions {
    bool title = false;
    bool planetVisible = false; // Title always displays Earth; gameplay only at arrival.
    unsigned planet = 0; // Neptune, Uranus, Saturn, Jupiter, Mars, Earth.
    SceneMessage message = SceneMessage::None;
    unsigned warps = 3;
    unsigned bonusCount = 0;
    // $9F38's colors during travel; normal stages use $9343's four palettes.
    bool usePlanetPalette = false;
    bool useStageColors = false;
    std::array<std::uint8_t, 3> stageColors{};
};

TextLine hudCharacters(const HudState& hud, bool title);
TextLine messageCharacters(const HudState& hud, const SceneOptions& options);

// Original XOR background bitmap. Call before software sprites/player overlays.
// Title always shows Earth at its original lower location.
void drawPlanet(Framebuffer& frame, unsigned planet, bool title = false);

// Current bitmap contact mask equivalent to OR(P0PF,P1PF,P2PF): indices
// 1/2/3 map to bits0/1/2. Inputs must be separate software and player layers.
// This does not retain hardware latches or include mode6 text (PF3).
std::uint8_t playerPlayfieldCollisions(const Framebuffer& field,
                                     const Framebuffer& playerLayer) noexcept;

// Original mode6 HUD, message/copyright layouts, doubled bitmap scanlines and
// player3/missile planet layers. A separate playerLayer (zero with indices4..6)
// retains physical scanline positions and priority across message layouts.
// Without it, inline playfield indices4..6 remain supported for compatibility.
SceneFrame composeScene(const Framebuffer& playfield, const HudState& hud,
                        const SceneOptions& options, PlayerColors playerColors = {},
                        const Framebuffer* playerLayer = nullptr);

struct Rgb { std::uint8_t r, g, b; bool operator==(const Rgb&) const = default; };
// Explicit classic Atari800 default.act profile, not a physical PAL calibration.
Rgb classicAtariRgb(std::uint8_t color);

} // namespace gyrus::game
