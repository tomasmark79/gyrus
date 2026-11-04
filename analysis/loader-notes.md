# Loader and runtime evidence

Source SHA-256 and segments: [manifest.json](manifest.json).

The boot header loads three 128-byte sectors at `$0700`. Code at `$0714`
checks the executable marker, reads little-endian segment start/end addresses,
copies bytes to their target addresses, handles INITAD, then jumps through
RUNAD at `$07AB`. `$07EB` supplies bytes using SIO reads into `$0880`.
The 24-bit byte counter at `$0709` is `$00400C` (16,396), excluding sector
padding. The stream begins at ATR offset 400, its first payload at 406.

Segments: `$8000..$BFFF` (16,384 bytes), then `$02E0..$02E1` = `$8028`.
There is no decompression in this loader. On 2026-11-04, Atari800 7.1.2
stopped at `$07AB` after disk loading; dumping `$8000..$BFFF` and comparing
to the extracted segment with `cmp` yielded identical bytes.

## Reproduction

```
python3 -m venv /tmp/gyrus-re-tools
/tmp/gyrus-re-tools/bin/pip install py65==1.2.0
/tmp/gyrus-re-tools/bin/python tools/reverse/extract.py GYRUSS.atr \
  --disassemble --root 9a94 --root 84ab
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy atari800 \
  -xl -nobasic -no-autosave-config -nosound -monitor GYRUSS.atr
```

At the emulator monitor:

```
bpc 7ab
cont
write 8000 bfff analysis/runtime-loaded.bin
bpc 8069
cont
write 0 ffff analysis/initialized-memory.bin
savestate analysis/title.a8s
```

`initialized-memory.bin` contains system ROM as well as game memory. It is a
local analysis reference, not a distributable game asset or runtime dependency.
The selected Atari800 configuration reports `DEFAULT_TV_MODE=PAL`.

To exercise the observed start branch directly (this is an injected flag,
not proof of physical keyboard handling): `c 8d 1`, then `cont` to the next
`$8069` breakpoint. This produced `game-start-memory.bin` and `game-start.a8s`.

## Root routines

- `$8028`: clears zero page, initializes work areas and derived graphics.
- `$8069`: main loop waits for `$55` to change, processes game state/collisions.
- `$83B0`: start flags `$8D` / `$8E`, configures one/two-player state.
- `$84AB`: display-list interrupt, updates game tick and moving objects.
- `$87B8`: installs deferred VBI `$9A94`, DLI `$84AB`, and configures graphics.
- `$9A94`: jump to the audio VBI handler `$B9DB`.
- `$8EF0`: title-screen state setup and logo copy.
- `$9634`: wave/path preparation; mutates some source table bits in RAM.

The first static reachability pass found 3,978 instructions and 77 direct call
targets after adding the two callback roots. This is not proof of exhaustive
code coverage: possible self-modifying operands and indirect dispatch need
runtime evidence. Several `$FFFF` operands in the original image remain
unchanged at the first main-loop entry. Whether these are later patched or
belong to unused paths is unresolved; do not treat them as known RAM targets.

The emulator command meanings are documented by its built-in `help` and
[upstream usage guide](https://github.com/atari800/atari800/blob/master/DOC/USAGE).
