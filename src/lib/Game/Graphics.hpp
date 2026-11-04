#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace gyrus::game {

inline constexpr int playfieldWidth = 160;
inline constexpr int playfieldHeight = 96;
using Framebuffer = std::array<std::uint8_t, playfieldWidth * playfieldHeight>;

struct IndexedImage {
    int width{};
    int height{};
    std::vector<std::uint8_t> pixels;
};

// Indices 0..3 are background/PF0/PF1/PF2, not RGB values.
IndexedImage titleImage();
// kind 0..28, depth 0..63 (increasing toward center), orientation 0..15.
// Includes the original right-hand padding needed by the packed blitter.
IndexedImage spriteImage(unsigned kind, unsigned depth, unsigned orientation);
void drawTitle(Framebuffer& frame);

// Original XOR operation and bounds rejection, with center coordinates as in
// the object arrays. Repeating the same draw restores the previous pixels.
bool xorSprite(Framebuffer& frame, unsigned kind, unsigned depth,
               unsigned orientation, int centerX, int centerY);

struct PlayerColors {
    // Raw Atari color codes for framebuffer indices 4, 5, 6 respectively.
    std::array<std::uint8_t, 3> atari{0x0e, 0x0e, 0x44};
};

// Composite the original three hardware-player masks onto the playfield.
// Overlay indices4..6 refer to PlayerColors; software XOR should be completed
// before this overlay. Angles wrap modulo128. depth>=60 draws nothing.
PlayerColors drawPlayer(Framebuffer& frame, std::uint8_t angle, std::uint8_t depth);

} // namespace gyrus::game
