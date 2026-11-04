# Three-object hazards

`Hazards.cpp` translates spawn `$8353..$83AF` and update `$8876..$8910`.
`Hazards::actors[0..2]` are original objects 36..38, with center first.
`direction`, `cooldown`, and `count` are `$B5`, `$B6`, and `$B7`.

## Session integration

- Call `spawnHazards` in the main-loop portion. It decrements cooldown first,
  even when another internal condition blocks spawning. An existing cooldown
  of one permits the spawn checks on the same call.
- Spawn requires zero hazard count, message countdown and transition, a normal
  stage, and level at least five. It then consumes RANDOM until a gate fails:
  first byte exactly zero, second byte with low three bits zero, third byte
  masked with `$70` chooses initial angle. Random reads are injectable and
  their number is part of the oracle comparison.
- Spawn assigns depths 50, sprite types 20/18/26, direction 2 and count 255.
  It does not draw objects or clear previous object fields. The negative count
  is a pending initialization state consumed on the next tick divisible by 16.
- Call `updateHazards` under the original VBI active-game/unpaused/message-zero
  gates (`$8546..$8550`). Do not gate it on death countdown; this update runs
  before the original death gate. Pass VBI `$55`, not the gameplay `$56` counter.
- Outer objects update every 16 ticks, moving outward by depth minus two.
  The center follows their depth and moves two angular units every tick,
  reversing direction at the outer angles. Initial negative count becomes 3.
- Feed objects 37 and 38 into the first two entries of `CollisionTarget[51]`.
  Object 36 is below the original player-shot collision scan and cannot be hit
  by that scan. It is still a rendered playfield object.
- A hit on either outer object emits `scoreUnits=2` (100 displayed points),
  removes it and marks the visible center hit. Center removal scores nothing.
  Projection failure or negative depth removes the relevant object without
  points. Each removal decrements count and sets cooldown to 180.
- Return events use the existing `EnemyEvents` convention. These routines
  emit no sound, bonus popup or palette writes. Colors come from the original
  sprite pixels under the current playfield palette; F9 is neither read nor
  written here. The surrounding player/playfield collision gate still uses F9.
- Stage completion checks count `$B7`. Stage initialization clears it and all
  object visibility, but preserves direction and cooldown. The StageSetup
  field currently named `pickupCounter` is this same `$B7` count.

The implementation preserves renderer geometry and saved draw snapshots through
`projectEnemyActor`. It explicitly rejects a center movement direction outside
the reachable original indices 1/2 rather than reading unrelated code bytes.

## Verification

The focused test first failed against empty spawn/update implementations, then
passed after translation. It covers deferred first drawing, outer hit score,
center removal, the remaining outer object, and cooldown behavior.

`tools/reverse/hazards_oracle.py` executes the original code using py65. Only
final XOR raster writes `$9B98` and score application `$982F` are intercepted;
all original projection, sprite descriptor selection, clipping, cached geometry
and state changes execute. Score arguments are captured and compared.

408 spawn attempts cover cooldowns, internal gates, RNG short-circuit read
counts and all eight spawn angles. 32 continuous traces (all spawn angles,
no hit or a hit on each object) compare 16,384 updates, including complete
outward travel and expiration. Compared values include B5/B6/B7, score, all
angles/depths/types/headings, width/hit state and projected positions. Result:
zero mismatches. Tests do not claim raster or hardware player collision parity.

Run `/tmp/gyrus-re-tools/bin/python tools/reverse/hazards_oracle.py`.
