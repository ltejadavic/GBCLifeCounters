# GBC One-Player Counter

This historical iteration replaced the monolithic technical test with the
first structured GBDK application. The ROM still exposes one player, while the
underlying model uses the planned fixed-size structures for up to eight
players.

The active build has moved to the four-player overview documented in
`docs/gbc_four_player.md`.

## Build and verify

From the repository root:

```text
make setup-toolchain
make all
```

`make all` runs the host-side C core tests, compiles the CGB ROM, and validates
the cartridge header and checksums.

Generated ROM:

```text
build/commander_gbc_one_player.gbc
```

## Emulator test checklist

1. Boot the ROM in Game Boy Color mode.
2. Confirm the title reads `COMMANDER GBC` and `1 PLAYER COUNTER`.
3. Confirm the bottom diagnostic reads `CGB COLOR: ACTIVE`. If it reads `OFF`,
   configure the emulator to use a Game Boy Color model before continuing.
4. Confirm the player is `P1` with no formatting characters around it, life
   starts at `40`, and status is `OK`.
5. Press Left and Right once. Life must change by -1 and +1 exactly once.
6. Press Down and Up once. Life must change by -5 and +5 exactly once.
7. Press B and A once. Life must change by -10 and +10 exactly once.
8. Reduce life to `5`. Status must become `LOW LIFE` and the palette must turn
   amber.
9. Reduce life to `0`, then below zero. Status must become `MAY LOSE`, the
   palette must turn red, and the negative value must remain visible.
10. Press Start. Life must return to `40`, status to `OK`, and the palette to
    green.

When testing transitions such as `10` to `5`, `5` to `0`, positive to negative,
or any value back to `40`, no digits from the previous value may remain on
screen.

If `CGB COLOR: ACTIVE` appears but all three states remain monochrome, check the
emulator's video settings for a forced DMG/monochrome palette.

The ROM must not freeze, apply repeated changes from one quick press, or mark
the player eliminated automatically.

## Current scope

Implemented in C:

- Fixed-size `Player` and `GameState` structures prepared for eight players.
- Reset logic for player state, commander damage, turn, and global values.
- Validated signed life actions with 16-bit saturation.
- Life warning at 5 or less.
- Possible loss at 0 or less without automatic elimination.
- Separate GBC data, action, rule, and UI modules.

Not exposed yet:

- Multiple players.
- Poison or commander-damage editing.
- Manual elimination and winner flow.
- Sound effects.
