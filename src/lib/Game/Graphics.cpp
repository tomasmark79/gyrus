#include "Graphics.hpp"
#include "GraphicsData.hpp"
#include "Polar.hpp"
#include <algorithm>
#include <stdexcept>

namespace gyrus::game {
namespace {
using namespace graphics_data;

static_assert([] {
    constexpr std::array<unsigned, 8> heights{11,6,5,3,11,6,5,3};
    for (unsigned bank = 0; bank < heights.size(); ++bank) {
        for (unsigned facing = 0; facing < 5; ++facing) {
            const unsigned end = shipOffsets[bank] + shipBytes[shipOffsets[bank] + facing] + 3 * heights[bank];
            if (end > shipBytes.size()) return false;
        }
    }
    return true;
}(), "Exported player masks must cover all three original plane reads");

const SpriteRecord& sprite(unsigned kind, unsigned depth, unsigned orientation, unsigned alignment) {
    if (kind >= 29 || depth >= 64 || orientation >= 16)
        throw std::out_of_range("Gyruss sprite kind, depth or orientation out of range");
    return spriteRecords[spriteIds[((kind * 8 + depth / 8) * 16 + orientation) * 2 + alignment]];
}

std::uint8_t pixel(const SpriteRecord& record, unsigned x, unsigned y) {
    return (spriteBytes[record.offset + y * record.stride + x / 4] >> (6 - (x % 4) * 2)) & 3;
}
}

IndexedImage titleImage() {
    IndexedImage image{104, 34, {}};
    image.pixels.reserve(104 * 34);
    for (auto byte : graphics_data::titleBytes)
        for (int shift = 6; shift >= 0; shift -= 2) image.pixels.push_back((byte >> shift) & 3);
    return image;
}

IndexedImage spriteImage(unsigned kind, unsigned depth, unsigned orientation) {
    const auto& record = sprite(kind, depth, orientation, 0);
    IndexedImage image{record.stride * 4, record.height, {}};
    image.pixels.reserve(image.width * image.height);
    for (int y = 0; y < image.height; ++y)
        for (int x = 0; x < image.width; ++x) image.pixels.push_back(pixel(record, x, y));
    return image;
}

void drawTitle(Framebuffer& frame) {
    const auto image = titleImage();
    for (int y = 0; y < image.height; ++y)
        std::copy_n(image.pixels.begin() + y * image.width, image.width,
                    frame.begin() + (y + 8) * playfieldWidth + 28);
}

bool xorSprite(Framebuffer& frame, unsigned kind, unsigned depth,
               unsigned orientation, int centerX, int centerY) {
    const auto& canonical = sprite(kind, depth, orientation, 0);
    const int size = canonical.height;
    const int x = std::max(0, centerX - size / 2);
    const int y = std::max(0, centerY - size / 2);
    // The original rejects equality too, rather than clipping these edges.
    if (size == 0 || x + size >= playfieldWidth || y + size >= playfieldHeight) return false;
    const auto& record = sprite(kind, depth, orientation, (x & 2) != 0);
    const int left = x & ~3;
    // The original font patch overwrites row0's lookup byte at $2000. It
    // consequently addresses $3010, 180 bytes before visible base $30C4.
    // Subsequent rows still advance40 bytes: preserve the resulting partial
    // visibility instead of treating it as an ordinary top-edge clip.
    const int base = (y == 0 ? -180 * 4 : y * playfieldWidth) + left;
    for (int row = 0; row < size; ++row) {
        for (int column = x & 1; column < record.stride * 4; ++column) {
            const int destination = base + row * playfieldWidth + column;
            if (destination >= 0 && destination < static_cast<int>(frame.size()))
                frame[destination] ^= pixel(record, column - (x & 1), row);
        }
    }
    return true;
}

PlayerColors drawPlayer(Framebuffer& frame, std::uint8_t angle, std::uint8_t depth) {
    PlayerColors colors;
    if (depth >= 60) return colors;
    angle &= 127;
    const auto projected = project(angle, depth);

    // Reconstruct the original quantized nose offset, including the one-byte
    // rounding carry that differs on the two halves of the orbit ($9C25).
    int adjustedX = projected.x - 4 - (projected.clipped ? 0 : 1);
    bool carry = adjustedX >= 0;
    adjustedX &= 255;
    if ((angle & 64) == 0) {
        const int sum = adjustedX + 2 + carry;
        adjustedX = sum & 255;
        carry = sum > 255;
    }
    const unsigned facing = (angle + 4 + carry) & 126;
    const int step = (facing >> 2) & 7;
    int noseOffset;
    if ((facing & 64) == 0) noseOffset = (facing & 32) ? step - 8 : -step;
    else noseOffset = (facing & 32) ? 8 - step : step;
    adjustedX = (adjustedX + noseOffset) & 255;

    std::array<int, 3> horizontal{(adjustedX + 44) & 255, 0, 0};
    if (depth >= 20) {
        horizontal[1] = horizontal[0];
        colors.atari[1] = 0x44;
    } else {
        const int sum = horizontal[0] + 8;
        horizontal[1] = sum & 255;
        if (facing & 64) horizontal[1] = (horizontal[1] - 15 - (sum > 255 ? 0 : 1)) & 255;
        constexpr std::array<unsigned, 16> thirdOffsets{4,5,8,9,9,9,8,5,4,3,0,255,255,255,0,3};
        const int third = adjustedX + thirdOffsets[facing >> 3];
        horizontal[2] = ((third & 255) + 44 + (third > 255)) & 255;
    }

    const unsigned rawSize = ((static_cast<unsigned>(depth - 12) & 255) >> 2) & 14;
    const unsigned sizeIndex = (rawSize >= 8 ? 6 : rawSize) / 2;
    const bool mirrored = (facing & 64) != 0;
    if (mirrored && rawSize == 0) {
        horizontal[1] = (adjustedX + 44) & 255;
        horizontal[0] = (horizontal[1] + 8) & 255;
    }
    constexpr std::array<unsigned, 4> heights{11, 6, 5, 3};
    constexpr std::array<unsigned, 4> verticalOffsets{0, 2, 2, 3};
    const unsigned height = heights[sizeIndex];
    const unsigned bank = graphics_data::shipOffsets[sizeIndex + (mirrored ? 4 : 0)];
    const unsigned orientation = facing >> 3;
    unsigned folded = orientation & 7;
    if (folded >= 5) folded = ((folded ^ 3) + 1) & 3;
    const unsigned source = bank + graphics_data::shipBytes[bank + folded];
    const bool reverse = orientation >= 5 && orientation < 13;
    // ANTIC starts at scanline8, so after24 blank +8 HUD scanlines the
    // bitmap begins at40. PM destination$2210 is scanline32: subtract4
    // double-line rows. Confirmed by live player/playfield collision probes.
    const int firstRow = (reverse ? projected.y + 10 - verticalOffsets[sizeIndex]
                                  : projected.y + verticalOffsets[sizeIndex]) - 4;

    // GTIA player0 has priority over player1, which has priority over player2.
    // Normal-width HPOS 48 is the left edge of the 160-pixel bitmap.
    for (int plane = 2; plane >= 0; --plane) {
        for (unsigned row = 0; row < height; ++row) {
            const int y = (firstRow + (reverse ? -static_cast<int>(row) : static_cast<int>(row))) & 255;
            if (y >= playfieldHeight) continue;
            const auto bits = graphics_data::shipBytes[source + plane * height + row];
            for (int bit = 0; bit < 8; ++bit) {
                const int x = horizontal[plane] - 48 + bit;
                if ((bits & (128 >> bit)) && x >= 0 && x < playfieldWidth)
                    frame[y * playfieldWidth + x] = 4 + plane;
            }
        }
    }
    return colors;
}
}
