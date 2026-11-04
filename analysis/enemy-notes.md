# Original enemy wave and entry-path reconstruction

Source: `GYRUSS.atr`, SHA-256
`dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc`.
All addresses are original CPU addresses. This subsystem uses typed C++ state,
original data arrays, and decoded algorithms; the shipped game does not execute
6502 instructions. py65 is used only by the developer differential oracle.

## Implemented API and original boundaries

- `EnemyPaths` owns two 128-point entry tracks, ten staggered path cursors and
  phase bytes, wave counters, timing, and the mutable wave descriptor copy.
- `prepareWave(paths, settings)` reconstructs `$9634..$9807` in C++.
- `EnemyState` owns ten incoming actors (original object slots 39..48), 36
  formation actors (slots 52..87), and shared counts.
- `updateIncomingLane(paths, enemies, parity, inputs)` reconstructs the initial
  wave consumer entered by `$8F96` (even) or `$8F8E` (odd), through `$922E`, with
  formation arrival helper `$922F`. It handles staggered starts, decoded path
  points, per-enemy dive markers, player-seeking dives, retreat, clipping,
  formation arrival, hits, and original score/sound/bonus requests.
- `updateEnemyEntries` reconstructs the appearance delay and two-lane scheduling
  at `$85FF..$8617`. Host code calls it once per unpaused gameplay update after
  applying outer gameplay gates.
- `EnemyEvents` returns score in original units of 50 displayed points, sound
  IDs, and bonus-label requests. The score and sound systems apply those events.

`EnemyActor::spriteType` is the original even byte index (0,2,4,...), not a
zero-based graphics kind. For `spriteImage`/`xorSprite`, use `spriteType/2`.
`heading` is the original 0..15 orientation; some sprite descriptors derive it
from angle and overwrite the path-provided heading, exactly as `$9A9A` does.
Actors preserve their last coordinates after hiding, as original RAM does.

`EnemyInputs` supplies original external state: player angle, death countdown,
level number, normal/challenge-stage flag (`$D9`), formation-return flag (`$E6`),
and a random-byte callback for the original clipping decision when `$E6!=0`.
No RNG is silently invented inside the subsystem.

## Wave preparation

Data retained in `EnemyData.hpp`:

- 48 four-byte descriptors at `$945F..$951E`.
- Eight initial depth values at `$951F..$9526`.
- Seventeen path pointers at `$9527..$9548`, represented as offsets into
  the compact commands from `$9549..$9633`.
- Sprite width/orientation metadata reached through the table at `$AC95`.

For ordinary wave indices 0..3, descriptor index is `(stageOffset>>1)|waveIndex`.
Stage offset is one of 0,8,16,24,32,40. Wave index 4 uses an externally supplied
accepted random index 0..23: the original repeatedly reads `RANDOM&31` until it
is below 24. Challenge stages add 24 to the descriptor index.

Descriptor bytes specify two initial angles and optional ninth/tenth enemies,
initial depth and mirror flags, and a compact path ID plus entry staggering.
Every command has a high-nibble operation and low-nibble repetition count.
Operations 0..7 apply original depth/angle deltas and derive sprite/orientation.
Operations >=8 create a single per-enemy marker without changing coordinates.
The zero byte terminates the program. Prepared point counts range from 47 to
115; a style byte 255 is appended at the terminal index.

First normal wave, stage offset 0: nine enemies, 54 path points, staggered cursors
255,253,...237. Track 0 begins angle 48/depth 50/style 10; track 1 begins
angle 80/depth 50/style 10. The appearance delay is 60 updates. Entry cadence is
3 except stage offset 40, where it is 2. Odd group starts half a cadence earlier.

### Descriptor writes matter

At `$9683` and `$9696`, original `LSR abs,X` instructions modify the first two
descriptor bytes while extracting bit zero. This is observable when the image
resides in writable RAM, as the supplied ATR loader places it. On a physical
read-only cartridge, those writes would be ignored. `WaveSettings` explicitly
selects `writableDescriptors` (default true for ATR); the decoder and oracle
preserve both cases. Repeated wave preparations must reuse the `EnemyPaths`
instance if writable descriptor behavior is intended.

## Incoming actors and formation

Ten incoming actors correspond to indices X=0..9 used by the original consumer,
which accesses object arrays at object index X+39. Updates process indices in
descending order, alternating parity: 8,6,4,2,0 or 9,7,5,3,1.

- `$1A+X` is the path cursor; high bit means the initial stagger delay.
- `$24+X` is phase: 0 inactive, high bit follows the entry track, bit 6 dives
  toward the player, otherwise the byte is formation-return depth.
- `$2E+X` is the allocated formation slot (1..36).
- Path buffers: depth `$06D6/$0756`, angle `$07D6/$0856`, packed style
  `$08D6/$0956`, for even/odd groups respectively.

Marker commands with low style nibble zero are ignored by other actor indices.
When the high nibble equals the current actor index, the actor begins an outward
player-seeking dive. Below level 7, depth/angle updates happen on alternate lane
updates; level 7 and later update every call. Steering uses the verified polar
shortest-path function. Normal entries reaching their end return inward and
turn toward an allocated formation angle; challenge entries disappear.

Formation initialization is backed by the parent's actual Atari800
`analysis/game-start-memory.bin` capture after the original start transition:
angles `[116,106,96,84,74,64,52,42,32,20,10,0]` repeated three times, with depths
52,48,44 for the three groups. Formation descriptors use original sprite type22.
The API allows the host to update these angles/depths as the original formation
oscillation is ported; it does not replace that oscillation with invented motion.

Projection, descriptor size, orientation overrides and viewport rejection match
the original graphics routine. Bitmap XOR writes are handled separately by the
graphics subsystem.

## Differential evidence

`tools/reverse/enemy_oracle.py` compiles a temporary native C++ comparison runner
and executes original routines with py65 1.2.0.

Preparation verification covers 576 calls: all fixed descriptors, all accepted
random-wave descriptors, both normal/challenge families, writable/read-only
profiles, and three repeated preparations per scenario. It compares all six
128-byte path arrays, ten cursor/phase pairs, descriptor mutations, and counters.
No code interception is needed for preparation. Zero mismatches.

Consumer verification covers normal/challenge stages at offsets0/16/40, all
four initial wave indices, uninterrupted motion, injected death retreat, and
periodic injected hits at levels1/9. Each scenario runs 260 group updates.
Comparisons include every incoming and formation actor, path phase/cursors,
formation slots, counts, timers, score requests, sound requests and bonus labels.
Additional scenarios compare the caller's scheduling block `$85FF..$8617`.
In total, 19,500 consumer/scheduling updates passed with zero mismatches on
2026-11-04.

Oracle interceptions are limited to old-sprite erase `$9AF8`, final bitmap raster
tail `$9B98`, score application `$982F`, sound request `$B995`, and bonus popup
allocation `$8C91`. Score, sound and popup requests are recorded and compared.
The original projection/size/clipping executes unmodified. Input scenes start
from original initialized memory with controlled actor state, so hardware video
timing and integrated collision behavior are outside this comparison.

The native regression test first failed against stubs, then passed after each
implemented stage. Commands:

```sh
g++ -std=c++20 -Wall -Wextra -Werror -Isrc/lib tests/EnemiesTest.cpp \
    src/lib/Game/Enemies.cpp src/lib/Game/Polar.cpp -o /tmp/gyrus-enemy-test
/tmp/gyrus-enemy-test
/tmp/gyrus-re-tools/bin/python tools/reverse/enemy_oracle.py
```

## Explicit remaining enemy scope

This is original wave preparation and initial-wave actor behavior, not the entire
enemy game. Formation oscillation/re-launch and dynamic attack programs are now
implemented and separately verified in [formation-notes.md](formation-notes.md).
Remaining work includes enemy projectile slots7..11 (`$8913`), special objects
(`$8876` and `$89F8`), integrated collision events, and full stage completion.
These paths must be ported from evidence; the current subsystem does not
substitute approximated enemies for them.
