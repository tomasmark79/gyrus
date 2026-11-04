#include <Game/Audio.hpp>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace gyrus::game {
namespace {
template<class Channels>
void applyWrites(PokeyRegisters& registers, const Channels& channels,
                 const std::array<std::uint8_t, 4>& writes) noexcept {
    for (unsigned channel = 0; channel < 4; ++channel) {
        if (writes[channel] & 1) registers[channel].frequency = channels[channel].frequency;
        if (writes[channel] & 2) registers[channel].control = channels[channel].control;
    }
}
} // namespace

PokeySynth::PokeySynth(unsigned sampleRate, double clockHz) : clockHz_(clockHz) {
    if (!std::isfinite(clockHz) || clockHz < 192000 || clockHz > 10000000 || !setSampleRate(sampleRate)) {
        throw std::invalid_argument("Unsupported POKEY clock or sample rate");
    }
}

bool PokeySynth::setSampleRate(unsigned sampleRate) noexcept {
    if (sampleRate < 8000 || sampleRate > 192000) return false;
    sampleRate_ = sampleRate;
    // Approximate AC coupling, not a model of a particular Atari board's analogue network.
    dcCoefficient_ = std::exp(-2 * std::numbers::pi * 20 / sampleRate);
    return true;
}

float PokeySynth::clock(const PokeyRegisters& registers) noexcept {
    // Altirra Hardware Reference (2026-01-02), sections 5.3 and 5.5:
    // shared XNOR polynomials 1+x^3+x^4, 1+x^3+x^5, 1+x^12+x^17.
    // Bits reach channels 1..4 one machine clock apart. No per-voice PRNGs.
    history4_ = static_cast<std::uint8_t>((history4_ << 1) | (noise4_ & 1));
    history5_ = static_cast<std::uint8_t>((history5_ << 1) | (noise5_ & 1));
    history17_ = static_cast<std::uint8_t>((history17_ << 1) | (noise17_ & 1));
    noise4_ = (noise4_ >> 1) | ((1 ^ ((noise4_ ^ (noise4_ >> 1)) & 1)) << 3);
    noise5_ = (noise5_ >> 1) | ((1 ^ ((noise5_ ^ (noise5_ >> 2)) & 1)) << 4);
    noise17_ = (noise17_ >> 1) | ((1 ^ ((noise17_ ^ (noise17_ >> 5)) & 1)) << 16);

    if (++prescaler_ == 28) {
        prescaler_ = 0;
        for (unsigned channel = 0; channel < 4; ++channel) {
            if (timers_[channel] != 0) {
                --timers_[channel];
                continue;
            }
            // AUDF writes affect the next reload, preserving the running counter.
            timers_[channel] = registers[channel].frequency;
            const auto control = registers[channel].control;
            if ((control & 0x80) || ((history5_ >> channel) & 1)) {
                if (control & 0x20) outputs_[channel] = !outputs_[channel];
                else if (control & 0x40) outputs_[channel] = ((history4_ >> channel) & 1) != 0;
                else outputs_[channel] = ((history17_ >> channel) & 1) != 0;
            }
        }
    }
    unsigned level = 0;
    for (unsigned channel = 0; channel < 4; ++channel) {
        const auto control = registers[channel].control;
        if ((control & 0x10) || outputs_[channel]) level += control & 15;
    }
    return static_cast<float>(level) / 60.0f;
}

void PokeySynth::render(const PokeyRegisters& registers, std::span<float> samples) noexcept {
    const double cyclesPerSample = clockHz_ / sampleRate_;
    for (auto& sample : samples) {
        fractionalCycles_ += cyclesPerSample;
        const auto cycles = static_cast<unsigned>(fractionalCycles_);
        fractionalCycles_ -= cycles;
        double accumulated = 0;
        for (unsigned cycle = 0; cycle < cycles; ++cycle) accumulated += clock(registers);
        // Average over machine clocks to reduce aliasing, then remove the DC level.
        // This is a box filter, not a high-order reconstruction of the analogue DAC.
        const double input = accumulated / cycles;
        const double output = input - previousInput_ + dcCoefficient_ * previousOutput_;
        previousInput_ = input;
        previousOutput_ = output;
        sample = static_cast<float>(std::clamp(output * 0.8, -1.0, 1.0));
    }
}

bool Audio::startMusic(std::uint8_t track) noexcept {
    if (!music_.start(track)) return false;
    if (const auto effect = music_.takeEffectRequest()) effects_.start(*effect);
    return true;
}

bool Audio::startEffect(std::uint8_t effect) noexcept { return effects_.start(effect); }

void Audio::stopMusic() noexcept {
    const auto mask = effectMask();
    music_.stop(mask);
    // This external command writes only AUDC, independent of the previous tick's mask.
    for (unsigned channel = 0; channel < 4; ++channel) {
        if ((mask & (1U << channel)) == 0) registers_[channel].control = 0;
    }
}

void Audio::tick(std::uint8_t frame) noexcept {
    music_.tick(effectMask());
    applyWrites(registers_, music_.channels(), music_.registerWrites());
    if (const auto effect = music_.takeEffectRequest()) effects_.start(*effect);
    effects_.tick(frame);
    applyWrites(registers_, effects_.channels(), effects_.registerWrites());
}

std::uint8_t Audio::effectMask() const noexcept {
    std::uint8_t mask = 0;
    for (unsigned channel = 0; channel < 4; ++channel) {
        if (effects_.channels()[channel].active) mask |= static_cast<std::uint8_t>(1U << channel);
    }
    return mask;
}
} // namespace gyrus::game
