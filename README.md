# Commander GBC Life Counter

Commander GBC Life Counter is an open-source Magic: The Gathering Commander
assistant built for Game Boy Color. It combines a portable Python rules engine
with a playable C/GBDK-2020 ROM designed for two to eight players around a real
table.

The project tracks public game information instead of attempting to simulate
every Magic rule. Possible losses are reported to the players, but elimination
always requires manual confirmation because replacement effects and card-specific
exceptions may change the result.

## Current status

The repository currently contains a functional multiplayer Game Boy Color MVP
and its Python reference implementation.

Implemented in the ROM:

- Configurable games for 2–8 players and starting life.
- Independent life totals, including negative values.
- Poison counters with warnings at 8 and possible loss at 10.
- RAD counters from 0–255 with a dedicated nuclear pixel-art icon.
- Commander damage tracked separately by source commander.
- Commander-damage warning at 18 and possible loss at 21.
- Commander damage also changes the target player's life total.
- Self commander damage is disabled.
- Unique Monarch and Initiative ownership with automatic transfer or clearing.
- Manual elimination, restoration, winner detection, and complete game reset.
- Commander selection from an offline searchable database.
- Twelve commander archetypes with 8×8 overview icons and 24×24 portraits.
- Original startup presentation, fantasy chiptune, and gameplay sound effects.
- Color and monochrome-compatible interface.

The ROM is playable in a Game Boy Color emulator. It is still an evolving MVP,
not a finished physical-cartridge release.

## Screens and interaction

The startup flow contains:

1. `NOCHES DE COMMANDER PRESENTA` splash screen.
2. `DEVELOPED BY LTJD` credit screen.
3. Game setup for player count and starting life.
4. Multiplayer overview.
5. Per-player details and commander selection.

The overview gives each player a compact two-line block:

- Main line: player, life total, and warning/loss state.
- Counter line: poison, RAD, commander archetype, Monarch, and Initiative.

The player detail screen contains commander, life, poison, RAD, maximum
commander damage received, Monarch, and Initiative. A commander portrait is
only displayed after a commander has been selected.

## Commander database and search

The checked-in database was generated on 2026-06-30 and contains 269 unique
searchable commanders:

- 171 unique face commanders from MTGJSON Commander Deck products.
- The current top 100 commanders from Scryfall ordered by EDHREC rank.
- Duplicate records are merged.
- Display names are ASCII and limited to 28 characters for the ROM.
- Each record stores a compact primary-archetype identifier.

Search runs entirely offline on the Game Boy. It supports A–Z, 0–9, spaces,
apostrophes, hyphens, and commas. Matching ignores punctuation and ranks:

1. Full-name prefixes.
2. Internal-word prefixes.
3. Normalized substring matches.

For Game Boy performance, sorted indexes and binary search handle the first two
categories. Substring matching only runs as a fallback and reuses an
incremental candidate cache while the query grows. Names are stored in a
compact string pool in ROM bank 2.

Rebuild the ROM index from the existing JSON file without network access:

```sh
python3 tools/generate_commander_db.py --from-json
```

Running the generator without `--from-json` refreshes public metadata and
therefore requires internet access.

## Archetype art

The current archetypes are:

- Control
- Artifacts
- Counters
- Enchantments
- Graveyard
- Lands
- Lifegain
- Sacrifice
- Spells
- Tokens
- Typal
- Voltron

Every archetype has two original code-generated 2bpp assets:

- An 8×8 miniature for the multiplayer overview.
- A more detailed four-tone 24×24 portrait for player detail.

The archetype graphics and RAD icon live in ROM bank 3 and use background tile
IDs 134–254. Regenerate them after changing their source drawings:

```sh
python3 tools/generate_archetype_assets.py
```

## Audio

Audio is generated directly with Game Boy pulse and noise channels; the ROM
does not embed external copyrighted samples.

- A quiet fantasy loop plays only on the two presentation screens.
- Gameplay has short effects for navigation, confirmation, cancellation,
  value changes, possible loss, and elimination.
- Background music stops when gameplay begins to avoid unnecessary continuous
  sound output.

## Rules behavior

### Life

- Default Commander starting life: 40.
- Life may go below zero.
- Life at 5 or less produces a warning.
- Life at 0 or less produces a possible-loss state.
- A possible loss does not automatically eliminate the player.

### Poison

- Poison cannot go below zero.
- Warning threshold: 8.
- Possible-loss threshold: 10.

### RAD

- RAD cannot go below zero and is capped at 255 in the ROM.
- RAD has no direct loss condition.
- The current ROM tracks the counters but does not yet expose the Fallout
  mill/nonland resolution helper. Players can adjust resulting life manually.

### Commander damage

- Damage is tracked by target player, source player, and commander slot.
- Damage from different commanders never combines toward the 21-damage rule.
- Adding commander damage subtracts the same amount from life.
- Removing incorrectly entered commander damage restores that applied life.
- Partner slots exist in the data model; the current ROM UI exposes slot 0.

### Monarch and Initiative

- Each status has at most one owner.
- Assigning a new owner removes it from the previous owner.
- Toggling the current owner clears the status.
- Eliminating the owner clears the status.

### Elimination and winner

- Warnings and possible losses are informational.
- Elimination and restoration are manual actions.
- A winner is shown only when exactly one player remains active.

## Project architecture

```text
prototype_python/
├── actions.py          # Portable game actions
├── constants.py        # Rules and limits
├── game_state.py       # Complete game model
├── player.py           # Player model
├── rules.py            # Warning/loss/winner checks
├── ui_terminal.py      # Thin terminal prototype
└── tests/              # pytest rules coverage

gbc_src/
├── actions.c           # GBC game mutations
├── game_state.c        # Fixed-size multiplayer state
├── rules.c             # GBC warning/loss checks
├── navigation.c        # Menu and scrolling behavior
├── ui.c                # Tile-based GBC interface
├── commander_db.c      # Indexed offline commander search
├── archetype_assets.c  # Banked 2bpp artwork loader
├── sound.c             # Chiptune and feedback effects
└── main.c              # Screen state and input loop

data/                   # Checked-in commander metadata
tools/                  # Database, art, ROM, and toolchain utilities
gbc_tests/              # Host-compiled C core tests
docs/                   # Emulator test checklists and implementation notes
```

The Python rules engine remains the behavioral reference. The GBC layer uses
simple structs, fixed-size arrays, integers, and explicit functions so the
rules stay predictable on constrained hardware.

## Requirements

Python prototype:

- Python 3.10.9
- pytest

Game Boy Color ROM:

- GNU Make
- GBDK-2020

The repository can install its local GBDK toolchain through the provided setup
target.

## Build and test

From the repository root:

```sh
make setup-toolchain
make verify
python3 -m pytest prototype_python/tests
```

`make verify` runs the host C tests, builds the ROM, and validates its header.

Generated ROM:

```text
build/commander_gbc_multiplayer.gbc
```

Run the terminal reference client:

```sh
python3 prototype_python/main.py
```

The detailed emulator regression checklist is available in
[`docs/gbc_four_player.md`](docs/gbc_four_player.md).

## GBC controls

### Presentation and setup

| Input | Action |
| --- | --- |
| Start | Continue from splash |
| A / Start | Continue from developer credit |
| Up / Down | Select setup field |
| Left / Right | Change player count or starting life |
| A | Start game |
| B | Restore setup defaults or cancel reconfiguration |

### Multiplayer overview

| Input | Action |
| --- | --- |
| Up / Down | Select player |
| Left / Right | Change selected player's life |
| Select | Cycle adjustment step: 1, 5, 10 |
| A | Open player detail |
| Start | Open reset confirmation |

### Player detail

| Input | Action |
| --- | --- |
| Up / Down | Select field |
| Left / Right | Edit Life, Poison, RAD, Monarch, or Initiative |
| Select | Cycle adjustment step: 1, 5, 10 |
| A on CMDR | Open commander search |
| A on CMD MAX | Open commander damage by source |
| A on status | Toggle Monarch or Initiative |
| Start | Eliminate or restore player with confirmation |
| B | Return to overview |

### Commander search

| Input | Action |
| --- | --- |
| D-Pad | Move keyboard cursor or suggestion selection |
| A | Type character or choose focused suggestion |
| Start / Select | Switch keyboard/list focus |
| B | Delete, return to keyboard, or leave search |

## Technical profile

- Target: Game Boy Color with monochrome fallback.
- Toolchain: GBDK-2020 / SDCC.
- Cartridge type: MBC5.
- ROM size: 64 KiB across four 16 KiB banks.
- Fixed ROM bank reserved for globally accessible logic.
- UI, database, and artwork separated across banks.
- No dynamic allocation in core GBC game state.
- No runtime network access.
- Original code-generated pixel art and synthesized sound.

## Known limitations

- No save/load support yet.
- RAD resolution helper is not exposed in the ROM.
- Partner commander damage/tax slots are modeled but not fully exposed in UI.
- Energy, Experience, Ticket, Commander Tax, Storm, and other advanced counters
  remain implemented primarily in the Python reference.
- Commander archetypes are broad automatic classifications and may require
  manual overrides.
- Commander names are shortened for the 160×144 display.
- Emulator validation is further ahead than physical cartridge testing.

## Roadmap

- RAD mill/nonland resolution helper.
- Energy, Experience, Ticket, and Commander Tax screens.
- Partner commander UI.
- Additional status and counter icons.
- Save/load investigation using cartridge SRAM.
- More hardware testing and UI/accessibility refinement.
- Continued commander database and archetype classification improvements.

## Contributing

Issues and pull requests are welcome. Contributions should preserve the main
constraints of the project:

- Keep rules separate from UI code.
- Add tests for behavioral changes.
- Avoid dynamic allocation and unnecessarily large lookup structures in GBC
  code.
- Run `make verify` and the Python test suite before submitting changes.
- Do not add copyrighted card art, music, or sound samples.

`CODEX_PLANNING.md` contains the extended product and rules planning reference.

## Fan-project notice

Commander GBC Life Counter is an unofficial fan project. Magic: The Gathering,
Commander, Game Boy, and related names belong to their respective owners. This
repository does not include card artwork and is not affiliated with or endorsed
by Wizards of the Coast or Nintendo.
