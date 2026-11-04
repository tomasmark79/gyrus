# Stage initialization

`Stage.cpp` translates CPU `$92E6..$9413` from the loaded Atari image. It is
separate from `Turns.cpp`: it neither advances a planet nor decrements a life.
Call `initializeStage(turns, paths, enemies, formation)` after initial game
setup, `advanceStage`, or a `ResumePlayer` result from `advanceDeath`.

The function mutates the four named models and returns `StageSetup`. Its audio,
message, palette, HUD, star-clear, collision-latch-clear and object-visibility
fields are requests for the session/renderer. The helper does not start the next
wave. Wait for the message countdown before the main-loop wave scheduler.

## State boundaries established by the original

- `$DC..$E5` dynamic programs and `$24..$2D` incoming phases become zero. Path
  cursors `$1A..$23`, generated tracks, descriptor mutations, and group timers
  retain their values until wave preparation replaces them.
- Score-chain `$CD` becomes `planetOffset / 8 + 2`. The implementation also
  preserves the final LSR carry for unusual offsets; the stage progression
  produces multiples of eight.
- `$DA` becomes 40. `$99`, `$96`, `$F5`, `$9F`, and `$A9` become zero; resume
  subsequently restores `$96/$F5` from the selected player's saved formation.
- The selected player's saved wave becomes `$A0`. Negative `$AC` requests
  resume, and `$AC` is cleared at the end. Resume restores the first N formation
  objects using `$87AF`'s fixed `$AD7C` sprite descriptor and `$9A9A` geometry.
- Formation angles/depths are **not** reinitialized here. `$56` and `$5B` also
  retain their values. `$99F0` renders the level number; it is not a formation
  position initializer. The initial grid is established earlier in game setup.
- `$848A` clears visibility/width of every object and invalidates the star list.
  The caller must apply `clearObjectVisibility` to player shots, enemy shots,
  upgrade targets/particles, explosions and other objects outside these four
  model arguments. Logical coordinates and other retained bytes should survive.
- `$B4`, `$E6`, `$DB`, `$B8`, `$B3`, `$90`, and `$B7` become zero. These are
  returned as explicit setup fields for the relevant controllers. `$A7` firing
  cooldown and `$AF/$B0/$BB` upgrade timers/counters are not reset here.
- Level-derived formation difficulty uses the selected player. Playfield palette
  selection at `$9343` reads player one's level even during player two's turn;
  the port deliberately preserves this behavior.
- Special-shot reload is `[0,255,63,31][min(level/4,3)]`. Enemy-shot cap is
  `min(uint8(level+1)/2,4)` and cooldown reload is `66 - 8*cap`; level 255 wraps
  before division exactly as on the CPU.
- Warps zero means challenge stage. It sets the selected player's special phase
  to 3 and suppresses upgrade targets. Otherwise three targets are available
  only when that player's special phase is zero.
- Music IDs for warps 0,1,2,3 are 2,4,3,5. Message offsets are 0 for normal stage,
  40 for challenge, and 20 for resume; each starts a 150-tick message.
- Player position resets to angle 0/depth 12. Joystick latches and aim-angle
  bytes are not reset by this routine.

Saved formation counts above 36 are explicitly rejected: those values would
alias unrelated original object memory, outside the typed pool.

## Verification

`tests/StageTest.cpp` first failed against the empty helper at the resume/music
assertion, then passed with the translation. It checks restored visibility,
preserved shifted grid positions/counters, stage-zero parameters and challenge
flags.

`tools/reverse/stage_setup_oracle.py` executes the original at `$92E6` and stops
at `$A01D`, the final player raster entry. The original screen clears, palette,
HUD/level, message formatting, sprite descriptor geometry and perspective
projection execute unchanged. Only the zero-value score refresh `$982F`, music
request `$B901`, and final XOR raster writer `$9B98` are observational stubs.
The music argument is captured and compared. This does not claim raster or
audio waveform equivalence.

96 complete initializations cover both players, fresh/resumed stages, levels
0..19/63/127/254/255, all warp and planet choices distributed deterministically,
saved formation counts 0/1/12/36, and special phases 0/1/3/128. Comparison covers
difficulty, colors, music, upgrade flags, projectile tuning, counts, saved wave,
message, player projection, unchanged frame/grid counters and collision mask,
all phases/programs, and all 36 formation positions/visibility widths. Result:
zero mismatches.

Run with `/tmp/gyrus-re-tools/bin/python tools/reverse/stage_setup_oracle.py`.
The existing `stage_oracle.py` separately verifies `advanceStage` and is unchanged.
