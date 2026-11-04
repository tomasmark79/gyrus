#include <Game/Music.hpp>
#include <gtest/gtest.h>

using gyrus::game::Music;

namespace {
void hashFrame(std::uint64_t& hash, const Music& music, std::uint8_t request) {
    const auto append = [&hash](std::uint8_t value) { hash = (hash ^ value) * 1099511628211ULL; };
    for (const auto& channel : music.channels()) {
        append(channel.frequency);
        append(channel.control);
    }
    append(static_cast<std::uint8_t>(music.isPlaying()));
    append(request);
}
std::uint8_t ownership(unsigned frame, bool masked) {
    return masked && frame % 113 < 31 ? static_cast<std::uint8_t>(1U << ((frame / 37) % 4)) : 0;
}
} // namespace

// Independent original-6502 fixtures from music_oracle.py --fixtures.
// Each frame contains channel1..4 AUDF/AUDC, music-enabled, and effect request.
TEST(MusicTest, AllEightTracksMatchOriginalThroughEndingsRepeatsAndTransitions) {
    constexpr std::array<std::array<std::uint64_t, 8>, 2> expected{{
        {{0x22922b5846d18765ULL, 0x25100ca12ee538adULL, 0x3bf675d88f7e56f9ULL,
          0xfdd256d29c14e4ceULL, 0x4afc2a9cf4fa8728ULL, 0xdff573fe2051e5c0ULL,
          0xecabea1bf217ead5ULL, 0x08fd120e0fc6737cULL}},
        {{0xc4db287c7b685fbeULL, 0x399a17c372e21ccdULL, 0x04dc43e1a57971e1ULL,
          0x661e4bd88400807dULL, 0x6e350d1e2509a01fULL, 0xb5424659d3fa5134ULL,
          0x74379f289c4b2f48ULL, 0xb39e2161c02c655cULL}},
    }};
    for (unsigned masked = 0; masked < 2; ++masked) {
        for (std::uint8_t track = 0; track < 8; ++track) {
            SCOPED_TRACE(::testing::Message() << "track=" << unsigned(track) << " masked=" << masked);
            Music music;
            ASSERT_TRUE(music.start(track));
            std::uint64_t hash = 14695981039346656037ULL;
            for (unsigned frame = 0; frame < 16384; ++frame) {
                const auto requested = music.takeEffectRequest().value_or(255);
                music.tick(ownership(frame, masked != 0));
                hashFrame(hash, music, requested);
            }
            EXPECT_EQ(hash, expected[masked][track]);
        }
    }
}

TEST(MusicTest, TrackReplacementStopAndRetainedModeMatchOriginal) {
    constexpr std::array<unsigned, 10> times{0,41,999,1500,2000,2500,6000,11000,25000,27000};
    constexpr std::array<std::uint8_t, 10> tracks{5,7,0,1,2,3,4,5,6,7};
    Music music;
    std::uint64_t hash = 14695981039346656037ULL;
    for (unsigned frame = 0; frame < 32768; ++frame) {
        for (unsigned i = 0; i < times.size(); ++i) {
            if (frame == times[i]) {
                ASSERT_TRUE(music.start(tracks[i]));
            }
        }
        const auto requested = music.takeEffectRequest().value_or(255);
        const auto mask = ownership(frame, true);
        if (frame == 2400) music.stop(mask);
        music.tick(mask);
        hashFrame(hash, music, requested);
    }
    EXPECT_EQ(hash, 0xe3d6b75685ec0743ULL);
}

TEST(MusicTest, StartRequestsOriginalEffectsAndTrackSevenRetainsMode) {
    Music music;
    ASSERT_TRUE(music.start(0));
    EXPECT_EQ(music.takeEffectRequest(), 6);
    EXPECT_FALSE(music.takeEffectRequest());
    ASSERT_TRUE(music.start(1));
    EXPECT_EQ(music.takeEffectRequest(), 4);
    ASSERT_TRUE(music.start(7));
    EXPECT_EQ(music.takeEffectRequest(), 4);
    ASSERT_TRUE(music.start(5));
    EXPECT_FALSE(music.takeEffectRequest());
    ASSERT_TRUE(music.start(7));
    EXPECT_FALSE(music.takeEffectRequest());
}

TEST(MusicTest, InvalidTrackDoesNotChangeRunningMusic) {
    Music actual, expected;
    ASSERT_TRUE(actual.start(5));
    ASSERT_TRUE(expected.start(5));
    EXPECT_FALSE(actual.start(8));
    EXPECT_FALSE(actual.start(255));
    for (unsigned frame = 0; frame < 4096; ++frame) {
        actual.tick();
        expected.tick();
        EXPECT_EQ(actual.channels(), expected.channels());
        EXPECT_EQ(actual.isPlaying(), expected.isPlaying());
    }
}

TEST(MusicTest, StoppingRetainsFrequenciesAndEffectOwnedControls) {
    Music music;
    ASSERT_TRUE(music.start(4));
    // Original track 4 begins with rests; its first channel-1 note is frame 94.
    for (unsigned frame = 0; frame < 95; ++frame) music.tick();
    const auto before = music.channels();
    ASSERT_EQ(before[0].frequency, 60);
    ASSERT_EQ(before[0].control, 0xa7);
    music.stop(1);
    EXPECT_FALSE(music.isPlaying());
    EXPECT_EQ(music.channels()[0], before[0]);
    for (unsigned channel = 1; channel < 4; ++channel) {
        EXPECT_EQ(music.channels()[channel].control, 0);
        EXPECT_EQ(music.channels()[channel].frequency, before[channel].frequency);
    }
    const auto stopped = music.channels();
    music.tick();
    EXPECT_EQ(music.channels(), stopped);
}
