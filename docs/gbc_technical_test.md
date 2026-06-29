# GBC Technical Test

This is the first Game Boy Color build milestone from the project planning. It
validates the GBDK-2020 toolchain, CGB cartridge header, text output, joypad
input, frame loop, and signed life-total updates.

It is intentionally a one-player technical test, not the Commander rules engine
port and not the multiplayer MVP.

## Build

From the repository root:

```text
make setup-toolchain
make verify
```

The toolchain setup script downloads the pinned official GBDK-2020 4.5.0
archive for macOS or Linux and verifies its SHA-256 checksum. An existing GBDK
installation can be used instead:

```text
make verify GBDK_HOME=/absolute/path/to/gbdk
```

Generated ROM:

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
