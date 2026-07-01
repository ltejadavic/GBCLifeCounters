# Commander GBC Life Counter

<p align="center">
  <strong>A Game Boy Color life counter and table assistant for Magic: The Gathering Commander.</strong>
</p>

<p align="center">
  Track life, poison, RAD counters, commander damage, Monarch, Initiative, eliminations, and commander identities for 2–8 players.
</p>

<p align="center">
  <img alt="Platform" src="https://img.shields.io/badge/platform-Game%20Boy%20Color-6a5acd">
  <img alt="Language" src="https://img.shields.io/badge/language-C%20%2F%20Python-blue">
  <img alt="Toolchain" src="https://img.shields.io/badge/toolchain-GBDK--2020-green">
  <img alt="Status" src="https://img.shields.io/badge/status-MVP-orange">
  <img alt="License" src="https://img.shields.io/badge/license-Fan%20Project-lightgrey">
</p>

---

## Overview

**Commander GBC Life Counter** is an open-source **Magic: The Gathering Commander assistant** built for the **Game Boy Color**.

It combines:

- a portable **Python rules reference engine**
- a playable **C / GBDK-2020 Game Boy Color ROM**
- an offline searchable commander database
- original pixel-art icons and portraits
- synthesized Game Boy audio

The project tracks **public game information** instead of trying to simulate every Magic rule. Possible losses are reported to players, but elimination always requires **manual confirmation**, because replacement effects and card-specific exceptions may change the result.

> This is a playable MVP, not yet a finished physical-cartridge release.

---

## Current Status

The repository currently contains:

| Component | Status |
|---|---|
| Python rules engine | Functional |
| Game Boy Color ROM | Functional MVP |
| Multiplayer support | 2–8 players |
| Commander search | Offline searchable database |
| Commander damage | Implemented |
| Poison counters | Implemented |
| RAD counters | Implemented |
| Monarch / Initiative | Implemented |
| Save/load | Not implemented yet |
| Physical cartridge validation | In progress |

---

## Implemented in the ROM

### Multiplayer Game State

- Configurable games for **2–8 players**
- Configurable starting life
- Independent life totals
- Negative life values supported
- Manual elimination and restoration
- Winner detection when exactly one player remains active
- Full game reset

### Commander Rules Support

- Commander damage tracked by:
  - target player
  - source player
  - commander slot
- Commander damage warning at **18**
- Possible commander-damage loss at **21**
- Commander damage also modifies the target player's life total
- Self commander damage is disabled
- Partner commander slots exist in the data model

### Counters and Statuses

- Poison counters
  - warning at **8**
  - possible loss at **10**
- RAD counters
  - range: **0–255**
  - custom nuclear pixel-art icon
- Monarch ownership
- Initiative ownership
- Automatic transfer or clearing of unique statuses

### Commander Selection

- Offline commander database
- Searchable directly on the Game Boy
- Commander archetypes
- 8×8 overview icons
- 24×24 player-detail portraits

### Presentation and Audio

- Original startup presentation
- `NOCHES DE COMMANDER PRESENTA` splash screen
- `DEVELOPED BY LTJD` credit screen
- Fantasy chiptune intro
- Gameplay sound effects
- Color and monochrome-compatible interface

---

## Screens and Interaction

The startup flow contains:

1. `NOCHES DE COMMANDER PRESENTA`
2. `DEVELOPED BY LTJD`
3. Game setup
4. Multiplayer overview
5. Player detail screens
6. Commander selection

---

## Multiplayer Overview

Each player is displayed using a compact two-line block.

| Line | Information |
|---|---|
| Main line | Player, life total, warning state, possible-loss state |
| Counter line | Poison, RAD, commander archetype, Monarch, Initiative |

---

## Player Detail Screen

The detail screen shows:

- selected commander
- life total
- poison counters
- RAD counters
- maximum commander damage received
- Monarch status
- Initiative status

A commander portrait is displayed only after a commander has been selected.

---

## Commander Database

The checked-in commander database was generated on **2026-06-30** and contains **269 unique searchable commanders**.

### Sources

The database includes:

- **171 unique face commanders** from MTGJSON Commander Deck products
- the current **top 100 commanders from Scryfall**, ordered by EDHREC rank

Duplicate records are merged.

### ROM Constraints

Each commander record stores:

- an ASCII display name
- a maximum display length of **28 characters**
- a compact primary-archetype identifier

Names are shortened for the Game Boy Color's **160×144** display.

---

## Commander Search

Search runs entirely offline on the Game Boy.

Supported characters:

- `A–Z`
- `0–9`
- spaces
- apostrophes
- hyphens
- commas

Matching ignores punctuation and ranks results in this order:

1. Full-name prefixes
2. Internal-word prefixes
3. Normalized substring matches

For Game Boy performance:

- sorted indexes and binary search handle prefix matching
- substring matching runs only as a fallback
- an incremental candidate cache is reused while the query grows
- names are stored in a compact string pool in **ROM bank 2**

---

## Rebuilding the Commander Index

Rebuild the ROM index from the existing JSON file without network access:

```bash
python3 tools/generate_commander_db.py --from-json
```

Refresh public metadata from online sources:

```bash
python3 tools/generate_commander_db.py
```

> Running the generator without `--from-json` requires internet access.

---

## Archetype Art

The current commander archetypes are:

| Archetype | Purpose |
|---|---|
| Control | Interaction and permission |
| Artifacts | Artifact-based strategies |
| Counters | +1/+1 counters and counter synergy |
| Enchantments | Enchantment-based engines |
| Graveyard | Recursion, self-mill, reanimation |
| Lands | Landfall and land-based value |
| Lifegain | Life gain and payoff engines |
| Sacrifice | Aristocrats and sacrifice loops |
| Spells | Instants, sorceries, and spell chains |
| Tokens | Creature token generation |
| Typal | Creature-type focused decks |
| Voltron | Commander combat damage strategies |

Each archetype has two original code-generated 2bpp assets:

| Asset | Size | Used in |
|---|---:|---|
| Miniature icon | 8×8 | Multiplayer overview |
| Portrait | 24×24 | Player detail screen |

The archetype graphics and RAD icon live in **ROM bank 3** and use background tile IDs **134–254**.

Regenerate them after changing their source drawings:

```bash
python3 tools/generate_archetype_assets.py
```

---

## Audio

Audio is generated directly with Game Boy pulse and noise channels.

The ROM does **not** embed external copyrighted samples.

### Included Audio

| Audio Type | Usage |
|---|---|
| Fantasy loop | Presentation screens only |
| Navigation effect | Menu movement |
| Confirmation effect | Accepted action |
| Cancellation effect | Back or cancel |
| Value-change effect | Life and counter changes |
| Possible-loss effect | Warning state |
| Elimination effect | Manual elimination |

Background music stops when gameplay begins to avoid unnecessary continuous sound output.

---

## Rules Behavior

### Life

| Rule | Value |
|---|---:|
| Default Commander starting life | 40 |
| Low-life warning | 5 or less |
| Possible-loss threshold | 0 or less |
| Negative life | Supported |
| Automatic elimination | No |

A possible loss does not automatically eliminate the player.

---

### Poison

| Rule | Value |
|---|---:|
| Minimum poison | 0 |
| Warning threshold | 8 |
| Possible-loss threshold | 10 |
| Automatic elimination | No |

---

### RAD

| Rule | Value |
|---|---:|
| Minimum RAD | 0 |
| Maximum RAD in ROM | 255 |
| Direct loss condition | None |

The current ROM tracks RAD counters but does not yet expose the Fallout mill/nonland resolution helper. Players can adjust resulting life manually.

---

### Commander Damage

Commander damage is tracked separately by:

- target player
- source player
- commander slot

Important behavior:

- damage from different commanders does **not** combine toward the 21-damage rule
- adding commander damage subtracts the same amount from life
- removing incorrectly entered commander damage restores that applied life
- partner slots exist in the data model
- the current ROM UI exposes commander slot `0`

---

### Monarch and Initiative

Each status has at most one owner.

Behavior:

- assigning a new owner removes it from the previous owner
- toggling the current owner clears the status
- eliminating the owner clears the status

---

### Elimination and Winner Detection

Warnings and possible losses are informational only.

| Event | Behavior |
|---|---|
| Possible loss | Shown as a warning |
| Elimination | Manual action |
| Restoration | Manual action |
| Winner | Shown only when one active player remains |

---

## Project Architecture

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

The Python rules engine remains the behavioral reference.

The GBC layer uses:

- simple structs
- fixed-size arrays
- integers
- explicit functions
- no dynamic allocation in the core game state

This keeps the rules predictable on constrained hardware.

---

## Requirements

### Python Prototype

- Python `3.10.9`
- `pytest`

### Game Boy Color ROM

- GNU Make
- GBDK-2020

The repository can install its local GBDK toolchain through the provided setup target.

---

## Build and Test

From the repository root:

```bash
make setup-toolchain
make verify
python3 -m pytest prototype_python/tests
```

`make verify` runs:

1. host C tests
2. ROM build
3. ROM header validation

Generated ROM:

```text
build/commander_gbc_multiplayer.gbc
```

Run the terminal reference client:

```bash
python3 prototype_python/main.py
```

The detailed emulator regression checklist is available in:

```text
docs/gbc_four_player.md
```

---

## GBC Controls

### Presentation and Setup

| Input | Action |
|---|---|
| Start | Continue from splash |
| A / Start | Continue from developer credit |
| Up / Down | Select setup field |
| Left / Right | Change player count or starting life |
| A | Start game |
| B | Restore setup defaults or cancel reconfiguration |

---

### Multiplayer Overview

| Input | Action |
|---|---|
| Up / Down | Select player |
| Left / Right | Change selected player's life |
| Select | Cycle adjustment step: 1, 5, 10 |
| A | Open player detail |
| Start | Open reset confirmation |

---

### Player Detail

| Input | Action |
|---|---|
| Up / Down | Select field |
| Left / Right | Edit Life, Poison, RAD, Monarch, or Initiative |
| Select | Cycle adjustment step: 1, 5, 10 |
| A on CMDR | Open commander search |
| A on CMD MAX | Open commander damage by source |
| A on status | Toggle Monarch or Initiative |
| Start | Eliminate or restore player with confirmation |
| B | Return to overview |

---

### Commander Search

| Input | Action |
|---|---|
| D-Pad | Move keyboard cursor or suggestion selection |
| A | Type character or choose focused suggestion |
| Start / Select | Switch keyboard/list focus |
| B | Delete, return to keyboard, or leave search |

---

## Technical Profile

| Item | Detail |
|---|---|
| Target | Game Boy Color |
| Fallback | Monochrome-compatible |
| Toolchain | GBDK-2020 / SDCC |
| Cartridge type | MBC5 |
| ROM size | 64 KiB |
| ROM layout | Four 16 KiB banks |
| Runtime network access | No |
| Dynamic allocation in core GBC state | No |
| Art | Original code-generated pixel art |
| Sound | Synthesized Game Boy audio |

The ROM uses separate banks for globally accessible logic, UI, database data, and artwork.

---

## Known Limitations

- No save/load support yet
- RAD resolution helper is not exposed in the ROM
- Partner commander damage and partner tax slots are modeled but not fully exposed in the UI
- Energy, Experience, Ticket, Commander Tax, Storm, and other advanced counters remain implemented primarily in the Python reference
- Commander archetypes are broad automatic classifications and may require manual overrides
- Commander names are shortened for the Game Boy Color display
- Emulator validation is further ahead than physical-cartridge testing

---

## Roadmap

Planned improvements include:

- RAD mill/nonland resolution helper
- Energy counters
- Experience counters
- Ticket counters
- Commander Tax screens
- Partner commander UI
- Additional status and counter icons
- Save/load investigation using cartridge SRAM
- More hardware testing
- UI and accessibility refinement
- Continued commander database improvements
- Manual archetype classification overrides

---

## Contributing

Issues and pull requests are welcome.

Contributions should preserve the main constraints of the project:

- keep rules separate from UI code
- add tests for behavioral changes
- avoid dynamic allocation in GBC core code
- avoid unnecessarily large lookup structures
- run `make verify` before submitting changes
- run the Python test suite before submitting changes
- do not add copyrighted card art
- do not add copyrighted music or sound samples

The extended product and rules planning reference is available in:

```text
CODEX_PLANNING.md
```

---

## Fan-Project Notice

**Commander GBC Life Counter** is an unofficial fan project.

Magic: The Gathering, Commander, Game Boy, and related names belong to their respective owners.

This repository:

- does not include card artwork
- is not affiliated with Wizards of the Coast
- is not endorsed by Wizards of the Coast
- is not affiliated with Nintendo
- is not endorsed by Nintendo

---

## Summary

Commander GBC Life Counter is a compact Commander table assistant for the Game Boy Color.

It is designed to be:

- playable around a real table
- readable on original hardware constraints
- rules-aware but manually controlled
- testable through a Python reference engine
- expandable with more counters, statuses, and commander features over time
