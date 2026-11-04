#include <Game/SoundEffects.hpp>

namespace gyrus::game {
namespace {
struct Preset {
    unsigned channel;
    std::uint8_t a;
    std::uint8_t b;
    std::uint8_t c;
};

// Original tables $B9B3/$B9BD/$B9C7/$B9D1, with channel register offsets / 2.
// Provenance: GYRUSS.atr SHA-256 dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc.
constexpr std::array<Preset, 10> presets{{
    {3, 0x40, 0x18, 0x03}, {0, 0x50, 0x0f, 0x64},
    {2, 0x68, 0x00, 0x00}, {1, 0x00, 0x04, 0x00},
    {3, 0x00, 0x83, 0x00}, {0, 0x50, 0x0f, 0x70},
    {3, 0x8e, 0xd0, 0x37}, {3, 0x40, 0x10, 0x00},
    {2, 0x5b, 0x00, 0x01}, {1, 0x20, 0x60, 0x10},
}};

constexpr std::uint8_t byte(unsigned value) noexcept {
    return static_cast<std::uint8_t>(value);
}
} // namespace

void SoundEffects::startOnChannel(std::uint8_t id, unsigned channel) noexcept {
    const auto& preset = presets[id];
    effects_[channel] = {id, preset.a, preset.b, preset.c};
    channels_[channel].active = true;
}

bool SoundEffects::start(std::uint8_t id) noexcept {
    if (id >= presets.size()) return false;
    startOnChannel(id, presets[id].channel);
    return true;
}

void SoundEffects::tick(std::uint8_t frame) noexcept {
    writes_.fill(0);
    // Original $BB03 traverses channel 4 down to 1 after the music update.
    for (int index = 3; index >= 0; --index) {
        auto& channel = channels_[static_cast<unsigned>(index)];
        if (!channel.active) continue;
        auto& effect = effects_[static_cast<unsigned>(index)];
        auto& written = writes_[static_cast<unsigned>(index)];
        const auto finish = [&channel, &written] { channel = {}; written = 3; };
        const auto emit = [&channel, &written](std::uint8_t control, std::uint8_t frequency) {
            channel.control = control;
            channel.frequency = frequency;
            written = 3;
        };

        switch (effect.id) {
        case 0:
        case 7: { // $BB37/$BB3A: stepped distortion with decaying volume.
            if (effect.id == 0 && (frame & 1) == 0) break;
            effect.a = byte(effect.a + 0x20U);
            if (effect.a >= 0x60) {
                --effect.b;
                effect.a = 0;
            }
            const auto volume = byte((effect.b >> 1) & 0x0f);
            if (volume == 0) finish();
            else emit(byte(volume | effect.a), effect.c);
            break;
        }
        case 1:
        case 5: { // $BB84: descending divisor; restart sweep with quieter volume.
            auto frequency = byte(effect.a - 3U);
            if (frequency < 0x50) {
                frequency = effect.c;
                effect.b = byte(effect.b - 2U);
                if ((effect.b & 0x80) != 0) {
                    finish();
                    break;
                }
            }
            effect.a = frequency;
            emit(byte(effect.b + 0xa0U), frequency);
            break;
        }
        case 2: // $BB56: alternating silence/tone, finishing when divisor reaches $80.
            if ((frame & 1) == 0) {
                emit(0, effect.a);
            } else {
                ++effect.a;
                if ((effect.a & 0x80) != 0) finish();
                else emit(0xa6, effect.a);
            }
            break;
        case 3: // $BB76: divisor sweep until its 8-bit counter wraps.
            effect.a = byte(effect.a + 8U);
            if (effect.a == 0) finish();
            else emit(effect.b, effect.a);
            break;
        case 4: // $BB69: hold register output on even frames.
            if ((frame & 1) == 0) break;
            ++effect.a;
            if (effect.a == 0x7a) finish();
            else emit(effect.b, effect.a);
            break;
        case 6:
        case 9: { // $BB9E: narrowing sweep, followed by effect 4 on this same channel.
            const unsigned sum = static_cast<unsigned>(effect.a) + effect.c;
            if (sum > 255 || sum >= effect.b) {
                effect.b = byte(effect.b - 2U);
                if (effect.b < 0x30) {
                    startOnChannel(4, static_cast<unsigned>(index));
                    break; // Transition itself does not touch the output registers.
                }
                effect.c = byte((effect.b - 0x20U) >> 2);
                effect.a = 0x20;
            } else {
                effect.a = byte(sum);
            }
            emit(0xa4, effect.a);
            break;
        }
        case 8: // $BBCB: alternating divisors with a triangular volume envelope.
            effect.a = byte(0x30U - effect.a);
            if ((frame & 3) == 0) {
                effect.b = byte(effect.b + effect.c);
                if (effect.b == 0) {
                    finish();
                    break;
                }
                if (effect.b >= 0x0b) effect.c = 0xff;
            }
            emit(byte(effect.b + 0xa0U), effect.a);
            break;
        default:
            break;
        }
    }
}
} // namespace gyrus::game
