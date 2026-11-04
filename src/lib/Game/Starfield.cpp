#include "Starfield.hpp"
#include "Polar.hpp"

namespace gyrus::game {
void Starfield::advance(std::uint8_t frame, bool travel, bool stopNearCenter,
                        const RandomByte& random) {
    if (!travel && (frame & 1)) return;
    // $8D28 visits slot20 first. This order determines random consumption.
    for (auto it = stars_.rbegin(); it != stars_.rend(); ++it) {
        auto& star = *it;
        if (!star.active) {
            if (random() & 7) continue;
            star.depth = 36;
            star.angle = random() & 126;
            star.active = true;
        } else {
            if (star.depth < 2) {
                star.active = false;
                continue;
            }
            star.depth -= 2;
            if (stopNearCenter && star.depth < 18) {
                star.active = false;
                continue;
            }
        }
        const auto point = project(star.angle, star.depth);
        if (point.clipped) {
            star.active = false;
            continue;
        }
        star.x = point.x;
        star.y = point.y;
    }
}

void Starfield::draw(Framebuffer& frame) const noexcept {
    for (const auto& star : stars_) {
        if (!star.active) continue;
        // Startup's lives glyph overwrites the low byte of row0's address
        // table ($2000), pointing row0 at $3010 instead of visible $30C4.
        // Preserve this observed original quirk; those stars are offscreen.
        if (star.y == 0) continue;
        // The original uses byte-address arithmetic even at the horizontal
        // edge, so preserve its linear address rather than clipping x alone.
        const unsigned offset = star.y * playfieldWidth + star.x;
        if (offset < frame.size()) frame[offset] ^= 3;
    }
}
}
