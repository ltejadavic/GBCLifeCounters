# GBC Four-Player Commander MVP

This iteration of the four-player Commander MVP tracks independent life and
poison values plus commander damage separated by source commander. The overview
remains compact while A opens detail and commander-source screens.

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
- A: open the selected player's Life/Poison detail screen.
- Start: open the reset confirmation.
- A: confirm reset while the confirmation is open.
- B: cancel reset while the confirmation is open.

On the player detail screen:

- Up / Down: select Life or Poison.
- Left / Right: change the selected value using the displayed step.
- Select: cycle the adjustment step through 1, 5, and 10.
- A while `CMD MAX` is selected: open commander damage by source.
- B: return to the overview.

On the commander damage screen:

- Up / Down: select the source commander, P1 through P4.
- Left / Right: change damage from that commander to the target player.
- Select: cycle the adjustment step through 1, 5, and 10.
- B: return to player detail.

## Emulator test checklist

1. Boot in Game Boy Color mode and confirm four rows appear: P1 through P4,
   each at 40 life, poison `P  0`, and status `OK`.
2. Confirm `>` initially marks P1.
3. Press Down repeatedly and confirm selection moves P1, P2, P3, P4, then
   wraps to P1. Up must wrap in the opposite direction.
4. With P2 selected, press Left and Right. Only P2 may change.
5. Press Select and confirm the displayed step cycles `1`, `5`, `10`, `1`.
   Left and Right must use the displayed step.
6. Select P2 and press A. Confirm the detail title names P2, Life is selected,
   and Poison starts at 0.
7. Use Up/Down to switch between Life and Poison. Changes must affect only the
   selected field and only P2.
8. Attempt to decrease Poison below 0. It must remain at 0.
9. Set Poison to 8. The poison status and field must show a warning palette.
   Return with B; P2 must show `P  8` and `PSN` in the overview.
10. Reopen P2 detail and set Poison to 10. It must show possible loss without
    eliminating the player. The overview must show `LOSS` for P2.
11. Set P3 life to 5, then 0 and negative. It must still show `LOW`, then
    `LOSS`, without affecting poison or removing the player.
12. Press Start, then B. The reset prompt must close without changing values.
13. Press Start, then A. Every player must return to 40 life and 0 poison,
    selection to P1, and adjustment step to 1.
14. In P2 detail, select `CMD MAX` and press A. Confirm the title reads
    `CMD DAMAGE TO P2` and rows list sources P1 through P4.
15. Add 12 commander damage from P1 and 9 from P3. `CMD MAX` must be 12 and
    there must be no warning: damage from different commanders does not combine.
16. Raise P1's commander damage to 18. It must show `WARN`; after returning,
    P2 must show `CMD` in the overview.
17. Raise that same source to 21. It must show `LOSS` without automatically
    eliminating or hiding P2.
18. Reduce commander damage below zero. It must clamp to 0 without affecting
    other source rows.
19. Confirm reset restores all commander damage, poison, and life values.
20. Confirm the bottom line reads `CGB COLOR: ACTIVE`.

No input should alter more than one player or apply more than once per button
press.

## Current limits

The UI currently edits the main commander slot only. The second partner slot is
kept separate in the model and rules but is not exposed yet. Manual elimination,
winner detection, player setup, and sound are also not exposed yet.
