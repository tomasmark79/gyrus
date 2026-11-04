# Gyruss reconstruction plan

The requested result is a playable, maintainable C++ reconstruction of the
supplied Atari game, using NixonCpp and working on desktop and in a browser.
The original executable is a behavioral reference, not the runtime engine.

## Evidence and boundaries

- Preserve `GYRUSS.atr` and identify all extracted data by source address/hash.
- Capture actual loader and gameplay state with Atari800.
- Use analysis-only 6502 execution for reference tests of recovered routines.
- Put translated gameplay in `src/lib/Game`, without SDL or Emscripten types.
- Keep NixonCpp's services, build tooling, tests and cross-compilation.
- Use SDL for the application adapter; prefer a version supported by both
  the pinned native Nix environment and Emscripten toolchain.
- Report approximations and missing systems; a title screen is not a complete port.

## Work sequence

- [x] Extract executable segments and verify against emulator memory.
- [x] Identify startup, main loop, display-list and vertical-blank callbacks.
- [x] Recover sprite/font formats, movement tables, input and audio commands.
- [x] Translate perspective and shortest-path steering, and exhaustively compare
  the C++ output with the original 6502 routines over the documented domain.
- [x] Translate coordinate conversion, player movement and projectiles; compare
  against original 6502 results at boundaries and all angular positions.
- [ ] Translate wave spawning, enemy paths, collisions, scoring, lives and stage
  transitions. Test fixed input/randomness traces against original snapshots.
- [x] Translate incoming wave decoding, trajectories, shot overlaps and scoring.
- [x] Decode original artwork and implement rendering, title, score and messages.
- [x] Integrate original title, software sprites and hardware ship masks in a
  portable framebuffer and desktop/browser first-wave preview.
- [x] Translate music/SFX sequencing and compare against original register traces.
- [x] Integrate shared music/SFX registers and a portable POKEY audio model.
- [x] Connect desktop and browser event loops, input, resizing and audio startup.
- [ ] Verify audible browser playback and compare synthesis with original audio.
- [ ] Validate actual play from title through death/restart and stage transitions
  on desktop and browser. Preserve reproducible commands and comparison evidence.

## Verification priorities

Compare integer overflow and angle wrap explicitly; preserve tick timing across
browser refresh rates; verify collisions near the center and viewport edges;
test restart and two-player state independently; ensure audio resumes after a
browser user gesture. Unsupported or not-yet-understood behavior stays documented.
