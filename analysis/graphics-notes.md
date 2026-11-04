# Gyruss graphics evidence

Source image SHA-256: `dd2ec0f5ca190c1f1446b76d67e1bd13e9a18e19d0792807b887474030752dcc`.
The root investigation supplied `initialized-memory.bin`, a complete Atari800
RAM dump at the first `$8069`, after graphics initialization. Addresses below
refer to the original 6502 program or this RAM image.

## Main playfield and title

- `$8827` builds the display list at `$0400`. The initial bytes from `$884E`
  are `70 70 70 46 D8 3F 4D C4 30`. Three eight-scanline blank instructions
  precede a mode-6 text line at `$3FD8`, then mode-D bitmap data at `$30C4`.
  Mode D is 160 pixels across, 2 bits/pixel, two scanlines per pixel row.
- There are 96 bitmap rows, 40 bytes per row, occupying `$30C4..$3FC3`.
  The framebuffer lookup built at `$8037` uses low-byte table `$2000`
  and high-byte table `$2060`. `$878A` computes `rowBase[y] + x/4` and
  preserves `x&3` for sub-byte alignment.
- Title bitmap: `$A909`, 26 bytes x 34 rows, unpacked MSB-first into
  104 x 34 color-index pixels. `$8F0B..$8F3D` copies it to `$320B` with
  40-byte screen stride: bitmap x=28, y=8. Its on-screen height is 68 scanlines.
- The title was visually decoded in `graphics-atlas.svg`: it clearly reads
  GYRUSS. The preview colors are diagnostic, not measured Atari RGB values.

## Software sprite renderer

`$9A97..$9C0E` projects an object's angular/radial position, selects an image,
and XOR-blits its packed 2bpp bytes to the bitmap. XOR is important: the same
image at the previous location erases it without clearing the star field.

Object arrays:

| Address | Meaning supported by renderer |
| --- | --- |
| `$046E + object` | projected horizontal coordinate |
| `$04C6 + object` | projected vertical coordinate |
| `$051E + object` | angular coordinate passed to projection |
| `$0576 + object` | radial/depth coordinate passed to projection and LOD |
| `$05CE + object` | size in low 7 bits; high bit used by collision/removal logic |
| `$0626 + object` | sprite orientation/frame index |
| `$067E + object` | even type-table offset (0,2,...,56) |

There are 29 kinds; their metadata pointers are at `$AC95 + kind*2`.
Two arrays of bank pointers begin at `$ACCF` and `$AD09` (offset `$3A`).
The complete extraction formulas are implemented in
`tools/reverse/export_graphics.py`. The earlier diagnostic atlas uses a fixed
alignment for identification; the C++ exporter retains both alignment banks.

For `depth=(radial>>3)&7`, the corresponding nibble of metadata bytes 1..4
selects sprite size. The renderer chooses one of four bank pointers with
`(orientation>>1)&6`. The first bank array is used when the actual left edge
has `(x&2)==0`, the second otherwise. Source offset adds metadata `[5+(orientation&3)]` and
`[8+size]`. Each row has `(size+6)/4` bytes (integer division); there are
`size` rows. Padding in these rows supports byte alignment and must not be
mistaken for the visible logical bounding box. The blitter adds an extra
two-bit shift when the actual left edge has `x&1`.

The atlas decodes 3,712 parameter combinations (29 kinds x 8 depths x
16 orientations), with 340 unique pixel sequences. Visual inspection confirms
ships, projectiles, explosions and numeric score sprites. Kinds 20..28 are
the score images `00`, `5`, `10`, `15`, `20`, `25`, `30`, `35`, `40`.
Kinds 14..16 look like explosion stages. Do not infer all remaining gameplay
roles from appearance alone.

The initialization routines generate additional graphics in RAM:

- `$A5C7`: row reversal for sprite families; source/destination pairs at `$A5BB`.
- `$A699`: reverse 2-bit pixel order and row order, with alignment correction;
  descriptors at `$A651`.
- `$A868`: generate bit-plane/shifted variants from packed sprites using
  descriptors at `$A773`.
- `$9D5B`: generate mirrored player masks using descriptors at `$9D47`.

Consequently the raw cartridge bytes alone are insufficient for all bank
pointers. The decoder deliberately requires initialized RAM.

## Player ship (hardware sprites)

The player's ship is separate from the software sprites. `$9C14..$9D44`
draws it with three player graphics channels. PMBASE is `$20`; DMACTL is
`$2E` and GRACTL is `3` (set at `$87E6..$8810`). The three scanline-mask
destinations are `$2210+Y`, `$2290+Y`, `$2310+Y` (player channels 0..2,
double-line DMA). The fourth player and missiles also participate in
background/planet graphics through `$9FD8`.

- Ship image pointer table `$9DA3`: `$9E5E,$9DC3,$9E04,$9E3B` and mirrored
  equivalents `$2531,$25DB,$261C,$2653`.
- At `$9DB3`, pairs of row-count and vertical-offset are
  `(11,0),(6,2),(5,2),(3,3)`, repeated for the mirrored bank.
- Each ship bank starts with five orientation offsets. A selected offset
  indexes plane 0; planes 1 and 2 are one and two row-counts further on.
- The orientation folds across a quadrant via indices `0,1,2,3,4,3,2,1`.
  Some orientations write rows bottom-to-top. `$9C5E..$9CC0` programs
  horizontal player offsets; the third channel uses `$B82B` offset table
  `4,5,8,9,9,9,8,5,4,3,0,-1,-1,-1,0,3`.

A native renderer can decode these masks into ordinary indexed images;
it does not need Atari DMA emulation. Preserve the original overlapping
plane composition and offsets when producing the final ship atlas.

## Font and palette

`$87C5..$87D4` copies 512 bytes of OS glyphs from `$E000..$E1FF` to
`$1E00..$1FFF`, then modifies the last glyph. CHBASE becomes `$1E`.
Thus the game font is **not stored in the supplied ATR**. The initialized
snapshot contains 64 eight-row glyphs provided by the currently selected
emulator OS. `graphics.json` includes these for analysis, with source
provenance retained; do not describe them as extracted from the ATR.

Palette indices 0..3 correspond to background and the three playfield
registers `$D016..$D018`; actual RGB depends on Atari color conversion.
The six triples at `$8DD1` are:

`04 0A 0E`, `94 24 44`, `46 28 44`, `78 28 74`, `2A 28 24`, `C8 28 C4`.

`$8DC2` installs a triple. The title display-list interrupt chooses the
second triple before VCOUNT `$40` and the first afterward (`$8508..$851D`).
Background/planet table records at `$9F08` have eight bytes each: 16-bit
image pointer, three playfield colors, packed height/row-byte count, then
vertical/horizontal positioning. `$9F38` uses record offset `$28` in title
mode, or the selected player's progression byte in gameplay.

## Reproduce

```
python3 tools/reverse/graphics.py analysis/initialized-memory.bin
```

This writes indexed `analysis/graphics.json` and a diagnostic SVG atlas.
The generated sprite output is derived data, not a C++ game or runtime
equivalence claim. Final Atari screenshot comparison remains necessary.

## Clean C++ graphics implementation

`src/lib/Game/Graphics.hpp/.cpp` decodes only compact data arrays exported in
`GraphicsData.hpp`. Runtime contains neither a 6502 interpreter nor an Atari
memory image. Exported data comprises 533 unique packed sprite records
(7,843 bytes), 678 ship-mask bytes, and the 884-byte title bitmap. The OS font
is excluded. Both original sprite alignment banks are retained.

`Framebuffer` is a 160x96 `std::array<uint8_t>`. Software graphics use indices
0..3. `drawPlayer` overlays indices4..6 and returns the original Atari color
codes for those channels, including the distant ship's color change.

`tests/GraphicsTest.cpp` compares software sprite pixel hashes against original
`$9AF8` execution in an offline py65 oracle, including all four horizontal
alignments and the top-left clamp. Player tests compare original `$9C14`
mask output and hardware positions after compositing three channels. All 128
angles at six depths (6,12,20,28,40,58), totaling 768 frames, match the oracle
aggregate hash `335323119`; this covers odd-angle rounding and every ship
size. Seven focused native Graphics tests pass. The
oracle command is:

```
/tmp/gyrus-re-tools/bin/python tools/reverse/export_graphics.py analysis/initialized-memory.bin --oracle
```

The oracle is an analysis dependency only. Its hashes are retained as constants
in the native tests. GTIA priority behavior for PRIOR `$11` was cross-checked
against Atari800's COLPM handlers in
https://github.com/atari800/atari800/blob/master/src/gtia.c: these player channels
appear in front of the ordinary playfield. Exact RGB output and final whole-screen
comparison remain outside these indexed-pixel tests.

The small ship banks contain two visible planes, but the original loop still
reads three planes while positioning the third offscreen. Export therefore
includes the full accessed span `max(facingOffsets)+3*height`, including the
neighboring original bytes. Copying only the nominal bank sizes caused an
out-of-bounds C++ read that ordinary pixel hashes did not detect because the
affected plane was hidden; debug assertions exposed it. A compile-time bound
check now prevents truncated exports, and assertions plus ASan/UBSan cover
the 768-frame oracle test.

The original startup font patch writes its final byte at `$2000`, overwriting
the first framebuffer row lookup. That row subsequently points at `$3010`
instead of `$30C4`. A sprite whose clamped top row is zero starts 180 bytes
before the visible framebuffer and advances by40 bytes per row from there.
The C++ blitter preserves this original quirk with a signed pixel offset;
two additional `$9AF8` oracle cases verify partial visibility. Starfield
points at y0 are offscreen for the same reason.

Live Atari800 calibration corrected an earlier shared oracle assumption:
ANTIC begins its display list at physical scanline8, so 24 blank scanlines
and the eight-line HUD put the bitmap at scanline40, not32. PM address
`$2210` represents scanline32. Player placement therefore subtracts four
double-line bitmap rows, and the oracle samples PM starting at `$2214`.
The previous aggregate `1458363991` verified decoding but shared the wrong
vertical calibration. The new hash above includes the empirically verified
offset. `tools/reverse/probe_gtia.py` demonstrates that `$2238=80` and
HPOSP0=88 contact bitmap(x40,y36), while (x40,y40) produces no collision.

## Player-shot heading regression

Descriptor `$B5E4` begins with zero, so projection/draw routine `$9A97`
always assigns heading0 at `$9AF2`. Polar angle determines position only;
using angle/8 to select the projectile sprite orientation exposes unrelated
packed sprite variants. `tools/reverse/shot_graphics_oracle.py` executes the
original routine for every even angle0..126 and every actual travel depth
12..54 in steps of2, exporting1408 indexed-raster hashes to
`tests/PlayerShotRasterData.hpp`. It also executes the original blitter with
the former angle/8 heading and detects1056 raster mismatches.

The `GameSession.PlayerShotsMatchOriginalRasterAtEveryAngleAndTravelDepth`
regression calls the actual session renderer, XORs away an unchanged baseline
scene and compares the isolated shot with those independent original hashes.
Thus it protects the session's heading selection, not just the sprite decoder.

```
/tmp/gyrus-re-tools/bin/python tools/reverse/shot_graphics_oracle.py
```
