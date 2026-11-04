# Death countdown and player turns

`Turns.cpp` translates `$855F..$85C2` into explicit state. Its inputs are the
current death countdown, incoming/formation counts, wave number, active player,
and both players' lives, upgrade and resume fields. A separate subsystem detects
the hit and starts the countdown; this routine does not invent collision rules.

When the countdown expires while incoming enemies remain, the original reloads
it from the incoming count. When that count is zero, the routine saves the wave
and formation, consumes a life, and selects the other living player (or the same
one when the other has no lives). It returns game over only when both have none.
Original byte wrap and special-object/protection branches are preserved.

The `ResumePlayer` result leaves the countdown at `$FF`, exactly as the call to
stage initialization `$92E6` expects. The adapter must restore that player's
stage state and clear the countdown; repeatedly calling this routine without
stage initialization would be incorrect. This module is not yet connected to
the first-wave preview.

`tools/reverse/turn_oracle.py` compares 8,192 deterministic cases with original
6502 execution, including countdown, alternating/single players, final lives,
upgrades, special flags and saved wave/formation state. It intercepts HUD redraw
at `$98BC` and stops at stage initialization or title reset. It does not verify
the behavior of those downstream routines.

`advanceStage` separately translates `$9425..$945E`: it updates protection from
the old level, clears saved-wave/special state, increments the level, decrements
the warp counter and advances the planet after the chance stage. Planet offsets
cycle through 0,8,16,24,32,40. `stage_oracle.py` compares all 12,288 combinations
of both players, all byte-sized levels, four warp counts and six planets, with
zero mismatches. It also stops before stage initialization.
