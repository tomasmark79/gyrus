#pragma once

#include <array>
#include <cstdint>

namespace gyrus::game {

/// Frame-driven reconstruction of the ten original Gyruss sound effects.
/// Produces POKEY register values; waveform generation is a separate concern.
class SoundEffects {
public:
    struct Channel {
        std::uint8_t frequency{}; ///< AUDF: divider value, not a frequency in Hz.
        std::uint8_t control{};   ///< AUDC: distortion/volume control.
        bool active{};
        bool operator==(const Channel&) const = default;
    };

    /// Selects the original channel and replaces its effect. Invalid IDs do nothing.
    bool start(std::uint8_t id) noexcept;
    /// Advance one original VBI audio update. The frame counter wraps at 256.
    void tick(std::uint8_t frame) noexcept;
    [[nodiscard]] const std::array<Channel, 4>& channels() const noexcept { return channels_; }
    /// Writes performed by the last tick: bit0 AUDF, bit1 AUDC.
    [[nodiscard]] const std::array<std::uint8_t, 4>& registerWrites() const noexcept { return writes_; }

private:
    void startOnChannel(std::uint8_t id, unsigned channel) noexcept;
    struct Effect {
        std::uint8_t id{};
        std::uint8_t a{};
        std::uint8_t b{};
        std::uint8_t c{};
    };
    std::array<Channel, 4> channels_{};
    std::array<Effect, 4> effects_{};
    std::array<std::uint8_t, 4> writes_{};
};

} // namespace gyrus::game
