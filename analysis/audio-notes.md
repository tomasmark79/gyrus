# Original Gyruss audio engine

Static analysis of `loaded-memory.bin` extracted from the supplied ATR,
with a bounded CPU-level check of effect register output using py65 1.2.0.
Addresses refer to the loaded Atari image. This report does not claim an
auditory comparison or a cycle-accurate POKEY model.

## Entry points and interrupt root

`$87B8` installs `$9A94` in the deferred VBI vector `$0222/$0223`.
`$9A94` jumps to `$B9DB`, the audio frame routine. Its epilogue at `$BB18`
restores Y, X, A and executes RTI, restoring NMI enable from `$CC`.
The frame routine disables NMI on entry. A nonzero `$93` skips all audio
updates (pause-like gate; it does not silence currently sounding registers).

Public operations:

| Operation | Entry | Input |
| --- | --- | --- |
| Start music | `$B901` | A = track ID 0..7; preserves X |
| Start effect | `$B995` | Y = effect ID 0..9; preserves X |
| Start effect on explicit channel | `$B9A0` | X = channel register offset 0,2,4,6; Y = ID |
| Stop music | `$BC16` | Clears music enable and silences channels without an effect |
| Audio frame | `$B9DB` | Reads frame counter `$55`, pause gate `$93` |

The original uses the four POKEY channels directly. Writes go to AUDF1/AUDC1
at `$D200/$D201`, indexed by 0,2,4,6. Channel 4 percussion also writes
`$D206/$D207`. No audio-engine writes to AUDCTL were found in this region;
POKEY clock/divider mode must be checked in the initialized machine state.
Initialization at `$87EF/$87F3` writes SKCTL 3 then 2. Reads from `$D20A`
elsewhere are RANDOM, not audio output. `$D20A` writes in setup are SKREST.

## Portable state model

For each channel `c`, let `x=2*c`:

| Original location | Meaning |
| --- | --- |
| `$5C+x` word | Music stream cursor |
| `$6A+x` | Remaining note duration in music ticks |
| `$6B+x` | Current music AUDC envelope |
| `$72+x` | Active effect ID + 1 (zero = no effect) |
| `$73+x` | Music repeat counter |
| `$7A+x` | Effect parameter A |
| `$7B+x` | Effect parameter B |
| `$82+x` | Effect parameter C |
| `$83+x` | Current music frequency divisor |

Global `$66` enables music; `$67` is the 5-frame tick countdown;
`$68` is a section repeat counter; `$69` is the persistent music mode/ID.
For mode >=3, channel 4 uses `$70` as a four-tick percussion byte counter,
`$71` as the shifted percussion pattern, and `$62/$63` as its cursor.
These overlay channel-4 note-duration/envelope state intentionally.

Clean C++ should represent these as channel structs and explicit fields,
not as a 64KB memory machine or an interpreter. Arithmetic on frequency,
duration, and effect parameters must preserve unsigned 8-bit wrapping.

## Music selection and data

`startMusic(id)` copies eight bytes from `$B955 + id*8` to the four cursor
words. Unless id=7, it sets persistent mode=id and section repeats=2.
Thus id 7 changes pointers while retaining the previous mode. Every channel
gets envelope `$A3`, repeat counter zero, frequency zero, duration one.
Mode 0 automatically starts effect 6; mode 1 starts effect 4. The global
tick countdown and enable flag both become 5.

| ID | Channel 1 | Channel 2 | Channel 3 | Channel 4 |
| --- | --- | --- | --- | --- |
| 0 | BC35 | BC59 | BC75 | BC81 |
| 1 | BC49 | BC6D | BC77 | BC83 |
| 2 | BC89 | BC8D | BC91 | BC9B |
| 3 | BCA5 | BD0F | BD67 | BD6E |
| 4 | BD82 | BE4B | BEDB | BF4B |
| 5 | BDA3 | BE4B | BEDB | BF4B |
| 6 | BFC7 | BFC7 | BFC0 | BFAA |
| 7 | BDA5 | BE4F | BEE1 | BF50 |

The tables/data live in `$B94E..$B994` and `$BC35..$BFFF`; use the supplied
image rather than retyping music notes. Channel attack AUDCs from `$B94E`
at offsets 0,2,4,6 are A7,A7,A6,A6. Normal decay subtracts one per frame
until A3, including while an effect owns that channel. Register output is
suppressed for a channel with an effect. Music continues advancing underneath
effects.

Every fifth audio frame, decrement each note duration. When it reaches zero,
read stream tokens until a note/rest is obtained:

- Token 0 is a rest.
- Tokens >=14 are frequency divisors.
- Token 13 is an absolute jump: replace cursor with the next little-endian
  word and continue parsing.
- Tokens 1..12 call the repeat helper at `$BBF9`: if repeat counter is zero,
  initialize it from the token; otherwise decrement it. If it becomes zero,
  skip the following offset byte. Otherwise jump backwards using that byte.
  Precisely, pointer = pointer + offset - 256, with the offset read at the
  current pointer (before skipping it). This reproduces the carry-sensitive
  high-byte decrement in the original.

After a note/rest, the following byte is its duration. Zero duration is a
track terminator: mode 0 starts track 3; other modes stop music, preserving
active effects. The sequencer returns from the music processing pass at that
point. Otherwise consume the duration byte.

For a note with no effect, write AUDF and attack AUDC. In mode 6 attack is
A3 on every channel. Rest writes AUDC=0 and leaves AUDF unchanged. Both use
shadow envelope A3 for rests. In the following frames even a rest may cause
A3 and divisor zero to be written by the common output pass: preserve this
observed ordering when comparing register traces.

### Percussion channel when mode >=3

Channel 4 bypasses normal note handling. On each music tick, decrement its
counter; every fourth tick load a new pattern byte and reset counter to 4.
Each tick shifts the two top bits of pattern out, MSB first, giving 0..3.
Without a channel-4 effect, write AUDF4 = value*16 and AUDC4=84 for nonzero
values, or AUDC4=0 for zero. Zero pattern bytes begin control sequences:

- Following byte zero resets its stream pointer to BFAA and resumes parsing.
- Following byte 1..127 invokes the same repeat helper and resumes parsing.
- Following byte with bit7 set transitions music. Strip bit7. If nonzero,
  only mode5 observes it; other modes resume parsing. In mode5 shift it right;
  if nonzero decrement section repeats, and resume parsing if repeats becomes
  zero. Otherwise startMusic(value+6), with `value` the post-shift value in
  mode5. A stripped zero transitions directly to track6.

## Sound effects

Initialization tables at B9B3, B9BD, B9C7, B9D1 supply channel, A, B, C.
Each effect overwrites the chosen channel's previous effect unconditionally.
The frame routine dispatches effects after music, from channel4 down to1.
It uses an RTS jump table ($BB23/$BB2D), whose stored addresses are one byte
before the actual handlers:

| ID | Channel (1-based) | A | B | C | Handler |
| --- | --- | --- | --- | --- | --- |
| 0 | 4 | 40 | 18 | 03 | BB37 |
| 1 | 1 | 50 | 0F | 64 | BB84 |
| 2 | 3 | 68 | 00 | 00 | BB56 |
| 3 | 2 | 00 | 04 | 00 | BB76 |
| 4 | 4 | 00 | 83 | 00 | BB69 |
| 5 | 1 | 50 | 0F | 70 | BB84 |
| 6 | 4 | 8E | D0 | 37 | BB9E |
| 7 | 4 | 40 | 10 | 00 | BB3A |
| 8 | 3 | 5B | 00 | 01 | BBCB |
| 9 | 2 | 20 | 60 | 10 | BB9E |

Handler algorithms below use byte arithmetic. `emit(control, frequency)`
writes AUDC then AUDF. `finish()` clears effect-active, AUDC, AUDF. A skipped
frame retains previous POKEY register values, rather than clearing them.

- IDs 0,7: ID0 skips even frames; ID7 runs every frame. A +=32; if A>=96,
  decrement B and reset A=0. Let v=(B>>1)&15. If v=0 finish, else emit(v|A,C).
- IDs 1,5: let f=A-3. If f<80, set f=C, subtract 2 from B, and finish if
  B has its sign bit set. Store A=f; emit(B+160,f).
- ID2: frequency starts at A. On odd frames increment A; finish if the
  result has its sign bit set (reaches 80 hex), otherwise emit(A6,A).
  On even frames emit(0,A). This sign-bit completion follows the BPL at
  `$BB64`, with fallthrough to `$BB66`.
- ID3: A +=8; finish on wrap to zero, else emit(B,A).
- ID4: run only on odd frames. Increment A, finish if A=7A,
  otherwise emit(B,A).
- IDs 6,9: compute unwrapped s=A+C. If s>255 or byte(s)>=B: B-=2;
  if B<30(hex), replace with effect4 on the same channel and skip output
  this frame. Else C=(B-20(hex))>>2 and A=20(hex). Otherwise A=byte(s).
  Emit(A4,A).
- ID8: A=30(hex)-A (wrap). On every fourth frame B+=C; finish if B=0;
  if B>=11 decimal, set C=FF. Emit(A0+B,A). This gives a rising/falling
  volume envelope while alternating two divisors.

Callsite correlations (semantic names require gameplay observation):

- Music0: $83FC and $8F08.
- Music2: $84DC.
- Dynamic music from global BC: $8501; BC then cleared.
- Dynamic music from player/level state: $937D.
- Dynamic effect from global BD, after stopMusic: $84F6; BD then cleared.
- Effect0: $85E2; effect2: $8609; effect3: $899F.
- Effect5: $8ACF and $8FD6; effect7: $9012.

## Remaining verification

1. Capture POKEY writes for each track/effect in Atari800 and compare a clean
   C++ sequencer's frame-by-frame output. The code above is static evidence.
2. Confirm PAL/NTSC frame cadence, AUDCTL startup value, and actual VBI timing.
3. Render POKEY controls including distortion modes, not just sine waves.
   Pure-tone A* controls and noise/percussion controls require different
   oscillation behavior. A correct sequencer alone is not audio fidelity.
4. Associate effect and track IDs with observed gameplay events.

Root runtime observation in Atari800 at game start: AUDCTL=00, SKCTL=03,
PAL selected. Thus the startup writes of SKCTL3 then2 do not describe the
final runtime SKCTL value; later OS execution restores it to3.

## Bounded CPU reference check

Each effect was initialized by running the original `$B995` routine in py65,
with music disabled and an otherwise zeroed RAM image. Eight invocations of
the original `$B9DB` routine used frame counter values 0..7 and stopped at
`$BB18` before its interrupt epilogue. The selected channel's resulting
AUDC/AUDF pairs were:

```text
0 00/00 0b/03 0b/03 2b/03 2b/03 4b/03 4b/03 0b/03
1 ad/64 ad/61 ad/5e ad/5b ad/58 ad/55 ad/52 ab/64
2 00/68 a6/69 00/69 a6/6a 00/6a a6/6b 00/6b a6/6c
3 04/08 04/10 04/18 04/20 04/28 04/30 04/38 04/40
4 00/00 83/01 83/01 83/02 83/02 83/03 83/03 83/04
5 ad/70 ad/6d ad/6a ad/67 ad/64 ad/61 ad/5e ad/5b
6 a4/c5 a4/20 a4/4b a4/76 a4/a1 a4/cc a4/20 a4/4b
7 07/00 27/00 47/00 07/00 27/00 47/00 06/00 26/00
8 a1/d5 a1/5b a1/d5 a1/5b a2/d5 a2/5b a2/d5 a2/5b
9 a4/30 a4/40 a4/50 a4/20 a4/2f a4/3e a4/4d a4/5c
```

This verifies CPU execution and register-state evolution in that bounded
setup only. py65 does not model POKEY hardware or the Atari OS/VBI timing,
and this is not a complete game/audio runtime comparison.

## Clean C++ effect implementation

`src/lib/Game/SoundEffects.hpp/.cpp` now implements all ten effects as four
typed channel states. It does not execute original instructions or access
an emulated address space. `start(id)` preserves current register values until
the next tick, selects the original channel, and replaces that channel's
effect. Invalid IDs return false without changing state. `tick(frame)` accepts
the original wrapping byte counter; the caller controls frame cadence.

`tools/reverse/audio_oracle.py --verify` compiles a temporary probe against
the actual C++ implementation, executes the original audio routines in py65,
and compares each channel's frequency/control/active values on every frame.
All 1,024-frame traces for ten isolated effects and a 1,024-frame mixed
replacement/concurrency trace matched, totaling 11,264 compared frames.
The mixed schedule is shared explicitly with `tests/SoundEffectsTest.cpp`;
GoogleTest verifies independent full-trace FNV-1a fixtures plus start/invalid-ID
contracts. The oracle can regenerate fixtures with `--fixtures`.

Both narrowing-sweep transitions were covered: effect6 switches to effect4
on frame361 and completes on605; effect9 switches on111 and completes on355.
These frame indices assume initial frame0 and no replacement. Music sequencing,
waveform/noise generation, and audible runtime comparison are not implemented
by this module.

## Clean C++ music implementation

`src/lib/Game/Music.hpp/.cpp` implements the eight track entries, four typed
voice states, five-frame tempo, attack/decay controls, repeat/jump tokens,
packed percussion, and automatic transitions. `MusicData.hpp` preserves the
971 original score bytes at BC35..BFFF and converts the eight pointer sets to
score-relative offsets. It is generated by
`tools/reverse/music_oracle.py --print-data`. No game CPU instructions are
interpreted in the C++ implementation.

`start(track)` validates 0..7 and preserves mode for entry7. It preserves
current output registers until tick. Drain `takeEffectRequest()` immediately
after start: mode0 requests effect6, mode1 requests effect4. This includes
entry7 if its retained mode is0 or1. `tick(effectMask)` advances once per
VBI; mask bits0..3 suppress writes for effect-owned channels. For percussion,
ownership also suppresses shifting the packed pattern, matching the original.
`stop(effectMask)` disables music and clears AUDC only on unowned channels,
preserving AUDF. Pause should skip tick entirely.

The music oracle executes original start/stop routines and the music portion
of B9DB, stopping at BB03 before the effect pass. It compares every AUDF/AUDC
byte plus playing state and automatic effect request. All 294,912 frames
matched: 16,384 frames for each of eight tracks with and without changing
ownership masks, and 32,768 mixed start/replacement/stop frames. This covers
track endings, repeat/jump commands, percussion restart, entry7 continuation,
and mode transitions 0→3→6, 3→6, 4→6, and 5→6 with its entry7 continuation.
Independent fixtures are checked in `tests/MusicTest.cpp`.

### Shared-output integration boundary

The music comparison deliberately supplies effect ownership flags without
executing effects that overwrite hardware registers. The effects comparison
likewise runs without music. A final combined audio layer must preserve the
actual shared register values when an engine skips a write. Choosing outputs
using only the final `active` flag is insufficient: an effect may finish and
write zero this frame, or start on a skipped frame while the previous music
registers should persist. Use a shared register sink or per-field write masks,
then verify combined music/effect traces against the original whole routine.
This boundary is now implemented by the Audio coordinator described below.
Audible comparison with original Atari800 output remains outstanding.

## Shared-register coordinator and PCM synthesis

`Audio.hpp/.cpp` combines Music and SoundEffects using explicit AUDF/AUDC
write masks. It runs music before effects, matching the original VBI order.
Writes made by finishing effects are applied even though the effect becomes
inactive. Skipped updates and start commands preserve actual shared register
contents. External stopMusic clears only unowned AUDCs. Automatic effect
requests from music start are applied immediately.

`combined_audio_oracle.py --verify` compares the entire original VBI audio
routine with this coordinator for 131,072 frames: eight tracks, each lasting
16,384 frames, with changing simultaneous/replacing effects, stop/restart,
and paused frames. Every register byte, effect ownership mask, and music
playing flag matched. Existing isolated music and effect APIs retain their
previous behavior; `registerWrites()` adds per-field write information.

`Audio::render(std::span<float>)` produces mono PCM for a native or browser
audio queue. Default rate is 48 kHz; valid rates are 8 kHz..192 kHz. At PAL cadence,
tick once then render 960 samples per 50 Hz game frame. PCM rendering preserves
phase across arbitrary queue chunk boundaries. The renderer is separate from
game ticks, and neither method performs SDL/browser calls or allocates memory.
Call these operations from one thread, or synchronize externally.

### Synthesis reference and fidelity

The implementation follows [Altirra Hardware Reference Manual, 2026-01-02,
sections 5.3–5.5](https://www.virtualdub.org/downloads/Altirra%20Hardware%20Reference%20Manual.pdf):
AUDCTL 0 uses four unlinked timers at machine-clock/28; a pure tone toggles
on each timer expiration. All channels share 4-, 5-, 17-bit XNOR noise generators
running at machine rate, with one-clock delays between channels. AUDC selects
noise/toggle, poly5 gating, and volume-only override. Tests check documented
short-polynomial sequences, long-polynomial samples, channel delays, divisor
frequency, and volume-only behavior. Nominal PAL clock is 1,773,450 Hz.

The model deliberately covers the observed AUDCTL 0 configuration. Linked
timers, high-pass channel coupling, 9-bit noise selection, serial/two-tone modes,
and timer IRQ behavior are not implemented. Frame-level register updates do
not reproduce original instruction-cycle write timing or startup phase.
Output uses linear channel mixing, machine-cycle box averaging, 20 Hz DC removal,
and 0.8 gain; it is not a measured model of Atari analogue circuitry or a
high-order bandlimited resampler. Do not label this cycle-exact synthesis.

Standalone validation passed all 9 Audio/PokeySynth tests, including 131,072-frame
coordinator fixtures and sample determinism across different chunk sizes.
Music-generated PCM is nonzero, finite, and bounded; actual device/browser
playback and listening comparison remain application-level verification.
