# GBC Technical Test

This was the first Game Boy Color build milestone from the project planning. It
validates the GBDK-2020 toolchain, CGB cartridge header, text output, joypad
input, frame loop, and signed life-total updates.

The active build has moved to the structured one-player counter documented in
`docs/gbc_one_player.md`.

It is intentionally a one-player technical test, not the Commander rules engine
port and not the multiplayer MVP.

## Historical milestone

The active `Makefile` now builds the one-player iteration. This technical-test
source remains available in Git history; use `docs/gbc_one_player.md` for the
current build and verification commands.

Historical ROM name:

```text
build/commander_gbc_technical_test.gbc
```

## Controls

- Left / Right: decrease / increase life by 1
- Down / Up: decrease / increase life by 5
- B / A: decrease / increase life by 10
- Start: reset life to 40

Life is displayed from -999 through 999 so negative-life rendering can be
tested. Input is edge-triggered: release a button before applying the same
change again.

## Expected result

The ROM should boot in a Game Boy Color emulator or compatible flash cartridge,
show `COMMANDER GBC`, start at 40 life, respond once per button press, and reset
to 40 with Start.
