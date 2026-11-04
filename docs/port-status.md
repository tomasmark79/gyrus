# Gyrus: Atari Gyruss to portable C++

## Foundation

This project starts from https://github.com/tomasmark79/NixonCpp at commit
`8feefd36844009afbc691f562616cf64ce2c7f4f`.
The template was renamed with its own `scripts/rename.sh Gyrus GyrusLib gyrus`.
Its Meson build, Nix environments, utility services, tests, and Emscripten
toolchain are retained. Gyrus has an independent Git history and its own
repository, https://github.com/tomasmark79/gyrus. Its sole remote, `origin`,
points there. NixonCpp is not configured as a remote.

The executable runs an SDL2 combat preview with original title, HUD, planets,
ship, incoming waves, formation attacks, enemy shots, scoring and synthesized
sound. It is not yet a complete playable port. Player damage, special enemies
and stage progression remain outside the interactive preview.

## Reverse engineering and C++ translation

- The executable stream has been extracted reproducibly by
  `tools/reverse/extract.py`. Its program payload exactly matches an Atari800
  memory dump taken after the disk loader completes.
- Entry point `$8028`, main loop `$8069`, display-list interrupt `$84AB`, and
  audio VBI `$9A94` are identified. See `analysis/loader-notes.md`.
- `src/lib/Game/Polar.cpp` translates projection `$A0CB` and steering `$A0BD`
  into ordinary C++ with the original perspective data. It does not execute
  6502 instructions or require an emulator at runtime.
- `tools/reverse/polar_oracle.py` compares the C++ against original 6502
  execution: 8,192 projection inputs and 16,384 steering inputs, no mismatches.
  This covers angles 0..127 and depths 0..63; it is not full-game equivalence.
- Original logo and software sprites have been decoded and inspected in
  `analysis/graphics-atlas.svg`. `analysis/graphics-notes.md` records addresses,
  palettes, hardware ship planes, and remaining rendering questions.
- `src/lib/Game/SoundEffects.cpp` translates all ten original sound effects
  into four explicit channel states. `tools/reverse/audio_oracle.py --verify`
  compared 11,264 updates (individual effects plus concurrent replacement) with
  original-code execution, with no register/state mismatches. These are POKEY
  control values, not synthesized audio.
  `analysis/audio-notes.md` records the recovered audio formats and routines.
- `Player.cpp` matches original input, movement and shot routines across 192
  input, 7,169 movement and 1,456 projectile cases. See `player_oracle.py`.
- `Music.cpp` matches 294,912 original updates across all eight tracks,
  transitions, repeat handling and effect ownership masks. `Audio.cpp` combines
  both sequencers with exact register-write masks; 131,072 combined frames match
  the original. SDL queues mono floating-point PCM from the POKEY model. Analogue
  mixing/filtering and frame-level register timing are approximations; listening
  comparison with the original remains outstanding.
- `Graphics.cpp` draws original title, alignment-sensitive XOR sprites and
  three-plane ship masks without a CPU emulator or OS ROM. Player rendering is
  compared against 768 original-routine frames. Debug tests caught an incomplete
  exported small-ship bank; the corrected 678-byte export preserves the original
  accessed data and passes ASan/UBSan and unchanged reference hashes.
- `Enemies.cpp` matches 576 wave preparations and 19,500 incoming/scheduling
  updates, including mutable descriptors in this disk-loaded version.
- `Score.cpp` matches 3,584 original cases, including the 60,000-point extra life.
  `Collision.cpp` matches 401,408 overlap cases and 512 complete shot/target scans.
- `Turns.cpp` matches 8,192 death countdown and turn-selection cases, stopping
  before stage reinitialization. It is not yet connected to the preview.
  Its stage-advance routine also matches all 12,288 tested combinations of
  player, level byte, warp count and planet.
- `Starfield.cpp` matches 300 original updates, including random-read count,
  state and framebuffer hashes. The first-wave preview now includes moving stars.
  Both stars and software sprites preserve the initialized row-zero lookup quirk.
- `SceneGraphics.cpp` adds the original font, display-list layouts, HUD/messages,
  six planets and an explicit classic Atari800 palette. Separate player masks
  preserve hardware positioning across message layouts. See `analysis/scene-notes.md`.
  A later live GTIA fixture corrected the earlier assumed bitmap origin: ANTIC
  starts this bitmap at scanline 40, shifting player masks four bitmap rows up.
  The previous mask oracle checked extraction consistently but did not validate
  that physical origin; the corrected renderer and expectations use live evidence.
- `Formation.cpp` reconstructs launch, dynamic steering and the rotating grid.
  Its oracle covers 7,500 program transitions, 340 launches and 6,400 combined
  updates including grid hits and random-byte consumption.
- `EnemyShots.cpp` matches 30,208 movement, spawn and death-gate cases. Ordinary
  enemy shots are connected to the preview; the death gate awaits the complete
  death/restart sequence.
- Player-shot rendering now uses fixed heading zero, as descriptor `$B5E4`
  requires. The previous adapter incorrectly used `angle/8`, producing correct
  sprites only in one quarter of the orbit. The regression compares all 1,408
  even-angle/travel-depth frames against `$9A97`; all match, while the old code
  mismatches 1,056. See `tools/reverse/shot_graphics_oracle.py`.
- All 21 native Meson test targets pass. The SDL2 frontend builds for desktop
  and WebAssembly. Fixed-step adapter tests cover display rates 30..144 Hz and
  bounded catch-up after background suspension.

Player damage, special objects, complete background animation and stage/turn
progression remain outstanding. Incoming and formation enemies are shot targets.
The adapter uses a 50 Hz PAL tick; end-to-end timing still needs
comparison with original runtime captures. Raster priority and title color-switch
boundaries need fuller runtime comparison.

The preview uses reproducible adapter randomness rather than cycle-timed POKEY
RANDOM. It opts into `reuseVacantFormationSlots`: after 36 formation allocations,
the original can index beyond its table during the optional random wave. The port
reuses an unoccupied, unreserved formation slot in that case. Strict reference
comparisons keep this repair disabled and reject the unsupported overflow. See
`analysis/enemy-notes.md` and the formation report for the exact evidence boundary.

## Interactive preview validation

Native SDL software rendering produced title and player BMP captures using
`--frames 3 --capture ...` (and `--preview` for the player). The web build ran
in dedicated headless Chromium: the original logo displayed, Start preview
entered the field, ArrowRight moved the ship, and a Space press produced an
inward shot. Screenshots were inspected. The final run had no browser runtime
exceptions or console errors. SDL renderer creation now occurs inside the first
Emscripten frame, after the loop exists: creating it earlier made SDL's swap
interval setup call the timing API too early, even with vsync disabled.
The initial browser smoke check also caught Emscripten clearing the Ready label
after initialization; readiness now persists in the page status handler.

`tools/reverse/browser_preview_check.mjs` reproduces the input/screenshot smoke
check with a local HTTP server on 8785 and dedicated Chromium CDP on 9444.
This validates the controls preview, not enemy gameplay or whole-game fidelity.
After scene/audio/first-wave integration, native debug and WebAssembly release
builds passed again, as did all 14 native targets. A fresh Chromium run reached
Ready, accepted a genuine Start click and movement/fire input with no runtime
exceptions or console errors. The HUD, planet and player capture was inspected.
This does not establish audible-device playback or listening equivalence.
An instrumented Chromium run additionally observed a running Web Audio context,
318 output callbacks and nonzero PCM (peak 0.1883) after a real Start click.
The smoke script now checks that audio reaches the browser output callback;
physical speaker output and listening equivalence remain unverified.

## Foundation validation

On 2026-11-04, the renamed native debug build completed successfully.
All three inherited Meson test suites passed (AssetManagerTest,
ConsoleLoggerTest, FileReaderTest). Running `Gyrus` initialized GyrusLib,
loaded `GyrusLogo.svg`, and exited successfully. The original ATR checksum
still matched after template import. This validates the template foundation,
not game behavior.

The WebAssembly release build also completed successfully with the template's
Emscripten 6.0.9 toolchain, producing `build/builddir-wasm-release/Gyrus.html`
and its companion JavaScript/WASM files. Browser preview execution is checked
above. Reproduce the builds from the project root with `make debug` and
`make cross-wasm`; the template scripts select their Nix environments.

Re-run reference comparisons after modifying translated routines:

```sh
.venv/bin/python tools/reverse/polar_oracle.py
.venv/bin/python tools/reverse/audio_oracle.py --verify
.venv/bin/python tools/reverse/player_oracle.py
.venv/bin/python tools/reverse/music_oracle.py --verify
```

These require `py65==1.2.0`, a C++20 compiler, and the extracted local memory
file. Ordinary game builds and Meson regression tests do not require py65.

## Original artifact

- Input: `GYRUSS.atr` (preserved unchanged).
- Size: 16,912 bytes.
- SHA-256: `dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc`.
- ATR header: `96024008800000000000000000000000`.
- Header declares 128-byte sectors and 16,896 bytes of disk data (132 sectors).
- Initial boot header bytes: `00 03 00 07 14 07`.
  Interpreted as Atari boot metadata, these declare three initial boot sectors,
  loading at `$0700`, with initialization at `$0714`.

The loader observations have now been corroborated in Atari800. It loads
the segments directly, without decompression. Gameplay has only been partially
reconstructed; see the subsystem evidence above.
The connected REA raw-binary interface does not advertise 6502 support;
do not open this image with a misleading x86 or ARM architecture.

## Implementation direction

Keep gameplay in portable, readable C++ within the template's library structure.
Use the SDL2 adapter shared between the native executable and Emscripten browser
build. Do not substitute an emulator wrapper for the requested C++ reconstruction.

Next work:

1. Translate formation attacks, enemy shots and special objects.
2. Connect damage, lives and stage/turn transitions with reference tests.
3. Complete moving backgrounds and validate raster priority and palette timing.
4. Compare synthesized sound with original playback and verify browser audio activation.
5. Compare full desktop/browser play against Atari800, including timing and restart.

The template's MIT license covers reused template code. It does not establish
a license for the supplied Atari game or recovered game assets.
