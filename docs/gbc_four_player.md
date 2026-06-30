# GBC Four-Player Life Overview

This is the first testable iteration of the four-player Commander MVP. It
validates that four independent players fit on screen and can be selected,
edited, warned, and reset safely before poison, commander damage, elimination,
and winner flows are added.

## Build and verify

From the repository root:

```text
make setup-toolchain
make all
```

Generated ROM:

```text
build/commander_gbc_four_player.gbc
```

## Controls

- Up / Down: select the previous or next player, wrapping between P1 and P4.
- Left / Right: decrease or increase the selected player's life.
- Select: cycle the life adjustment step through 1, 5, and 10.
- Start: open the reset confirmation.
- A: confirm reset while the confirmation is open.
- B: cancel reset while the confirmation is open.

## Emulator test checklist

1. Boot in Game Boy Color mode and confirm four rows appear: P1 through P4,
   each at 40 life with status `OK`.
2. Confirm `>` initially marks P1.
3. Press Down repeatedly and confirm selection moves P1, P2, P3, P4, then
   wraps to P1. Up must wrap in the opposite direction.
4. With P2 selected, press Left and Right. Only P2 may change.
5. Press Select and confirm the displayed step cycles `1`, `5`, `10`, `1`.
   Left and Right must use the displayed step.
6. Set P2 to 5. Its row must show `LOW` with the warning palette while the
   other rows remain normal.
7. Set P3 to 0 and then negative. Its row must show `LOSS` with the loss
   palette, retain negative life, and not disappear.
8. Press Start, then B. The reset prompt must close without changing values.
9. Press Start, then A. Every player must return to 40, selection to P1, and
   adjustment step to 1.
10. Confirm the bottom line reads `CGB COLOR: ACTIVE`.

No input should alter more than one player or apply more than once per button
press.

## Current limits

This iteration tracks life only. Poison, commander damage, manual elimination,
winner detection, player setup, and sound are not exposed in the ROM yet.
