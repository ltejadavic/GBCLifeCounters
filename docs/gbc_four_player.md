# GBC Multiplayer Commander MVP

This iteration supports two through eight players with configurable starting
life. It tracks independent life and poison values plus commander damage
separated by source commander.

## Build and verify

From the repository root:

```text
make setup-toolchain
make all
```

Generated ROM:

```text
build/commander_gbc_multiplayer.gbc
```

## Controls

On the setup screen:

- Up / Down: select player count or starting life.
- Left / Right: set 2–8 players or adjust starting life by 5.
- A: start the configured game.
- B: restore setup defaults of four players and 40 life.

- Up / Down: select the previous or next player, wrapping across all configured
  players. Lists scroll four rows at a time.
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
- Start: request manual elimination, or restoration if the player is already
  eliminated.
- B: return to the overview.

On the commander damage screen:

- Up / Down: select the source commander across all configured players.
- Left / Right: change damage from that commander to the target player.
- Select: cycle the adjustment step through 1, 5, and 10.
- B: return to player detail.

Commander damage entered on this screen represents normal commander combat
damage: adding it also subtracts the same amount from life. Removing a tracked
amount restores that applied life delta as an input correction. Infect and
other replacement-effect helpers are not part of this screen yet.

## Test 8: setup and 2–8 players

1. Boot and confirm `GAME SETUP` defaults to four players and 40 life.
2. Verify player count cannot go below 2 or above 8.
3. Verify starting life changes by 5 and stays between 5 and 100.
4. Change both values, press B, and confirm defaults return to 4 and 40.
5. Configure six players with 30 starting life and press A.
6. Confirm the overview shows `PLAYERS 1-4 OF 6`, with P1–P4 at 30.
7. Move down to P5 and P6. The list must scroll and keep `>` on the selected
   player. Moving past P6 must wrap to P1 and restore the first window.
8. Change P6 life and poison. P1–P5 must remain unchanged.
9. Open P6 commander damage and move through sources P1–P6. The source list
   must scroll without corrupting rows or values.
10. Add commander damage from P5 to P6, return to overview, and confirm P6 keeps
    the correct status.
11. Confirm reset restores six players to 30 life and zero counters; it must not
    revert to four players or 40 life.
12. Restart the ROM, configure eight players, and confirm navigation reaches P8
    and wraps correctly.

## Test 9: commander life coupling and setup return

1. In a six-player game, confirm the header reads exactly `PLAYERS 1-4 OF 6`.
   After scrolling it must show valid single-digit ranges without extra numbers.
2. Reduce P1 from 40 to 37 using normal life editing.
3. Record 5 commander damage to P1 from P2. The tracked commander value must be
   5 and P1 life must become 32.
4. Remove 2 commander damage. The tracked value must become 3 and life 34.
   Remove more than remains: commander damage must clamp to 0 and life return
   only to 37, not above it.
5. From overview press Start, then Select. Setup must reopen with the current
   player count and starting life.
6. Change setup values, then press B. The existing game and all its values must
   return unchanged.
7. Reopen Setup and press A after changing values. A fresh game must start with
   the newly selected player count and starting life.

## Full regression checklist

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
20. Put P1 at a possible loss value and confirm it remains active until manually
    eliminated.
21. In P1 detail press Start, then B. The prompt must cancel without changing
    P1. Repeat with Start, then A: P1 must remain visible as `OUT`.
22. Try to edit an `OUT` player from overview and detail. Its values must not
    change.
23. Open the `OUT` player's detail, press Start, then A. The player must be
    restored with its previous values.
24. Eliminate P1, P2, and P3. The overview must show `WINNER: P4`.
25. Restore P2. The winner label must disappear because two players are active.
26. Eliminate all players. No winner may be shown when zero players are active.
27. Confirm reset restores all players and removes every `OUT` and winner label.
28. Confirm the bottom line reads `CGB COLOR: ACTIVE`.

No input should alter more than one player or apply more than once per button
press.

## Current limits

The UI currently edits the main commander slot only. The second partner slot is
kept separate in the model and rules but is not exposed yet. Sound is also not
exposed yet.
