# Sequential session comparison

`tools/reverse/session_oracle.py` compares the complete portable `GameSession`
against a sequential driver executing the original Atari instructions. It
does not replace enemy, player, formation, wave, upgrade, projectile, collision,
score or stage routines with mocks.

## Explicit driver boundary

The original image comes from `initialized-memory.bin`, with CPU ROM/program
`$6000..$BFFF` refreshed from `loaded-memory.bin` to undo prior self-modification.
Zero page and object/path workspace `$046E..$09D5` start cleared. The first-stage
player variables and original formation positions are initialized explicitly,
then the original `$92E6` executes through its final jump to player raster
`$A01D`. This matches the portable `begin()` state rather than invoking Atari OS
boot and title/input loops.

Each tick executes:

1. Original DLI body at `$8520`, through exit `$86A0` before saved-register pops.
2. Original main loop at `$806F`, through the next frame-wait entry `$8069`.
3. Comparison against one `GameSession::step(15, false)`.

Input is neutral joystick and released fire. The console reads 7 (no buttons),
trigger registers read 1, VCOUNT reports 128, and player collision hardware reads
zero. The native session has `damageEnabled=false`; this trace deliberately does
not verify hardware collision or death/restart. The generic VCOUNT wait `$8405`
returns immediately; interrupt-enable writes have no concurrent interrupt effect.

Only final sprite XOR blitters `$9B98/$9B9C` and audio entrypoints
`$B901/$B995/$BC16` return observationally. Original descriptor geometry,
clipping, cached sprite state, stars, player movement, player raster writes,
shot selection, wave decoding, formation attack programs and main-loop control
execute. This is not a comparison of full framebuffer bytes or audio waveforms.
The entrypoint excludes the preceding VBI audio/bonus dispatcher `$84AB..$851F`;
no chance-stage bonus is reached in this first-stage profile.

Every original RANDOM read uses the same 32-bit LCG as the portable adapter:
`state = state * 1664525 + 1013904223`, initialized to `$12345678`, returning
its high byte. This deliberately tests identical explicit random inputs, not
cycle-timed POKEY noise.

## Sentinels and result

204 values are compared after **every** tick:

- VBI and gameplay counters `$55/$56`, grid phase `$5B`, message countdown,
  wave index, incoming/formation counts, next formation allocation, collision
  mask, score-chain byte, appearance/group timers and prepared-wave count.
- Enemy-shot count/cooldown, upgrade spawn delay/target phase/count/program,
  player angle/depth/aim, transition and death countdown.
- Full RNG state, all ten path cursors/phases/dynamic programs.
- All ten incoming angles/depths/sprite types/width-and-hit bytes.
- All 36 formation angles/depths/width-and-hit bytes.

The first strict run found a real integration mismatch at tick 2: the original
consumed 32 random bytes while C++ consumed 21. `$8C03..$8C10` reads eleven bytes
for the player thruster on even ticks with no transition. Omitting these visual
reads shifted gameplay randomness. The session now preserves those eleven reads
after movement, under the original death/explosion/even-tick/transition gates.

After that correction, 5,000 sequential ticks passed all 1,020,000 comparisons,
with 53,291 RNG reads. Wave-index changes were:

| Tick | Next wave index |
| --- | --- |
| 1 | 0 |
| 150 | 1 |
| 530 | 2 |
| 911 | 3 |
| 1292 | 4 |
| 1664 | 5 |

This covers all four ordinary entry waves and more than 3,300 subsequent ticks
of formation/attack/upgrade scheduling. The neutral player does not clear the
stage, so chance stages, bonus dispatch, stage advance, two-player turn switches,
shots fired by the player and the opt-in formation overflow repair are outside
this trace. Their focused tests/oracles provide separate evidence.

Run `/tmp/gyrus-re-tools/bin/python tools/reverse/session_oracle.py --ticks 5000`.
The script stops at the first mismatch and reports the tick, original addresses,
native/original values and recent random-read PCs; it does not accept a mismatch
or silently realign state.
