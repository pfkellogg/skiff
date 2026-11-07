# Skiff + FM-1 Big Screen

[Skiff](https://github.com/illuminus5/skiff) by illuminus5 (a six-track groovebox for the M-VAVE FM-1, built on Leo
Kuroshita's [Felucca](https://github.com/hugelton/Felucca)), with one addition: while an app asks for it, the FM-1
sends its screen over its USB cable, so [FM-1 Mirror](https://github.com/pfkellogg/fm1-mirror) (Mac, Windows, Linux)
and [FM-1 Mirror + Spyglass](https://github.com/pfkellogg/fm1-mirror-spyglass-android) (Android) show it big, live.
For players who can't read the FM-1's 1.54-inch screen.

This is an **unofficial** build, not a Skiff release, and illuminus5 has nothing to do with it. Everything else is
**Skiff 2.1.1** (commit `48e77fd`, 2026-10-31) as published. Tested on a real FM-1 (2026-11-03): HOME, the tracks,
the PRESETS, the MENU, with FM-1 Mirror on a Mac and FM-1 Mirror + Spyglass on an Android tablet.

**Watch it:** [Skiff on a Big Screen + the FM-1 Firmware Family Tree](https://youtu.be/lmMT-FtAoSE) (4:53): Skiff on the big screen, and why
23 of the 25 most popular FM-1 firmwares come from Felucca.

A fourth: **[SLOOP + FM-1 Big Screen](https://github.com/pfkellogg/sloop-fm1/blob/big-screen/BIG_SCREEN.md)**, tested on a real FM-1 (2026-11-07). Watch it: [SLOOP on a Big Screen](https://youtu.be/Pd4Bm6JvtkU).

A fifth: **[Melodee + FM-1 Big Screen](https://github.com/pfkellogg/melodee/blob/big-screen/BIG_SCREEN.md)**, tested on a real FM-1 (2026-11-07).

## The family Skiff comes from

Skiff, JIANT FM and ChoralRoot all come from the same parent, Leo Kuroshita's Felucca, and so do 23 of the 25 most
popular FM-1 firmwares: **[FIRMWARE_FAMILY.md](FIRMWARE_FAMILY.md)** has the study, the family tree and every source.

## Skiff moves fast

Skiff gets new versions often. This build is **Skiff 2.1.1** plus the screen. It doesn't update itself: when Skiff
moves on, this build stays at 2.1.1 until the screen is added to a newer Skiff and released here again (the release
names say which Skiff is inside). You can always go back to the newest Skiff with its own
[web installer](https://illuminus5.github.io/skiff/webapp/installer/): that one has no big screen.

## Installing it

1. **Charge the FM-1, plug it straight into the computer** (no hub), close every app that uses it.
2. With this repository and Python 3 (`pip3 install mido python-rtmidi`), from its folder:

   ```
   python3 tools/fm1_install.py Skiff-2.1.1-BigScreen.fwsc --yes
   ```

   (the `.fwsc` from this fork's Releases). It writes for about two minutes, then the FM-1 restarts.
3. **Unplug the USB cable and plug it back in** if the computer doesn't see the FM-1 after the restart.

Skiff's installer writes over M-VAVE's official firmware (V15 or V16) and over Skiff. We also installed this build
straight over JIANT FM, another Felucca firmware, on 2026-11-03, and it worked. Over M-VAVE's Open firmware or the FM-1
Big Screen firmware, put the official V15 back first (M-VAVE's [FM1 Workbench](https://open.m-vave.com/fm1-workbench):
Connect device, then Device update › Restore original V15).

**Back:** Skiff's web installer puts the newest Skiff back, or the official firmware (it saves a backup of your music
first).

## Seeing the screen

Open **FM-1 Mirror** (Mac, Windows, Linux) or **FM-1 Mirror + Spyglass** (Android) with the FM-1 plugged in by USB:
the screen appears, big, in colour or high-contrast looks.

**The sound playing, on top of the spyglass.** The FM-1 also tells the app what's playing: the selected track, its
engine and its sound (`T1 ANALOG ACID`). FM-1 Mirror shows it in gold on top of its spyglass and follows the PRESETS
knob and track changes. (It's sent only while an app asks, like the screen.)

**USB only.** Skiff doesn't use the FM-1's Bluetooth MIDI, so **FM-1 Mirror Bluetooth** can't show it.

## What changed in the code

The same drop-in as [JIANT FM + Big Screen](https://github.com/pfkellogg/JIANT/blob/big-screen/BIG_SCREEN.md) and
[ChoralRoot + Big Screen](https://github.com/pfkellogg/ChoralRootFM1/blob/big-screen/BIG_SCREEN.md): Skiff is built on
Felucca too, so the screen goes in at the same places. **About 37 lines in Skiff's own files**, behind one switch,
`SKIFF_BIG_SCREEN` (1 in `felucca.c`):

| File | What |
|---|---|
| `firmware/src/lcd.c` | `mirror_lcd(...)` in `lcd_fill` and `lcd_blit`: every rectangle on its way to the LCD |
| `firmware/src/usb.c` | where `sysex_byte` finishes a message: a 6-byte `F0 7D 'F' 'S' .. F7` goes to `sm_sysex`; the SysEx send ring's size (`SXQ`) can be set from outside |
| `firmware/src/main.c` | `mirror_service()` once a main-loop pass |
| `firmware/src/felucca.c` | the switch, `#include "skiff_mirror.c"`, and `SXQ 512` (the screen at ~67 KB/s, not ~25) |

New files, all MIT:

- `firmware/src/skiff_mirror.c`: the adapter. Each row goes back into RGB565 for `sm_rows`; each message goes into
  usb.c's SysEx queue only when it has room (never waiting on USB); a new viewer, or rows that had to be dropped, get
  the whole screen again (at most twice a second). Every 100 ms it puts the sound together (Skiff's own
  `sound_name()`, as the header does) for `sm_name`, and Skiff's BPM (`F0 7D 'F' 'S' 03 lo hi F7`).
- `firmware/src/screen_mirror.c` / `.h`: the firmware-neutral screen mirror from
  [pfkellogg/fm1_open, `screen_mirror/`](https://github.com/pfkellogg/fm1_open/tree/screen-out/screen_mirror), with its
  sound-name message (`F0 7D 'F' 'S' 02 name F7`).

**Also in this build: Song Position Pointer** (`SKIFF_SPP`, about 72 lines in `midi_clock.c`, `seq.c`, `usb.c`,
`midi_uart.c`). With MENU › MIDI › CLOCK on USB or TRS, a Song Position Pointer (`F2`) from the master moves each
track to the step that position reaches at its own STEP DIV, LEN and direction, so a master that jumps (a DAW, a looper)
lands Skiff in place; Continue then plays on. The patterns' steps only: a song's entries don't move. Nothing changes
with CLOCK on INT.

Size: image 566,860 bytes; RAM 91,460 of 98,304 bytes.

## Building it

As Skiff (BUILDING.md), or on a Mac without Docker: `tools/build.py` also takes JieLi's **Windows** toolchain 2.5.3
under Wine (the same clang 4.0.1 for pi32v2):

```
JIELI_WINE=/path/to/wine JIELI_WIN_BIN=/path/to/JL/pi32/bin ./build.sh
```

## Licence

Skiff and Felucca: GPL-3.0-only (LICENSE, LICENSING.md), and so is this build as a whole; its source is this branch.
`skiff_mirror.c`, `screen_mirror.c` and `screen_mirror.h`: MIT, © 2026 Patrick F. Kellogg, so any FM-1 firmware can
use them. M-VAVE and FM-1 are M-VAVE's names.
