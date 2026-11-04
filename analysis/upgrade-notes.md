# Special targets and dual-shot upgrade

Source: supplied `GYRUSS.atr`, program loaded at $8000, initialized projection
snapshot `analysis/initialized-memory.bin`. Native implementation:
`src/lib/Game/Upgrade.hpp/.cpp`; differential oracle:
`tools/reverse/upgrade_oracle.py` (py65 1.2.0).

## Verified entry points and state

- $82A7..$831D: special-group spawn. Requires active player's $AD == 0 and
  return-to-formation flag $E6. Fewer than three formation enemies ($96)
  abandons the group ($B1=0, $AD=$FF). Counts 3..5 spawn immediately;
  counts >=6 require ($55 & $0C)==0 and decrement-to-zero of $AF.
- Spawn sets target slots49,51 type4 and middle slot50 type$10 if player's
  $94 is zero, otherwise type4. Angles are player+12, player, player-12;
  depth50. It also initializes six particle angles/depths using $AC7D/$AC83,
  headings0, sets $DB=$8C and increments $AD. Raster flags are retained.
  Particle sprite type$22 comes from game initialization, not this spawn.
- $89F8..$8B6A: target group, every eighth $55 tick, gated by $B1 and $AD.
  Previous phases1 and5 request forced shooter1 and2 in $B3. Previous phase7
  initializes all three targets (depth28, heading1), score chain $B0=0,
  animation $BB=$19. Later calls process hit flags then movement and drawing.
- Normal movement alternates radial ±2 with angular motion determined by the
  incremented $AD. Angular magnitudes by global $A3 rank are 6,2,4,6,8
  (8 for all ranks >=4). Death countdown $AC or phase high bit forces radial
  -4. Animation rotates $BB left twice and uses its low two bits for heading.
- Every hit gives ten 50-point score units. Type$10 gives sound9 and bonus
  label0. Ordinary targets additionally give old $B0 plus10 score units,
  increment $B0 by10, request sound5, and label(old $B0 >>3)+1.
- Middle-target hit when $94==0 activates two collectible particles, even
  if its type was not $10. Slots25..28 become depth0; slots23/24 use player
  depth and heading1; $DB=$C2. Original inherited carry means angles are
  `(player+13)&$7E` and `(first-25)&$7E` for player angles0..127: for even
  angles this is player+12 and player-14. The asymmetry is intentional here.
- $870C..$8789: particles every fourth tick, or every second tick when $DB
  bit6 is set. Slot28's descriptor is loaded once for all six. Draw precedes
  movement. Angle deltas from $AC89 are +2,-2,+1,-1,0,0 (doubled in collection
  mode), then mask$7E; depth deltas from $AC8F are 0,0,0,0,-1,+1.
  Bit7 suppresses initial erase. Regular countdown1 erases without moving.
  Collection examines slots23..25 after movement and accepts matching angular
  buckets: `(particleAngle XOR playerAngle)&$F8 == 0`; no distance test.
  Each collection clears depth and decrements $DB. Low two bits becoming zero
  increments active player's $94 as a byte and clears $DB.

## Integration contract

`UpgradeState` owns six particles and three targets plus exact byte counters.
Use `spawnUpgrade` from the active main-loop gate, `updateUpgradeTargets` from
VBI, and `updateUpgradeParticles` with the original pause/message gates.
`UpgradeInputs.tick` is $55 (not shot movement's $56). Feed $B3 to
`EnemyShots::forcedShooter` and copy its consumed value back. Target active/hit
state maps to the existing `EnemyActor` representation. Target events use the
existing `EnemyEvents` score units, raw polar bonus coordinates and SFX IDs.
Synchronize `dualShotLevel` with the active player's byte; it is not a bool.

Particle `drawn` is separate from retained width metadata: final erase and
collection do not clear the original width flag. Render the particle's stored
projected position while `drawn` is true, not its already advanced angle/depth.
Do not render erased particles merely because `actor.visible` is retained.

## Verification boundary

Six focused GTests cover spawn, abandonment, warmup/forced shots, middle hit,
collection and final erase. The differential test covers 15,000 isolated
states plus two consecutive 512-frame scenarios (2,050 calls including spawn),
comparing counters, all nine object records, raster visibility, scores, bonus
requests and sounds. Projection, sprite profiles and clipping execute in the
original CPU. Bitmap erase/raster writes and external score/bonus/sound sinks
are intercepted. Valid target depths, fixed game sprite descriptors and even
particle/target angles are used; spawn also tests odd player angles. This
proves the translated routines, not generation of hardware collision latches
or a full-stage screenshot. Pool ownership, scheduler initialization and the
main-loop gates remain caller responsibilities.

## Draw-state snapshots

`EnemyActor.renderedDepth`, `renderedSpriteType`, and `renderedHeading` record
only successful draws, alongside `position`. Renderers must select sprite
pixels from those fields: particles draw first and then modify logical depth;
formation seek-player code ($9274) also decrements logical depth after drawing.
The particle snapshot records the slot28 shared descriptor, even if the local
particle's logical sprite type differs. No logical counters or object state
were changed to repair rendering. Seven Upgrade GTests now pass, including a
31-to-32 depth transition whose rendered depth must remain31.

`tools/reverse/render_snapshot_oracle.py` executes the original raster without
interception and compares all 15,360 indexed pixels plus resulting logical
depth against native snapshot rendering. Its 4,032 cases cover both radial
particles around every eight-depth bucket boundary and formation seek-player
programs across four enemy sprite types, 16 angles and three headings. All
cases match. This extends the earlier state oracle, whose bitmap writes were
intercepted; full scene composition and hardware player-missile overlays remain
outside this focused framebuffer check.
