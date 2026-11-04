# Enemy shots and the beginning of player death

Evidence: original loaded program plus initialized runtime tables extracted
from the supplied `GYRUSS.atr`. Implementation is
`src/lib/Game/EnemyShots.hpp/.cpp`. No original instructions execute at game
runtime. Source addresses below are Atari addresses, not native addresses.

## Exact scope

- `$821F..$82A6`: ordinary/forced enemy-shot allocation.
- `$831F..$8352`: independent special shot in object slot 11.
- `$8913..$8955`: movement, initial drawing, animation and clipping for slots 7..11.
- `$85C8..$85F9`: player-death gate after movement, supplied with original-style
  playfield collision bits. The caller skips this block while `$AC` is nonzero.

The actual generation of GTIA collision bits is not approximated here. Neither
the five-piece explosion drawing nor the later lives/player-turn scheduler is
implemented by this module. A death-start event tells the adapter to request
SFX 0, begin player explosion mode `$80`, and hide the player.

## State and frame order

`EnemyShots::shots[0..4]` correspond to Atari object slots 7..11. Each holds
angle, depth, sprite ID, heading, projected position, width and active/initial
draw flags. Original `$05CE+slot=$80` means pending initial drawing, not a hit.
`activeCount`, `cooldown`, `forcedShooter`, and `specialCountdown` correspond
to `$A9`, `$A7`, `$B3`, and `$B4` respectively.

The main-loop spawn blocks execute after player-shot hit scanning, with the
outer active-game/pause/death gates at `$806F..$8079`. They are distinct from
VBI movement. The special spawn block can also be reached directly at `$80F7`.
At `$8529..$852D`, nonzero ordinary cooldown decrements once per VBI before
the pause and active-game checks. The adapter must preserve this scheduling;
spawn/update functions do not implicitly decrement cooldown.

The `$8913` movement function uses gameplay counter `$56`, not VBI counter
`$55`. It runs only when the lower two bits are zero. The caller at `$8546`
also skips it during message countdown `$9A`; active-game and pause gates
occur before that call. The standalone function handles only its internal
four-tick gate.

## Ordinary and forced spawn

The ordinary scan visits source object slots 39..87 in ascending order.
The API therefore takes exactly 49 sources, including special object slots
49..51 between incoming enemies 39..48 and formation enemies 52..87.
It selects the first active, non-hit source with depth at least 26 and angle
equal to or adjacent to aim `$B2`. The implementation preserves the original
carry-sensitive SBC/ADC and masking sequence at `$824B..$8260`; it does not
substitute a floating-point angular threshold.

On finding that first eligible source, `(RANDOM & 3)` must equal zero. Failure
ends the scan, rather than trying another source. A second RANDOM byte chooses
the aim offset: masked values 0,2,4,6 produce offsets -2,0,+2,0 respectively.
The final angle is masked with `$7E`. Depth copies the source. The sprite ID
is `$26`; allocation sets active/pending initial draw, increments `$A9`, and
reloads cooldown `$A7` from `$A8`. No sound is requested in these spawn blocks.

If count is below cap and `$B3` is nonzero, it selects source slot `48+B3`
without checking source visibility, depth, alignment, ordinary cooldown or
normal-stage flag. The forced selector is cleared. The native source selector accepts
1..3 (target slots49..51); the special-group setter at `$8A1A` actually emits
only1 and2, at previous phases1 and5. Other values are explicitly rejected. This path consumes
only the random offset byte, without the probability test.

### Original cap/allocation quirk

The sequence `CMP $A6; BCS $8239; ...; $8239: BEQ $82A7` suppresses allocation
when count **equals** cap. A count greater than cap continues scanning and
bypasses ordinary cooldown/stage checks. The C++ preserves this behavior.

The original free-slot search starts at 7 and has no upper bound. If all five
shot slots are occupied, it continues into other object pools. The new API
reports `EnemyShotSpawnStatus::outsideShotPool` at that precise boundary;
it does not invent how those other subsystem slots should subsequently behave.
This is an explicitly unresolved integration case, not a normal successful
spawn. Any already-consumed forced selector/probability RNG call is preserved;
the offset RNG read lies after the unsupported boundary and is not consumed.
The special slot can independently change count, so runtime reachability of
this condition needs checking rather than assuming the cap prevents it.

At stage setup, `$9395..$93AC` derives cap from byte `$A3` as
`min(((A3+1) & 255) >> 1, 4)`, and cooldown reload is `66 - 8*cap` for this
observed cap range. The module takes these as inputs because progression owns
`$A3` and stage reset scheduling.

## Independent special shot

At countdown zero, an active slot 11 blocks timer reload; otherwise `$F6`
reloads `$B4`. A zero reload disables this spawn. A nonzero timer decrements
immediately, including on the same call that reloads it. Expiration spawns only
with transition `$B8=0` and normal-stage flag `$D9!=0`.

The resulting slot 11 angle is `playerAngle XOR 2` (no extra mask), depth is
50, sprite ID `$18`, and the pending-draw flag is set. Count increments even
if slot 11 became active after its timer started; this original replacement
behavior is preserved. The stage setup table `$9421` is `[0,255,63,31]`, indexed
by `min(A3>>2,3)`. The caller supplies that reload value.

## Movement and clipping

A pending initial draw skips movement and animation on its first eligible
update, then resolves projection and sprite width. Subsequent updates erase
the old bitmap, toggle sprite ID with XOR 2 when `tick & 4` except for slot 11,
and subtract two depth units. Negative wrapped depth removes the shot.
Projection and the original sprite-descriptor widths determine clipping;
right/bottom edges are exclusive (`left+width <160`, `top+width <96`). Removal
clears active/width and decrements the byte count, including original wrap.

Regular IDs `$24/$26` share descriptor `$AD43` with width profile
`[4,4,4,3,3,2,2,2]` across eight-depth bands. Special ID `$18` uses `$B0CE`
with `[7,7,7,6,4,3,3,3]`. These use the extracted `EnemyData.hpp` profiles.

## Player-death gate

With death countdown zero, transition zero and normal-stage flag set, the
original ORs P0PF/P1PF/P2PF (`$D004..$D006`), writes HITCLR, then tests `$F9`.
Thus collision latches clear even if the selected mask finds no hit. Neither
message countdown nor `$D8` is an extra gate here. On a hit, `$AC` becomes 90,
SFX 0 is requested, the five-piece explosion setup is called with Y=$80,
and player hardware horizontal-position registers are cleared. Explosion
setup ends by setting `$D8=$88` at `$A076`; the C++ gate retains that flag.

`resolvePlayerHit` returns whether to clear the collision latch and whether
death started. It updates the shared `PlayerGates` countdown/explosion fields.
It takes independently established collision bits; using a guessed radial
distance as their replacement would exceed this evidence.

## Verification and limits

`tools/reverse/enemy_shots_oracle.py` compiled the native implementation and
compared it with original routines in py65 1.2.0:

- 10,240 movement cases: all 128 angles, even depths across 0..63, first draw,
  plus mixed-slot randomized phases, clipping, animation and counter wrap.
- 7,680 ordinary/forced spawns: adjacent/wrapped/odd angles, RNG consumption,
  gate combinations, first eligible source, count-over-cap and full-pool boundary.
- 10,240 special-spawn cases: every countdown byte with five reload values,
  occupancy, transition and stage gates.
- 2,048 player-death gate cases: collision OR/mask and caller/internal gates.

All 30,208 comparisons matched. Six standalone GoogleTests also passed after
failing against inert stubs. Original projection/descriptor/clipping code
executes in the oracle; bitmap erase/raster writes are intercepted. The death
oracle records sound/explosion requests and substitutes only the documented
final explosion flag. These comparisons do not establish player pixel-collision
generation, explosion visuals, pool-overflow behavior or the later lives/turn
state machine.
