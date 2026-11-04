# Player movement and projectile evidence

Source: extracted `GYRUSS.atr`, SHA-256
`dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc`.
Addresses below are original 6502 CPU addresses. Findings are static unless
explicitly described as execution comparisons. The original Atari runtime is
controlled by the parent investigation; no independent emulator session was used.

## Portable subsystem implemented and verified

`src/lib/Game/Polar.hpp` and `Polar.cpp` implement the original perspective and
shortest-path steering in ordinary C++, with no CPU emulator or runtime ROM input.
Only 1,088 bytes of original coordinate samples are embedded as data.

`tools/reverse/polar_oracle.py` builds the C++ routines and compares their outputs
against py65 1.2.0 execution of the original routines in `loaded-memory.bin`:

- `$A0CB` perspective: all 128 angles × 64 depths = 8,192 cases match x, y,
  and original carry/invalid result.
- `$A0BD` steering: all 128 current angles × 128 target angles = 16,384 cases
  match the accumulator (signed movement converted to its byte representation).
- Zero mismatches on 2026-11-04.
- Tests were written first, then executed against stub functions; the run failed
  with `original outer orbit changed`. The implemented routines pass the direct
  native regression runner with `-Wall -Wextra -Werror`.

Reproduce:

```sh
g++ -std=c++20 -Wall -Wextra -Werror -Isrc/lib tests/PolarTest.cpp \
    src/lib/Game/Polar.cpp -o /tmp/gyrus-polar-test
/tmp/gyrus-polar-test
/tmp/gyrus-re-tools/bin/python tools/reverse/polar_oracle.py
```

The temporary Python environment is investigator-local; alternatively install
`py65==1.2.0` in a local virtual environment. The game itself does not need Python.
Depths outside 0..63 were not verified: the C++ API rejects them rather than reading
outside the original table. Angle arguments normalize modulo 128.

## State and perspective

| State | Address | Meaning established by use |
| --- | --- | --- |
| Angle array | `$051E + object` | Wrapped 0..127 angle; object 0 is player |
| Depth array | `$0576 + object` | Perspective depth, increasing toward center |
| X array | `$046E + object` | Logical playfield X |
| Y array | `$04C6 + object` | Logical playfield Y |
| State/size array | `$05CE + object` | 0 inactive; bit 7 hit/pending destruction; low 7 bits collision width |
| Player index | `$92` | 0/1 active player; chooses joystick and per-player state |
| Input nibble | `$8F` | Active-low joystick direction nibble |
| Fire latch | `$90` | Pending new trigger press |
| Trigger debounce | `$ED` | 0 after release, 1 after accepted press |
| Update counter | `$55` | Incremented by display interrupt at `$8520` |
| Player transition | `$B8` | Nonzero inhibits shooting; controls inward motion |
| Player explosion | `$D8` | Nonzero immediately skips movement routine |

`$A0CB..$A113` maps angle/depth to (A=x,Y=y,C=invalid).
The playfield center is (80,48) in a nominal 160×96 logical bitmap.
Angles: 0 bottom, 32 left, 64 top, 96 right.
The 64-entry angle-fold table `$A559..$A598` and 17-word pointer table
`$A599..$A5BA` select a row in `$A119..$A558`:

```text
row = abs(16 - ((angle >> 1) & 31))
sample = row * 64 + (depth & 254)
(rawY, rawX) = perspective[sample : sample+2]
quadrant = angle >> 5
y = rawY >= 128 ? rawY : 48 + (quadrant in {1,2} ? -rawY : rawY)
x = rawX >= 128 ? rawX : 80 + (quadrant in {0,1} ? -rawX : rawX)
if rawY < 128 and unsigned_byte(y) >= 89: y = 255
invalid = x == 255 or y == 255
```

The sign table is `$A114 = {0,255,255,0,0}`: Y uses index quadrant and
X uses quadrant+1. Samples carry original perspective rounding and cannot be
replaced with a circle/sine formula without changing trajectories.

Starting depth is 12 (`$93F9..$93FE`). At that depth, angles 0,16,...112 project
to `(80,88),(43,78),(25,48),(43,18),(80,8),(117,18),(135,48),(117,78)`.
At depth 58 they are close to center, e.g. angle 0 → (80,50).

## Input and steering

Input sampler `$9A21..$9A93`:

- Reads POKEY KBCODE `$D209`; code `$21` while SKSTAT bit 2 is clear toggles
  pause flag `$93` on a debounced press (`$EE`). The precise key label is not
  established here.
- Reads PIA PORTA `$D300`; selects low nibble for player 0 and high nibble for
  player 1, storing `$8F`.
- GTIA TRIG0/1 `$D010+player`: released value 1 clears `$ED`; pressed value 0
  increments `$90` only when `$ED` was zero, then sets `$ED` to 1. Holding fire
  does not automatically retrigger in this routine.
- Active-low console START sets `$8D` through debounce `$EB`; when not in a
  game, fire also takes the START path. SELECT sets `$8E` through `$EC`.

Direction mapping at `$89E8 + joystickNibble`:

| Physical direction | Nibble | Target angle |
| --- | --- | --- |
| Up | E | 64 |
| Down | D | 0 |
| Left | B | 32 |
| Right | 7 | 96 |
| Up+left | A | 48 |
| Down+left | 9 | 16 |
| Up+right | 6 | 80 |
| Down+right | 5 | 112 |
| Released | F | no target (255) |

Impossible opposing-direction nibbles have other bytes in the table and should
not be mistaken for ordinary input behavior.

Movement routine `$8B6B` saves prior player angle in `$B2`. On even `$55`
updates, when a valid direction is present, `$A0BD` calculates:

```text
delta = (current == target) ? 0 : (((current-target)&127) < 64 ? -2 : +2)
angle = (angle + delta) & 127
```

The opposite-angle tie chooses +2. Player sprite rendering `$9C14` uses additional
pixel offsets around this projected center; the general perspective result is
not the final top-left player sprite position. Render details belong to graphics
analysis. Real-time frequency still needs observed DLI timing; do not assume the
counter is one full display frame solely from its name.

## Player shots

`$8956..$89E7` updates and spawns shots. Object slots 1..6 belong to player shots;
slots 4..6 are primary slots, slots 1..3 are paired upgraded shots.

1. Each active slot is erased/redrawn through `$9AFB` / `$9A9A`.
2. If hit bit 7 is set, clear its state. Otherwise add 2 to depth; clear it when
   resulting depth is >= `$38` (56). Thus shots travel inward at fixed depth steps,
   with screen speed determined by perspective data.
3. Consume `$90` regardless of whether firing is permitted. Firing is inhibited
   if `$AC | $B8 | $9A` is nonzero.
4. If `$D9` is zero, request sound effect 3 at `$B995`. This happens before slot
   allocation, even if all three primary slots are occupied.
5. Search free primary slots in order 6,5,4. No free primary slot means no shot.
6. Normal shot angle is player angle masked with `$7E`, depth is player depth.
7. If per-player `$94+player` is nonzero (dual-fire upgrade), primary angle is
   `(playerAngle-2)&$7E`; paired slot `primary-3` receives
   `(playerAngle+2)&$7E`, with the same depth.

Sprite descriptor pointer is `$B5E4`. `$9A9A` performs projection, picks sprite
and collision size, and stores the active state; simply setting a boolean is not
a faithful reconstruction of all slot state.

## Collision and score pointers

The main loop `$813A..$821C` compares active shots 1..6 against active positive
state objects 37..87 (`$25..$57`). With target width `w=state&127`, `half=w>>1`,
each axis accepts a target coordinate in `(shot+half-w, shot+half]`, using byte
arithmetic. It marks target state bit7 and shot bit7. If paired upgraded shot
exists, it marks the paired slot as hit too (slot±3). Subsequent logic allocates
explosion slots 12..16. This is a point-versus-square check, not general circles.

Player-hit logic `$85C5..$85F9` uses hardware GTIA collision registers
`$D004..$D006`, masks with `$F9`, and is gated by `$B8==0` and `$D9!=0`.
A hit sets `$AC=$5A` (90 update countdown), requests sound effect 0, starts player
explosion `$A01D(Y=$80)`, and hides player hardware sprites. A portable renderer
will need a corresponding software mask-overlap check, not just the shot AABB.

Score increment routine `$982F` takes A in units of 50 displayed points and uses
decimal digit arrays at `$2110..$2115` / `$2116..$211B`, appending a final zero
when rendering. It adds A/2 to the hundreds position and (A&1)*5 to the tens
position, carrying decimal digits. At a particular ten-thousands digit result
of 6, it increments player life counter `$AA`/`$AB` and redraws life markers.
Exact all-range extra-life behavior should be oracle-tested before porting.

## Remaining scope

The portable subsystem is not a playable game. Player/DLI update timing,
ship-mask collision behavior, enemy trajectories, transitions, score overflow,
and end-to-end controls still need implementation and runtime validation.

## Typed player and shot implementation

`src/lib/Game/Player.hpp/.cpp` now implement the following independent,
rendering-free part of the original game:

- `sampleInput(PlayerState&, uint8_t joystickNibble, bool triggerPressed)`:
  selected joystick nibble plus original press-edge debounce and byte fire latch.
- `updatePlayerMovement(PlayerState&, uint8_t tick, PlayerGates&)`:
  original even-tick steering and `$B8` transition-counter/depth progression,
  with `$AC` caller gate and `$D8` explosion gate.
- `updatePlayerShots(PlayerState&, const PlayerGates&) -> ShotEvents`:
  six original shot slots, depth update, hit/center removal, firing gates,
  primary allocation order, dual-shot pairing, original projection, descriptor
  width, and viewport clipping. Sound requests are returned as events.

`PlayerState::aimAngle` represents `$B2`: it first receives current angle, then
receives joystick target if a target is accepted on an even update.
Array index 0 corresponds to original object slot 1. Primary indices are 3..5.
`Shot::position` is the logical center used by the original shot renderer;
`Shot::width` comes from descriptor `$B5E4` (eight depth groups: 3,3,3,2,1,1,1,1).

The transition behavior is deliberately exact and slightly non-obvious:
on each eighth tick, nonzero `$B8` below 128 increments by 2, plus another 2 if
its old value was >=60. Player depth receives `min(oldB8,60)`. Values >=128
keep the counter unchanged and set depth 60. An explosion suppresses this
progression. Lifecycle code supplies/owns the gates; this module invents no
enemy or level progression.

The new test was executed against stubs and failed with
`new trigger press must latch fire`. The implemented native regression test
then passed with `-Wall -Wextra -Werror`.

`tools/reverse/player_oracle.py` differential result on 2026-11-04:

- 192 input cases: all 16 nibbles, pressed/released, debounce states, and
  pending counters 0/1/255 (including byte wrap).
- 7,169 movement cases: all 128 angles × 16 nibbles at ticks 1/2/8,
  all 256 transition values at ticks 7/8 with and without explosion,
  plus caller death-countdown suppression.
- 1,456 shot cases: all 128 launch angles with normal/dual fire, plus 1,200
  deterministically seeded cases mixing occupied/hit slots, depth thresholds,
  pending fire, primary capacity, sound suppression, and firing gates.
- Zero mismatches in the compared input, movement, transition, shot geometry,
  slot state, and sound-request results.

Oracle boundary: input executes `$9A3D..$9A5D` (console/pause handling outside
the typed subsystem); movement executes `$8B6B` with ship rasterization `$9C14`
intercepted; shots execute `$8956` with erase `$9AFB`, raster-writing tail `$9B98`,
and SFX request `$B995` intercepted. Shot projection, descriptor sizing and
viewport-clipping code `$9A9A..$9B97` executes unmodified. Existing shot centers
in randomized fixtures are safe interior coordinates, so intercepted erasure
does not suppress any expected clipping side effects. This oracle establishes
the bounded player/shot behavior, not complete rendering, music or game fidelity.

```sh
g++ -std=c++20 -Wall -Wextra -Werror -Isrc/lib tests/PlayerTest.cpp \
    src/lib/Game/Player.cpp src/lib/Game/Polar.cpp -o /tmp/gyrus-player-test
/tmp/gyrus-player-test
/tmp/gyrus-re-tools/bin/python tools/reverse/player_oracle.py
```
