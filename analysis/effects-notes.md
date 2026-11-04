# Original player explosion

`PlayerExplosion` reconstructs `$A01D` with mode Y=$80 and the player-death
branch of `$8CC4` (AC nonzero). Five original object slots29..33 carry
byte-wrapped x/y coordinates and signed-byte velocities from the40 data
bytes at `$A095..$A0BC`. Orientation rounding, mirrored facing and reverse
initialization order match the original. The runtime stores only typed
particle state and those compact vectors; it executes no original CPU code.

Initialization sets D8=$88 and leaves particles invisible. Eligible frames
have `(frame & 7)==0`. First update decrements the count, skips erasure,
moves and XOR-draws each particle. Later updates erase the old positions,
move and draw; update8 erases without moving or drawing. Native advance
retains the same post-update positions, and draw reconstructs them into a
fresh framebuffer. Five kind0,size3,orientation0 sprites use the original
unsigned x<157,y<93 clipping test, including byte wrap. Calling draw twice
on a persistent framebuffer XOR-erases; the intended session contract is
advance once, then draw once while constructing the current frame.

`tools/reverse/effects_oracle.py` executes original initialization and XOR
updates through py65. All128 angles at positions(80,48),(2,2),(155,91) for
64 frames produce384 explosions and24,576 full bitmap/state comparisons,
aggregate FNV330969541. This compares original persistent erasure/drawing
against independently reconstructed C++ fresh frames, including overlapping
particles, clipping, mirrored directions and expiration.

```
/tmp/gyrus-re-tools/bin/python tools/reverse/effects_oracle.py
```

The AC=0 effect branch uses a different cadence and can restore the player;
it is outside this death-only class. `$8956` is the projectile update/spawn
routine, not a general explosion animator. This module does not change
life counts, audio, stage state, death timer or input gating.

## Score popups

`ScorePopups` reproduces allocation at `$8C91` and updates at `$8C21`,
using three slots17..19 and paired suffix objects20..22. It accepts labels
0..7, corresponding to original sprite kinds21..28, plus kind20 suffix
eight pixels to the right. Depth is clamped to18; input depth must be<64.
The allocator adds4 to angle and clears its low bit. Incoming carry can
affect the first slot when input angle is odd; later slot searches clear
carry. The optional carry argument preserves this CPU-level input explicitly.

Status starts at158;31 calls later it becomes visible. A120-call lifetime
starts at that transition. An important original quirk: displaying or
expiring any one slot returns immediately through a tail-call to the suffix
blitter, so subsequent slots do not advance on that call. Empty slots retain
old coordinates/timer until reuse, matching the original object state.

The offline oracle covers24 sequences of320 frames (7680frames), all eight
labels, odd angles and both input carry values, allocation exhaustion,
staggered starts, timer starvation and reuse. The original persistent XOR
framebuffer and normalized states match fresh native reconstruction with
FNV3681590433. Native draw reproduces the suffix even if polar projection
failed; malformed/out-of-range original object data outside the accepted
depth/label domain is not a supported API input.

## Ordinary hit explosions

`HitExplosions` reproduces allocation from `$81B3..$8207` and updater `$86A9`.
Five slots12..16 update only when `(frame & 3)==2`. A newly allocated object
starts invisible; its first eligible tick sets timer10 and draws kind14.
Subsequent ticks erase the previous image, cycle kinds14/15/16, decrement
the timer and project/draw. At timer0 it disappears. Movement occurs before
animation on old timer values whose low two bits equal1. Radial motion is
clamped by rejecting results below20; angular movement keeps angles even.

The22 motion bytes at `$B8EB..$B900` are indexed by enemy kind minus3, with
fallback index4. This apparent off-by-one is intentional: original LSR of
the even descriptor clears carry, so the following SBC2 subtracts3.
Each spawn takes the enemy sprite kind, not its doubled descriptor byte.
Sprite clipping can terminate an effect before its timer expires.
Depth inputs are limited to0..63. A radial update that leaves this domain
terminates the native effect instead of indexing beyond the perspective
table; ordinary game-depth fixtures through58 never take this guard.

The oracle executes the actual original allocation path with its stack
operations, then compares24 sequences/2400 full frames and typed states.
Coverage includes every motion-table entry, clipping near the outer edge,
five-slot exhaustion and reuse. Aggregate FNV410647429 matches native.

```
/tmp/gyrus-re-tools/bin/python tools/reverse/effects_oracle.py --popups
/tmp/gyrus-re-tools/bin/python tools/reverse/effects_oracle.py --hits
```
