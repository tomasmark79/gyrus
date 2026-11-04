# Original scene rendering

`SceneGraphics.hpp/.cpp` reconstructs the original scene around the separate
160x96 indexed playfield. `SceneFrame` contains 160 color clocks by 200
scanlines, with raw Atari color codes. Display it at a 320x200 aspect ratio.
No CPU instructions or RAM image are used by the runtime renderer.

## Sources and generated data

`tools/reverse/export_scene.py` reads `analysis/initialized-memory.bin` and
exports only the required data into `SceneGraphicsData.hpp`:

- Six planet bitmaps from the `$9F08` records: Neptune, Uranus, Saturn,
  Jupiter, Mars and Earth (468 packed bytes).
- Planet player3 and missile masks (32 bytes) and their original positions.
- The four display-list layouts built by `$8827` and changed by `$8DE3`.
- Original message, copyright, congratulation and planet-name text.
- 512 font bytes from `$1E00..$1FFF`.

**Font provenance:** `$87C5` copies these glyphs from the emulator's OS ROM
and changes the final glyph to the lives icon. They are not present in the
ATR. Only these copied/modified glyphs are exported, not an OS ROM. The
initialized RAM and font hashes are recorded in the generated header.

## Layout and text behavior

The top mode6 line is 20 characters by eight scanlines. Bitmap pixels below
it span two scanlines each. The normal layout consumes 200 scanlines. Message
and copyright layouts consume 192; the perfect-stage congratulation layout
consumes 184. The remainder is blank. These source-row mappings are generated
from the original display lists rather than invented positions.

`hudCharacters` reconstructs `$982F`, `$98BC` and `$99F0` display behavior:
six score digits with a final zero, suppressed leading zeroes, up to four
reserve-ship icons (`min(lives,5)-1`), and the stage counter. Title mode shows
the two scores in the original left and right positions. `messageCharacters`
uses the original templates for WARPS TO, READY PLAYER, CHANCE STAGE, planet
names, bonus counts and perfect-stage congratulations.

`drawPlanet` preserves `$9F38`'s XOR operation. Title mode uses Earth at
bitmap y=48; gameplay uses each record's position. `composeScene` adds the
original planet player3/missile layers. Passing a **separate playerLayer**
keeps the ship's physical scanline position independent of message display
list remapping and lets it overlap mode6 text, as Atari player DMA does.

## Color evidence and limits

Live Atari800 7.1.2 monitor evidence supplied by the root investigation:

- Saved title at the first `$8069`: PF0/PF1/PF2=`94/24/44`, PF3=`28`,
  BK=`00`, PM3=`98`, PRIOR=`11`.
- Game-start state at the next `$8069`: PF0/PF1/PF2=`78/28/74`, PF3=`44`,
  PM0/PM1/PM2=`0E/0E/44`, PM3=`84`, BK=`00`, PRIOR=`11`.

These snapshots precede the next display-list interrupt. Normal gameplay
uses the four palettes selected by `stage&3` at `$9343`. Travel may request
the planet record's palette with `usePlanetPalette`. The stable title's
interrupt at bitmap row41 installs `94/24/44` for the lower field; its final
interrupt restores `04/0A/0E` for the next frame's upper field. The compositor
switches at logical row42. Exact instruction-cycle/scanline-edge timing is
not yet measured against a settled original screenshot.

`classicAtariRgb` is an explicit RGB profile from Atari800's historical
`act/default.act`, pinned to commit
`c3d8074628af3e6556a04228d0eb77d142fef195`, SHA-256
`9d3973cff27fa73e1a57301af404de6e49fc89b56cc9f9497806b10efccde1f1`.
This is a reproducible original Atari color-code mapping, **not** a claim to
match the installed emulator's dynamically generated PAL palette or a
physical television. Low color bit0 is ignored, as by GTIA color registers.

Source: https://github.com/atari800/atari800/blob/c3d8074628af3e6556a04228d0eb77d142fef195/act/default.act

## Verification

Nine focused tests pass with `_GLIBCXX_ASSERTIONS`, AddressSanitizer and
UndefinedBehaviorSanitizer. Original `$9F38` was executed offline with py65
to validate all six planet pixel buffers and title positioning. Original
`$982F` and `$98DE` validate score and message character bytes. Tests also
cover original scanline layout lengths, Earth overlay pixels, fixed player
positions over messages, and known colors in the named RGB profile.

```
/tmp/gyrus-re-tools/bin/python tools/reverse/export_scene.py analysis/initialized-memory.bin --oracle
```

These checks do not yet prove complete screen equivalence: dynamic DLI
timing, every planet-overlay priority overlap, flashing text/state changes,
and original-versus-port full screenshot comparisons remain to be verified.

## Original starfield

`Starfield.hpp/.cpp` is a separate clean implementation of `$8D1E..$8DBD`:
twenty slots, visited in descending order, normal updates on even frame ticks,
travel updates every tick, a spawn chance `(randomByte&7)==0`, even angular
positions, initial depth36 and depth decrement2. Original wave-state2 removes
stars when depth drops below18. The random-byte provider is injected; no
assumption about POKEY's hardware random stream is hidden in the class.

Draw uses original color-index3 XOR, including cancellation of overlapping
stars. The original corrupted row0 lookup makes y0 stars invisible and is
preserved. A 300-tick original-routine oracle covers frame-counter wrap,
travel gating, wave2 removal and 1,536 random-byte reads. Native frame hashes
aggregate to `2225442909`, normalized state hashes to `1751748955`, matching
the oracle. Four Starfield tests pass with assertions, ASan and UBSan.

```
/tmp/gyrus-re-tools/bin/python tools/reverse/export_scene.py analysis/initialized-memory.bin --starfield-oracle
```

## Live placement and collision proof

`python3 tools/reverse/probe_gtia.py` starts its own isolated Atari800 monitor
and replays twelve assertions against the captured game-start state. It
does not change the state file. Atari8007.1.2 gives P0PF values0/1/2/4 for
bitmap indices0/1/2/3, and simultaneous P0/P1/P2 contacts give registers
1/2/4 (OR7). PRIOR11 and PRIOR04 give identical contacts despite different
visual priorities. The fixture also establishes physical bitmap origin40:
PM `$2238=80`, HPOS88 contacts (x40,y36), not (x40,y40).
Player placement was corrected upward four bitmap rows; planet PM/missile
placement similarly removed an erroneous four-row offset. Prior offline
mask tests shared the wrong physical-origin assumption and alone could not
prove calibration. Native expected hashes and Earth overlay checks now
follow the live evidence.

`playerPlayfieldCollisions(field, playerLayer)` returns the current bitmap
contact OR mask. It requires separate uncomposited layers, ignores RGB and
priority, and includes only the normal doubled bitmap, not mode6 text PF3.
It does not implement persistent hardware latch timing. Original gameplay
reads D004, ORs D005/D006 at85D0, writes HITCLR D01E at85D9, then ANDs F9.
Normal F9=3 ignores PF2/index3 stars. This occurs within the DLI after
input/star/enemy updates: the hardware values reflect pixels rasterized
before the read, not newly mutated RAM. Pause, travel(B8), gate(D9=0), and
death branches can skip the read/clear, so latches must persist across those
branches. Other clears occur at84E4 (perfect transition) and936C (stage
initialization). A session should accumulate rendered contacts and consume
and clear at those original decision points; an unconditional fresh-frame
death test would not reproduce that ordering. Exact mid-raster mutation
timing remains outside this bitmap helper.
