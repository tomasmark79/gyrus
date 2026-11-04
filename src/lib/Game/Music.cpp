#include <Game/Music.hpp>
#include <Game/MusicData.hpp>
#include <cassert>
#include <utility>

namespace gyrus::game {
namespace {
constexpr std::array<std::uint8_t, 4> attackControls{0xa7, 0xa7, 0xa6, 0xa6};
constexpr std::uint8_t sustainControl = 0xa3;

std::uint8_t dataByte(std::uint16_t offset) noexcept {
    assert(offset < detail::musicData.size());
    return detail::musicData[offset];
}

bool owned(std::uint8_t mask, unsigned channel) noexcept {
    return (mask & (1U << channel)) != 0;
}
} // namespace

bool Music::start(std::uint8_t track) noexcept {
    if (track >= detail::trackStarts.size()) return false;
    // $B901: entry 7 installs a continuation while retaining the current mode.
    if (track != 7) {
        mode_ = track;
        sectionRepeats_ = 2;
    }
    for (unsigned channel = 0; channel < voices_.size(); ++channel) {
        voices_[channel] = {detail::trackStarts[track][channel], 1, sustainControl, 0, 0};
    }
    percussionCountdown_ = 1;
    percussionPattern_ = sustainControl;
    tickCountdown_ = 5;
    playing_ = true;
    effectRequest_ = mode_ == 0 ? std::optional<std::uint8_t>(6)
                     : mode_ == 1 ? std::optional<std::uint8_t>(4) : std::nullopt;
    return true;
}

void Music::stop(std::uint8_t effectMask) noexcept {
    // $BC16 changes AUDC only; the previous frequency divisors remain stored.
    playing_ = false;
    for (unsigned channel = 0; channel < channels_.size(); ++channel) {
        if (!owned(effectMask, channel)) {
            channels_[channel].control = 0;
            writes_[channel] |= 2;
        }
    }
}

void Music::repeat(Voice& voice, std::uint8_t count) noexcept {
    // $BBF9: repeat token followed by a negative displacement encoded as a byte.
    if (voice.repeat == 0) {
        voice.repeat = count;
    } else if (--voice.repeat == 0) {
        ++voice.cursor;
        return;
    }
    voice.cursor = static_cast<std::uint16_t>(voice.cursor + dataByte(voice.cursor) - 256);
}

bool Music::advanceNote(unsigned channel, std::uint8_t effectMask) noexcept {
    auto& voice = voices_[channel];
    if (--voice.duration != 0) return true;
    std::uint8_t note;
    for (;;) {
        note = dataByte(voice.cursor++);
        if (note == 0 || note >= 14) break;
        if (note == 13) {
            // The original score stores absolute little-endian pointers.
            const auto target = static_cast<std::uint16_t>(dataByte(voice.cursor)
                | (static_cast<unsigned>(dataByte(voice.cursor + 1)) << 8));
            voice.cursor = static_cast<std::uint16_t>(target - detail::musicDataBase);
        } else {
            repeat(voice, note);
        }
    }
    voice.frequency = note;
    if (!owned(effectMask, channel)) {
        // $BAC0: a rest leaves AUDF untouched. Masked notes do not reset the envelope.
        if (note != 0) {
            channels_[channel].frequency = note;
            writes_[channel] |= 1;
        }
        voice.envelope = note == 0 || mode_ == 6 ? sustainControl : attackControls[channel];
        channels_[channel].control = note == 0 ? 0 : voice.envelope;
        writes_[channel] |= 2;
    }
    const auto duration = dataByte(voice.cursor);
    if (duration == 0) {
        if (mode_ == 0) start(3);
        else stop(effectMask);
        return false; // Original returns from the entire music pass at this point.
    }
    voice.duration = duration;
    ++voice.cursor;
    return true;
}

void Music::advancePercussion(std::uint8_t effectMask) noexcept {
    // $BA29: channel 4 replaces the note stream with packed two-bit percussion.
    for (;;) {
        if (--percussionCountdown_ != 0) break;
        percussionCountdown_ = 4;
        bool restarted = false;
        for (;;) {
            auto& voice = voices_[3];
            percussionPattern_ = dataByte(voice.cursor++);
            if (percussionPattern_ != 0) break;
            auto command = dataByte(voice.cursor++);
            if (command == 0) {
                voice.cursor = 0xbfaa - detail::musicDataBase;
            } else if ((command & 0x80) == 0) {
                repeat(voice, command);
            } else {
                command &= 0x7f;
                if (command != 0) {
                    if (mode_ != 5) continue;
                    command >>= 1;
                    if (command != 0 && --sectionRepeats_ == 0) continue;
                }
                start(static_cast<std::uint8_t>(command + 6));
                restarted = true;
                break;
            }
        }
        if (!restarted) break;
        // A track transition jumps back to the channel-4 tick in the same frame.
    }
    if (!owned(effectMask, 3)) {
        const auto percussion = static_cast<std::uint8_t>(percussionPattern_ >> 6);
        percussionPattern_ = static_cast<std::uint8_t>(percussionPattern_ << 2);
        if (percussion != 0) {
            channels_[3].frequency = static_cast<std::uint8_t>(percussion << 4);
            writes_[3] |= 1;
        }
        channels_[3].control = percussion == 0 ? 0 : 0x84;
        writes_[3] |= 2;
    }
}

void Music::tick(std::uint8_t effectMask) noexcept {
    writes_.fill(0);
    if (!playing_) return;
    // $B9EE: refresh sustaining notes before the slower five-frame score tick.
    for (int index = 3; index >= 0; --index) {
        const auto channel = static_cast<unsigned>(index);
        if (channel == 3 && mode_ >= 3) {
            if (!owned(effectMask, channel)) {
                channels_[channel].control = 0;
                writes_[channel] |= 2;
            }
            continue;
        }
        auto& voice = voices_[channel];
        if (voice.envelope != sustainControl) --voice.envelope;
        if (!owned(effectMask, channel)) {
            channels_[channel] = {voice.frequency, voice.envelope};
            writes_[channel] |= 3;
        }
    }
    if (--tickCountdown_ != 0) return;
    tickCountdown_ = 5;
    int firstChannel = 3;
    if (mode_ >= 3) {
        advancePercussion(effectMask);
        firstChannel = 2;
    }
    for (int channel = firstChannel; channel >= 0; --channel) {
        if (!advanceNote(static_cast<unsigned>(channel), effectMask)) return;
    }
}

std::optional<std::uint8_t> Music::takeEffectRequest() noexcept {
    return std::exchange(effectRequest_, std::nullopt);
}
} // namespace gyrus::game
