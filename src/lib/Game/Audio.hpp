#pragma once

#include <Game/Music.hpp>
#include <Game/SoundEffects.hpp>
#include <array>
#include <cstdint>
#include <span>

namespace gyrus::game {

struct PokeyChannel {
    std::uint8_t frequency{};
    std::uint8_t control{};
    bool operator==(const PokeyChannel&) const = default;
};
using PokeyRegisters = std::array<PokeyChannel, 4>;

/// AUDCTL=0 POKEY sound model: four unlinked /28 timers and shared noise.
/// Frame-level register changes and approximate analogue filtering; not cycle-exact.
class PokeySynth {
public:
    static constexpr double palClockHz = 1773450.0;
    static constexpr double ntscClockHz = 1789770.0;
    explicit PokeySynth(unsigned sampleRate = 48000, double clockHz = palClockHz);
    bool setSampleRate(unsigned sampleRate) noexcept;
    [[nodiscard]] unsigned sampleRate() const noexcept { return sampleRate_; }
    void render(const PokeyRegisters& registers, std::span<float> monoSamples) noexcept;

private:
    float clock(const PokeyRegisters& registers) noexcept;
    std::array<std::uint8_t, 4> timers_{};
    std::array<bool, 4> outputs_{};
    std::uint32_t noise4_{}, noise5_{}, noise17_{};
    std::uint8_t history4_{}, history5_{}, history17_{};
    unsigned prescaler_{};
    unsigned sampleRate_{48000};
    double clockHz_{palClockHz};
    double fractionalCycles_{};
    double dcCoefficient_{};
    double previousInput_{}, previousOutput_{};
};

/// Coordinates the original sequencers with one shared register bank.
class Audio {
public:
    explicit Audio(unsigned sampleRate = 48000) : synth_(sampleRate) {}
    bool startMusic(std::uint8_t track) noexcept;
    bool startEffect(std::uint8_t effect) noexcept;
    void stopMusic() noexcept;
    void tick(std::uint8_t frame) noexcept;
    void render(std::span<float> monoSamples) noexcept { synth_.render(registers_, monoSamples); }
    bool setSampleRate(unsigned rate) noexcept { return synth_.setSampleRate(rate); }
    [[nodiscard]] unsigned sampleRate() const noexcept { return synth_.sampleRate(); }
    [[nodiscard]] const PokeyRegisters& registers() const noexcept { return registers_; }
    [[nodiscard]] bool musicPlaying() const noexcept { return music_.isPlaying(); }
    [[nodiscard]] std::uint8_t effectMask() const noexcept;

private:
    Music music_;
    SoundEffects effects_;
    PokeyRegisters registers_{};
    PokeySynth synth_;
};

} // namespace gyrus::game
