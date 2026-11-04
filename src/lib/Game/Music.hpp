#pragma once

#include <array>
#include <cstdint>
#include <optional>

namespace gyrus::game {

/// The original four-channel music sequencer, reconstructed as typed C++ state.
/// Call before SoundEffects once per VBI. This class does not synthesize audio.
class Music {
public:
    struct Channel {
        std::uint8_t frequency{};
        std::uint8_t control{};
        bool operator==(const Channel&) const = default;
    };

    bool start(std::uint8_t track) noexcept;
    /// Stops music, silencing only channels not owned by effects (mask bits 0..3).
    void stop(std::uint8_t effectMask = 0) noexcept;
    void tick(std::uint8_t effectMask = 0) noexcept;
    /// Drain immediately after start: modes 0/1 also start original effects 6/4.
    [[nodiscard]] std::optional<std::uint8_t> takeEffectRequest() noexcept;
    [[nodiscard]] bool isPlaying() const noexcept { return playing_; }
    [[nodiscard]] const std::array<Channel, 4>& channels() const noexcept { return channels_; }
    /// Writes performed by the last tick (bit0 AUDF, bit1 AUDC); stop adds AUDC writes.
    [[nodiscard]] const std::array<std::uint8_t, 4>& registerWrites() const noexcept { return writes_; }

private:
    struct Voice {
        std::uint16_t cursor{};
        std::uint8_t duration{};
        std::uint8_t envelope{};
        std::uint8_t repeat{};
        std::uint8_t frequency{};
    };
    void repeat(Voice& voice, std::uint8_t count) noexcept;
    bool advanceNote(unsigned channel, std::uint8_t effectMask) noexcept;
    void advancePercussion(std::uint8_t effectMask) noexcept;
    std::array<Channel, 4> channels_{};
    std::array<Voice, 4> voices_{};
    std::array<std::uint8_t, 4> writes_{};
    std::optional<std::uint8_t> effectRequest_;
    std::uint8_t mode_{};
    std::uint8_t sectionRepeats_{};
    std::uint8_t tickCountdown_{};
    std::uint8_t percussionCountdown_{};
    std::uint8_t percussionPattern_{};
    bool playing_{};
};

} // namespace gyrus::game
