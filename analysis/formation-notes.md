# Formation attacks, steering programs, and grid reconstruction

Source: `GYRUSS.atr`, SHA-256
`dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc`.
Implementation: `src/lib/Game/Formation.hpp/.cpp`. It uses the typed actors and
path tracks from `Enemies`, without executing original machine code at runtime.

## API and update order

- `formationControl(levelNumber)` selects difficulty from original `$9419` and
  `$941D`: random-turn masks31/15/15/7, launch masks127/63/31/15, and active
  attack caps2/3/4/4 for level groups of four. It creates fresh control state.
- `attemptFormationLaunch(paths, enemies, control, inputs)` implements `$840B`.
  It returns the number of actors launched. `inputs.returnToFormation` is the
  original `$E6` flag, and must be enabled by stage lifecycle before attacks.
- `updateFormationProgram(...)` implements `$9199..$92C9`, including the
  indirect branches at `$9274`, `$92B3`, `$92B7`, and `$92C0`.
- `updateEnemyEntriesWithFormation` wraps the original entry scheduler.
  `updateIncomingLaneWithFormation` wraps one group update for direct use/testing.
  Both execute the dynamic program immediately after each successful actor
  projection, before moving to the next actor, matching original control flow.
- `updateFormationGrid(enemies, control)` implements `$8620..$869E`, including
  formation movement, hit removal and score requests.

Host ordering within an active unpaused gameplay update is: launch attempts,
other player/shot work, incoming scheduler with formation programs, then grid.
Outer original pause/message/transition gates remain the host's responsibility.
Keep `FormationControl` and `EnemyState` between waves. In particular, original
grid clocks `$56/$5B` are not reset by wave preparation; `$5B` is reset on new
game at `$8E97`.

`EnemyInputs.randomByte` supplies one byte per original RANDOM read. Its ordering
is tested, not replaced by an internal approximate random timer. Launch reads
one random byte before checking the death flag or incoming-count cap. Dynamic
program selection may read up to three bytes in one update.

## Launch behavior

The original launch requires `$E6!=0`, `(RANDOM & launchMask)==0`, no death
countdown, and incoming count unequal to the configured cap. It scans formation
slots in order and chooses the first visible, non-hit actor. It scans incoming
slots for the first unused object, transfers counts, and remembers the source
formation slot. The outgoing actor starts at depth38, then the seek setup
immediately decrements it to36 and creates its short steering program.

If the resulting incoming count is odd, launch repeats without another random
gate, subject to cap/availability. This normally launches pairs, not an arbitrary
random number of actors. Vacated home slots are retained for eventual return.

## Dynamic programs

The ten `$DC..$E5` bytes are represented by `FormationControl::attackState`.
Each actor owns one short instruction slot in the shared path buffers:
track parity = actorIndex&1, index = (actorIndex>>1)+1. The routine resets the
actor cursor to that slot on each applicable call.

- State0: no dynamic action.
- State1: on depth16, transitions to state2; a straight sprite10 instead
  clears the attack state.
- State2: writes the next inward step (depth+2, angle delta derived from
  sprite8/10/12), until depth30 selects another program.
- States3/4: adapt the next angular step, or select another program when
  `RANDOM & randomTurnMask` is zero.

Program selection normally chooses a player-seeking outward segment. If a
random low nibble is zero, another read selects among direct dives, a one-shot
dive, and return to formation. The `$92C0` return branch is not present in a
direct-call-only disassembly: it is reached through the indirect jump table
at `$92D6`, and has been included in this reconstruction and oracle.

## Grid movement and hits

The old `$56` value determines whether an update runs: only values divisible by
16 update the grid; `$56` increments on every call, with byte wrap. `$5B`
increments on each grid update. Odd phases add2 modulo128 to every formation
angle; even phases toggle depth bit1. Inactive slots move too, preserving the
original home positions for returning attackers.

An active hit actor decrements formation count, requests one original score unit
(50 displayed points), and clears its active/hit state. Other active actors
recompute projection/descriptor size/orientation. Thus static formation targets
are handled here, not by `$870C` (a different special-object routine).

## Differential evidence

`tools/reverse/formation_oracle.py` compares the actual compiled C++ module with
py65 1.2.0 execution of the original routines:

- 7,500 dynamic-program transitions: all actor indices and control states,
  boundary depths, sprite directions, every indirect-program choice, randomized
  angles/levels/random streams, and exact random-read counts.
- 340 launch transitions: four difficulty ranges, varying active counts,
  available formation slots, enabled/death gates and random masks; complete
  actor/path/control state and random-read count compared.
- 6,400 integrated gameplay updates: four 1,600-update scenarios covering
  launch, scheduler, per-actor dynamic hook, full grid motion, incoming and static
  formation hit injection, death retreat, scoring, sounds and bonus requests.
- Zero mismatches on 2026-11-04.

For launch and integrated traces, only bitmap erase/raster writes and applied
score/audio/popup side effects are intercepted. Requests for those effects are
recorded and compared. Original coordinate projection, descriptor sizing,
clipping, phase/cursor updates, random reads and control flow execute unchanged.
Dynamic-program-only checks need no interception. This establishes the bounded
subsystems, not full display-interrupt timing or end-to-end game equivalence.

```sh
g++ -std=c++20 -Wall -Wextra -Werror -Isrc/lib tests/FormationTest.cpp \
    src/lib/Game/Formation.cpp src/lib/Game/Enemies.cpp src/lib/Game/Polar.cpp \
    -o /tmp/gyrus-formation-test
/tmp/gyrus-formation-test
/tmp/gyrus-re-tools/bin/python tools/reverse/formation_oracle.py
```

## Deliberate playable-port correction: formation allocation overflow

Original `$F5` allocates home slots monotonically during a stage. Four normal
stage0 waves without hits produce counts9,18,27,36. Killing formed actors does
not decrement `$F5`. The main loop may then request one additional random wave
when fewer than six formation actors survive. A controlled original execution
with `$F5=36`, formationCount5 and random wave4 incremented `$F5` to37 after111
group updates. Original slot37 indexes beyond the 36 formation actors and aliases
adjacent RAM/object arrays. Resetting `$F5` or silently clamping would also change
normal behavior, and is not done by default.

The playable adapter may explicitly opt into
`EnemyInputs::reuseVacantFormationSlots=true`. Only after the first36 sequential
allocations, it chooses a vacant formation slot not reserved by another incoming
actor. `formationSlotReserved` is set during allocation/launch and cleared on
arrival/removal. `$F5` remains36 rather than wrapping or silently inventing more
formation slots. Exhaustion raises a descriptive `std::overflow_error`.

This opt-in is an intentional correction to the original aliasing bug, not a
claim of strict machine-state equivalence in overflow situations. The default
strict profile and all differential oracles retain original sequential allocation.
Regressions cover low surviving count/high `$F5`, another actor's reservation,
reservation release, unchanged 36th allocation, strict allocation37, and explicit
capacity exhaustion.

Wave index4 is a single optional random wave: preparation sets the next index to5.
It is not repeatedly prepared while the formation count stays below six.
