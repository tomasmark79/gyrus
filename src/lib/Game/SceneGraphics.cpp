#include "SceneGraphics.hpp"
#include "SceneGraphicsData.hpp"
#include <algorithm>
#include <stdexcept>

namespace gyrus::game {
namespace {
void validate(const HudState& hud) {
    if (hud.activePlayer > 1) throw std::out_of_range("Gyruss active player must be 0 or 1");
    for (const auto& score : hud.scoreDigits)
        for (auto digit : score)
            if (digit > 9) throw std::out_of_range("Gyruss score digits must be decimal");
}

const scene_data::Planet& planetRecord(unsigned planet, bool title) {
    if (planet >= 6) throw std::out_of_range("Gyruss planet must be 0..5");
    return scene_data::planets[title ? 5 : planet];
}

bool maskPixel(unsigned offset, unsigned rows, int left, int top, int x, int y) {
    if (x < left || x >= left + 8 || y < top || y >= top + static_cast<int>(rows)) return false;
    return (scene_data::planetMasks[offset + y - top] & (128 >> (x - left))) != 0;
}
}

TextLine hudCharacters(const HudState& hud, bool title) {
    validate(hud);
    TextLine result{};
    const auto score = [&](unsigned player, unsigned column) {
        bool significant = false;
        for (unsigned digit = 0; digit < 6; ++digit) {
            significant |= hud.scoreDigits[player][digit] != 0;
            if (significant) result[column + digit] = 0xd0 | hud.scoreDigits[player][digit];
        }
        if (significant || !title) result[column + 6] = 0xd0;
    };
    if (title) {
        score(0, 0);
        score(1, 13);
    } else {
        // $930A copies the five original STAGE characters from $9414.
        constexpr std::array<std::uint8_t,5> stageLabel{0xf3,0xf4,0xe1,0xe7,0xe5};
        std::copy(stageLabel.begin(), stageLabel.end(), result.begin() + 12);
        const unsigned lives = std::min(hud.lives, 5u);
        for (unsigned i = 1; i < lives; ++i) result[i - 1] = 0xff;
        score(hud.activePlayer, 4);
        const auto stage = hud.stage & 255;
        if (stage >= 10) {
            result[18] = static_cast<std::uint8_t>(0xd0 + stage / 10);
            result[19] = 0xd0 + stage % 10;
        } else if (stage != 0) result[18] = 0xd0 + stage;
    }
    return result;
}

TextLine messageCharacters(const HudState& hud, const SceneOptions& options) {
    validate(hud);
    planetRecord(options.planet, options.title);
    TextLine result{};
    if (options.message == SceneMessage::None) return result;
    if (options.message == SceneMessage::Perfect) {
        std::copy_n(scene_data::perfectText.begin(), 20, result.begin());
        return result;
    }
    const unsigned index = static_cast<unsigned>(options.message) - 1;
    if (index >= 5) throw std::out_of_range("Invalid Gyruss scene message");
    std::copy_n(scene_data::messages.begin() + index * 20, 20, result.begin());
    switch (options.message) {
    case SceneMessage::WarpsTo:
        result[1] = 0xd0 | (options.warps & 255);
        if (options.warps == 1) result[7] = 0;
        std::copy_n(scene_data::planetNames.begin() + options.planet * 8, 8, result.begin() + 12);
        break;
    case SceneMessage::ReadyPlayer:
        result[16] = 0xd1 + hud.activePlayer;
        break;
    case SceneMessage::Planet:
        std::copy_n(scene_data::planetNames.begin() + options.planet * 8, 8, result.begin() + 6);
        break;
    case SceneMessage::BonusCount: {
        const unsigned count = options.bonusCount & 127;
        result[7] = result[11] = 0xd0 | (count / 10);
        result[8] = result[12] = 0xd0 | (count % 10);
        break;
    }
    default: break;
    }
    return result;
}

void drawPlanet(Framebuffer& frame, unsigned planet, bool title) {
    const auto& record = planetRecord(planet, title);
    const unsigned top = title ? 48 : record.y;
    const unsigned left = (record.x / 4) * 4;
    for (unsigned row = 0; row < record.height; ++row)
        for (unsigned column = 0; column < record.widthBytes * 4u; ++column) {
            const auto packed = scene_data::planetPixels[record.offset + row * record.widthBytes + column / 4];
            const auto pixel = (packed >> (6 - (column % 4) * 2)) & 3;
            frame[(top + row) * playfieldWidth + left + column] ^= pixel;
        }
}

SceneFrame composeScene(const Framebuffer& playfield, const HudState& hud,
                        const SceneOptions& options, PlayerColors playerColors,
                        const Framebuffer* playerLayer) {
    const auto& planet = planetRecord(options.planet, options.title);
    const auto hudLine = hudCharacters(hud, options.title);
    const auto message = messageCharacters(hud, options);
    const unsigned layout = options.message == SceneMessage::Perfect ? 3 :
                            options.message != SceneMessage::None ? 1 : options.title ? 2 : 0;
    const unsigned planetTop = options.title ? 48 : planet.y;
    const std::uint8_t textColor = options.title ? 0x28 : 0x44;
    const auto paletteForRow = [&](unsigned row) -> std::array<std::uint8_t, 4> {
        if (options.title) {
            // Stable title DLI sequence: row41 interrupt installs colored
            // lower-field palette; final interrupt restores next frame's gray.
            return row < 42 ? std::array<std::uint8_t,4>{0,4,10,14} :
                              std::array<std::uint8_t,4>{0,0x94,0x24,0x44};
        }
        if (options.usePlanetPalette) return {0, planet.pf0, planet.pf1, planet.pf2};
        if (options.useStageColors)
            return {0, options.stageColors[0], options.stageColors[1], options.stageColors[2]};
        const unsigned offset = (hud.stage & 3) * 3;
        return {0, scene_data::stagePalettes[offset], scene_data::stagePalettes[offset+1],
                   scene_data::stagePalettes[offset+2]};
    };
    SceneFrame result{};
    for (unsigned scanline = 0; scanline < sceneHeight; ++scanline) {
        const unsigned source = scene_data::scanlineLayouts[layout * sceneHeight + scanline];
        if (source == 65535) continue;
        if (source < playfieldHeight) {
            const auto palette = paletteForRow(source);
            for (int x = 0; x < sceneWidth; ++x) {
                const auto index = playfield[source * sceneWidth + x];
                auto color = index < 4 ? palette[index] : index <= 6 ? playerColors.atari[index - 4] : 0;
                if (index < 4 && (options.title || options.planetVisible)) {
                    // Fifth-player missiles sit behind player3 but in front
                    // of the bitmap under the game's PRIOR $11 setting.
                    if (maskPixel(planet.missileOffset, planet.missileRows,
                                  planet.x + planet.missileX, planetTop + planet.missileY, x, source))
                        color = textColor;
                    if (maskPixel(planet.playerOffset, planet.playerRows,
                                  planet.x + planet.playerX, planetTop + planet.playerY, x, source))
                        color = planet.playerColor;
                }
                result[scanline * sceneWidth + x] = color & 254;
            }
            continue;
        }
        const unsigned fontRow = source & 7;
        const std::uint8_t* text;
        switch ((source - 256) / 8) {
        case 0: text = hudLine.data(); break;
        case 1: text = message.data(); break;
        case 2: text = scene_data::copyrightText.data(); break;
        case 3: text = scene_data::perfectText.data(); break;
        case 4: text = scene_data::perfectText.data() + 20; break;
        default: throw std::logic_error("Invalid generated Gyruss scanline layout");
        }
        const auto palette = paletteForRow(scanline > 8 ? (scanline - 8) / 2 : 0);
        for (unsigned column = 0; column < 20; ++column) {
            const auto character = text[column];
            const auto bitmap = scene_data::font[(character & 63) * 8 + fontRow];
            const unsigned selection = character >> 6;
            const auto color = selection == 3 ? textColor : palette[selection + 1];
            for (unsigned bit = 0; bit < 8; ++bit)
                if (bitmap & (128 >> bit)) result[scanline * sceneWidth + column * 8 + bit] = color & 254;
        }
    }
    if (playerLayer) {
        // Player/missile DMA does not follow the character/bitmap display list.
        // Draw its fixed-position scanlines last, including over mode6 text.
        for (unsigned row = 0; row < playfieldHeight; ++row) {
            for (unsigned x = 0; x < playfieldWidth; ++x) {
                const auto index = (*playerLayer)[row * playfieldWidth + x];
                if (index >= 4 && index <= 6) {
                    const auto color = playerColors.atari[index - 4] & 254;
                    result[(8 + row * 2) * sceneWidth + x] = color;
                    result[(9 + row * 2) * sceneWidth + x] = color;
                }
            }
        }
    }
    return result;
}

std::uint8_t playerPlayfieldCollisions(const Framebuffer& field,
                                     const Framebuffer& playerLayer) noexcept {
    std::uint8_t mask = 0;
    for (std::size_t i = 0; i < field.size(); ++i) {
        if (playerLayer[i] >= 4 && playerLayer[i] <= 6 &&
            field[i] >= 1 && field[i] <= 3) {
            mask |= static_cast<std::uint8_t>(1u << (field[i] - 1));
        }
    }
    return mask;
}

Rgb classicAtariRgb(std::uint8_t color) {
    const unsigned offset = (color & 254) * 3;
    return {scene_data::classicPalette[offset], scene_data::classicPalette[offset + 1],
            scene_data::classicPalette[offset + 2]};
}
}
