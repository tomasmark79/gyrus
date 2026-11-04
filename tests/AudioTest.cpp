#include <Game/Audio.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <vector>

using gyrus::game::Audio;
using gyrus::game::PokeyRegisters;
using gyrus::game::PokeySynth;

TEST(AudioTest, SharedRegistersMatchOriginalMusicEffectsReplacementPauseAndStop) {
    // Independent whole-VBI fixtures: combined_audio_oracle.py --fixtures.
    constexpr std::array<std::uint64_t, 8> expected{
        0x4d0cbcabecf1d5d6ULL, 0x0bd608935490e17bULL, 0x919ac0e0c26beff8ULL,
        0x2ebfdabc5f80751fULL, 0xe6f562be7cf4ff1bULL, 0xe24ccf4a46f9eb24ULL,
        0xc52b9dfeaea8da9aULL, 0xa455e1e9c0fcc314ULL};
    for (std::uint8_t track = 0; track < 8; ++track) {
        SCOPED_TRACE(unsigned(track));
        Audio audio;
        std::uint64_t hash = 14695981039346656037ULL;
        const auto append = [&hash](std::uint8_t value) { hash = (hash ^ value) * 1099511628211ULL; };
        for (unsigned frame = 0; frame < 16384; ++frame) {
            if (frame == 0 || frame == 8192) {
                ASSERT_TRUE(audio.startMusic(track));
            }
            if (frame % 173 == 0 || frame % 271 == 1) {
                ASSERT_TRUE(audio.startEffect(static_cast<std::uint8_t>((frame / 37) % 10)));
            }
            if (frame == 7777) audio.stopMusic();
            if (frame % 997 < 980) audio.tick(static_cast<std::uint8_t>(frame));
            for (const auto& channel : audio.registers()) {
                append(channel.frequency);
                append(channel.control);
            }
            append(audio.effectMask());
            append(static_cast<std::uint8_t>(audio.musicPlaying()));
        }
        EXPECT_EQ(hash, expected[track]);
    }
}

TEST(AudioTest, InvalidCommandsPreserveRegisterState) {
    Audio audio;
    ASSERT_TRUE(audio.startMusic(4));
    audio.tick(0);
    const auto before = audio.registers();
    EXPECT_FALSE(audio.startMusic(255));
    EXPECT_FALSE(audio.startEffect(255));
    EXPECT_EQ(audio.registers(), before);
    EXPECT_TRUE(audio.musicPlaying());
}

TEST(PokeySynthTest, SilentRegistersProduceExactSilence) {
    PokeySynth synth;
    std::vector<float> samples(4800, 1);
    synth.render({}, samples);
    EXPECT_TRUE(std::all_of(samples.begin(), samples.end(), [](float sample) { return sample == 0; }));
}

TEST(PokeySynthTest, PureToneUsesTheDocumentedClockDivideBy28AndTwo) {
    PokeySynth synth;
    PokeyRegisters registers{};
    registers[0] = {72, 0xaf};
    std::vector<float> samples(48000);
    synth.render(registers, samples);
    unsigned rising = 0;
    for (unsigned i = 1; i < samples.size(); ++i) {
        if (samples[i-1] <= 0 && samples[i] > 0) ++rising;
    }
    const auto expectedHz = PokeySynth::palClockHz / (28 * 73 * 2);
    EXPECT_NEAR(rising, expectedHz, 2);
    EXPECT_GT(*std::max_element(samples.begin(), samples.end()), 0.05f);
    EXPECT_LT(*std::min_element(samples.begin(), samples.end()), -0.05f);
}

TEST(PokeySynthTest, DistortionAndNoiseDelaysFollowDocumentedPolynomialPatterns) {
    // Altirra Hardware Reference 2026-01-02, 5.3/5.5: these two complete
    // polynomial periods and their one-clock inter-channel delays are explicit.
    constexpr std::string_view poly4 = "000011101100101";
    constexpr std::string_view poly5 = "0000011100100010101111011010011";
    // 17-bit XNOR 1+x^12+x^17, zero initial state, sampled cycles 28,56,... .
    constexpr std::string_view poly17 = "1001110100001011110001010101110001001101001110100011000010011111";
    constexpr std::array<std::uint8_t, 6> modes{0x00, 0x20, 0x40, 0x80, 0xa0, 0xc0};
    const double dc = std::exp(-2 * std::numbers::pi * 20 / 48000);
    for (const auto mode : modes) {
        // Long-polynomial vector here is for channel1; short-polynomial modes
        // additionally verify the staggered channel2..4 sample positions.
        const unsigned channelCount = (mode & 0x60) == 0 ? 1 : 4;
        for (unsigned channel = 0; channel < channelCount; ++channel) {
            SCOPED_TRACE(::testing::Message() << "mode=" << unsigned(mode) << " channel=" << channel);
            PokeySynth synth(48000, 48000.0 * 28);
            PokeyRegisters registers{};
            registers[channel] = {0, static_cast<std::uint8_t>(mode | 15)};
            std::array<float, 64> samples{};
            synth.render(registers, samples);
            bool bit = false;
            double previousInput = 0, previousOutput = 0;
            for (unsigned sample = 0; sample < samples.size(); ++sample) {
                const bool previousBit = bit;
                const unsigned position = 28 * (sample + 1) - 1 - channel;
                if ((mode & 0x80) || poly5[position % poly5.size()] == '1') {
                    if (mode & 0x20) bit = !bit;
                    else if (mode & 0x40) bit = poly4[position % poly4.size()] == '1';
                    else bit = poly17[sample] == '1';
                }
                // The timer fires on the final clock of this 28-cycle box-filter sample.
                const double input = (27 * double(previousBit) + double(bit)) / 28 * 0.25;
                const double output = input - previousInput + dc * previousOutput;
                EXPECT_NEAR(samples[sample], output * 0.8, 0.000002);
                previousInput = input;
                previousOutput = output;
            }
        }
    }
}

TEST(PokeySynthTest, VolumeOnlyOverridesWaveformAndDcDecays) {
    PokeyRegisters registers{};
    registers[0] = {72, 0x1f};
    PokeySynth synth;
    std::vector<float> samples(48000);
    synth.render(registers, samples);
    EXPECT_GT(samples.front(), 0.1f);
    EXPECT_LT(std::abs(samples.back()), 0.00001f);
    auto other = registers;
    other[0].control = 0xff;
    PokeySynth second;
    std::vector<float> same(samples.size());
    second.render(other, same);
    EXPECT_EQ(samples, same);
}

TEST(PokeySynthTest, RenderIsIndependentOfQueueChunkBoundariesAndStaysBounded) {
    PokeyRegisters registers{{{72,0xaf},{31,0x8f},{53,0x4f},{128,0x0f}}};
    PokeySynth whole(44100), chunks(44100);
    std::vector<float> expected(44100), actual(44100);
    whole.render(registers, expected);
    for (unsigned offset = 0; offset < actual.size();) {
        const auto count = std::min<unsigned>(137, static_cast<unsigned>(actual.size()) - offset);
        chunks.render(registers, std::span<float>(actual).subspan(offset, count));
        offset += count;
    }
    EXPECT_EQ(actual, expected);
    for (const auto sample : actual) {
        EXPECT_TRUE(std::isfinite(sample));
        EXPECT_GE(sample, -1);
        EXPECT_LE(sample, 1);
    }
}

TEST(PokeySynthTest, SampleRateChangesAreValidated) {
    EXPECT_THROW(PokeySynth(0), std::invalid_argument);
    EXPECT_THROW(PokeySynth(48000, 0), std::invalid_argument);
    EXPECT_THROW(PokeySynth(48000, std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
    PokeySynth synth;
    EXPECT_FALSE(synth.setSampleRate(0));
    EXPECT_FALSE(synth.setSampleRate(1000000));
    EXPECT_EQ(synth.sampleRate(), 48000U);
    EXPECT_TRUE(synth.setSampleRate(44100));
    EXPECT_EQ(synth.sampleRate(), 44100U);
}

TEST(AudioTest, SequencedMusicProducesNonSilentSamples) {
    Audio audio;
    ASSERT_TRUE(audio.startMusic(0));
    std::array<float, 960> samples{};
    double energy = 0;
    for (unsigned frame = 0; frame < 50; ++frame) {
        audio.tick(static_cast<std::uint8_t>(frame));
        audio.render(samples);
        for (const auto sample : samples) energy += sample * sample;
    }
    EXPECT_GT(energy, 1);
}
