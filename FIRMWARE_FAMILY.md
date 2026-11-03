# The FM-1's 25 most popular custom firmwares, and which come from Felucca

Researched 2026-11-03 for the "Skiff on a Big Screen" video (AIKellogg on YouTube). Corrections welcome. Popularity = GitHub stars that day (music firmwares;
not tools, emulators, hubs or games). Lineage = each firmware's own README (its "based on / fork of / built on"
lines), checked against the family sections of the community directory
[aroum/fm1-custom-fw](https://github.com/aroum/fm1-custom-fw) (about 200 firmwares listed).

| # | Firmware | Author | Stars | Comes from | How (its README) |
|---|---|---|---|---|---|
| 1 | SLOOP | isod89 | 728 | **Felucca** | "based on Felucca"; parent of its own family |
| 2 | **Felucca** | Leo Kuroshita (Hügelton) | 644 | **the parent** | began 2026-10-02 as FM-1 custom firmware |
| 3 | M-VAVE FM1 Open Firmware | M-VAVE | 296 | **not Felucca** | M-VAVE's own open source (V14) + SDK |
| 4 | X0X | Charles Vestal | 215 | **Felucca** | "a fork of Felucca" |
| 5 | JIANT FM | juanjiant | 204 | **Felucca** (+ SLOOP) | GitHub fork of Felucca, song system from SLOOP |
| 6 | Melodee | keremimo | 179 | **Felucca** | "a modified version of Felucca" |
| 7 | Skiff | illuminus5 | 132 | **Felucca** | "Built on Felucca" |
| 8 | AMB-1 | Charles Vestal | 110 | **Felucca** | fm1-base + felucca-bsp, from Felucca |
| 9 | ChoralRoot FM-1 | Quixotic7 | 97 | **Felucca** | built on Felucca |
| 10 | GHOULBOX 2.0 | Jason Persinger | 82 | **Felucca** | "Built on Felucca" (GitHub fork) |
| 11 | FoMni-1 | Charles Vestal | 74 | **Felucca** | Felucca's platform, by way of X0X |
| 12 | Lunar Modulator | ip2k | 63 | **not Felucca** | its own (from 2026-09-06); borrows 3 Felucca engines; not yet run on an FM-1 |
| 13 | LOOM-8 | Charles Vestal | 60 | **Felucca** | fm1-base + felucca-bsp, from Felucca |
| 14 | OberVave | zvenson | 49 | **Felucca** | Dinghy (Felucca's platform as SLOOP carries it) |
| 15 | Hortator | DEADACTIVE | 47 | **Felucca** | "from upstream Felucca" |
| 16 | ORBIT | dspaudio | 38 | **Felucca** | "Based on SLOOP/Felucca" |
| 17 | Jangada | zednaked | 37 | **Felucca** | "a fork of Felucca" |
| 18 | zp12 | zvenson | 35 | **Felucca** | from sloopDX / SLOOP |
| 19 | DUB-1 | sbegueria | 33 | **Felucca** | "a fork of AMB-1" |
| 20 | GR!MM | GRiMMBIENT | 32 | **Felucca** | Felucca's platform + Dinghy's template |
| 21 | Tonitor | Nuno Zimas | 31 | **Felucca** | "Based on Felucca": hal, loader, USB |
| 22 | FMini-1 | Guilherme Ruppert | 30 | **Felucca** | "based on the Felucca" |
| 23 | LOF-1 | RedtailWorks | 28 | **Felucca** | forked from AMB-1 |
| 24 | sloopDX | zvenson | 26 | **Felucca** | from SLOOP |
| 25 | MAKU | okumurari2 | 25 | **Felucca** | "built on Felucca" (GitHub fork) |

**23 of the top 25 come from Felucca**, directly or through a middle family. Two don't: M-VAVE's own open firmware
and Lunar Modulator.

## The family tree

```
Felucca (Leo Kuroshita, 2026-10-02)
├── directly: X0X, JIANT, Melodee, Skiff, ChoralRoot, GHOULBOX, Hortator, Jangada, Tonitor, FMini-1, MAKU
├── SLOOP ── ORBIT, sloopDX ── zp12
│        └── Dinghy (a template) ── OberVave, GR!MM
├── X0X ── FoMni
└── fm1-base / felucca-bsp ── AMB-1 ── DUB-1, LOF-1
                           └── LOOM-8
Not from Felucca: M-VAVE FM1 Open Firmware (M-VAVE), Lunar Modulator (ip2k)
```

Why they share it: Felucca's **platform** (the hardware layer, USB, the update loader, the installer, storage) is the
hard part of an FM-1 firmware, and it's open (GPL-3.0). So most firmwares keep it and write their own sounds and
screens on top. That's why the same Big Screen code (screen in `lcd.c`, hello in `usb.c`, a call in `main.c`) went
into ChoralRoot, JIANT and Skiff at the same places.

## Notes

- Stars change daily; this is one day's snapshot. The whole scene is about a month old (Felucca: 2026-10-02).
- The directory files JIANT under SLOOP's family: JIANT is a GitHub fork of Felucca whose song system comes from SLOOP.
- Polly (Charles Vestal, 25 stars, Felucca) ties MAKU at #25.
- Felucca's README: "an open-source synthesizer firmware project that began as custom firmware for the M-VAVE FM-1".

## The parent: Leo Kuroshita (Felucca)

From his GitHub profile (kurogedelic), the Hügelton organisation and the repositories' own READMEs, 2026-11-03:

- **Leo Kuroshita**, Kobe, Japan; **Hügelton Instruments** ("Electric products", GitHub organisation since 2020-07-25;
  also lists lunalargo). On GitHub since 2014.
- Before the FM-1, music tools for small machines: **Yacht** (a handheld music studio for the Playdate, 2025),
  **Btns** (a button board compatible with the monome grid, 2024), monome **norns** scripts (tokei, avonlea),
  **FamDrum** (a 16-step drum machine for the NES, 2026), **BerryMG** (an RP2354 cartridge for the Game Boy),
  **CrispyZebra** (a simulation of Casio's CZ synths), **Komyo** (a Buddhist chant-style vocal synth), MOD2 apps.
- **2026-09-30: FM-1 Transporter** — read and write the FM-1's flash from a computer through a small RP2040/RP2350
  board on the FM-1's USB lines ("three wires only").
- **2026-10-02: Felucca** published; first release v0.8-beta on 2026-10-03; 23 releases by 2026-11-03; v1.9.0
  (plugin engines) on 2026-11-01. 644 stars, 155 forks.
- Why it became the base (its READMEs): open source (GPL-3.0-only); installs from a **web page** in Chrome or Edge,
  "no extra hardware is needed"; a browser version to try it; the platform split out as **felucca-bsp**
  (2026-10-09, MPL-2.0: "the chip layer (clock, interrupts, flash, USB, UART, timers, boot entry) ... no sound code and
  no user interface"), the engines as **Felucca-Engines** (MIT); **plugins** from 1.9: "sound engines that anyone can
  make and install without building a firmware". Charles Vestal's fm1-base builds on felucca-bsp (AMB-1, LOOM-8).
- Since then also **Pendel** (2026-11-02): firmware for the M-VAVE Pocket AMP, "sibling of Felucca".
