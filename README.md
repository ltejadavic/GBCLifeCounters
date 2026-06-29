Commander GBC Life Counter

A retro-style Magic: The Gathering Commander life counter and game-state tracker designed for Game Boy Color.

This project is intended to track not only life totals, but also important Commander-specific information such as commander damage, poison counters, rad counters, energy, experience counters, commander tax, monarch, initiative, storm count, turn count, and other optional game elements.

The project will first be developed as a Python 3.10.9 prototype to validate the rules, data model, and user flow. After the logic is stable, the final version will be ported to Game Boy Color, most likely using C with GBDK-2020.

⸻

Project Status

Current stage:

GBC technical test ROM implemented after the completed Python prototype

Planned development order:

1. Python rules engine
2. Python automated tests
3. Python UI mockup
4. Game Boy Color technical test
5. One-player GBC life counter
6. Four-player Commander MVP
7. More-than-four-player support
8. Advanced counters and statuses
9. Icon system and UI polish
10. Save/load investigation

Running the Python Prototype

Requirements:

* Python 3.10.9
* pytest (tests only)

Run the automated tests from the repository root:

```text
python -m pytest prototype_python/tests
```

Run the terminal prototype from the repository root:

```text
python prototype_python/main.py
```

The Python rules engine is the source of truth. The terminal program is a thin
interactive client over that engine. The multiplayer rules engine has not yet
been ported to C/GBDK.

Game Boy Color Technical Test

The first C/GBDK milestone is an interactive one-player life display. It proves
that the toolchain, CGB cartridge header, text rendering, joypad input, frame
loop, signed life values, and reset control work before the multiplayer port.

Build and verify it from the repository root:

```text
make setup-toolchain
make verify
```

Generated ROM:

```text
build/commander_gbc_technical_test.gbc
```

Controls:

* Left / Right: -1 / +1 life
* Down / Up: -5 / +5 life
* B / A: -10 / +10 life
* Start: reset to 40 life

See `docs/gbc_technical_test.md` for build details and expected behavior.

⸻

Important Technical Note

Game Boy Color does not run Python directly.

Python will be used for:

* Prototyping the game logic.
* Testing Commander rules.
* Designing the data model.
* Simulating menus and screen flow.
* Creating automated tests before porting.

The final Game Boy Color ROM will be implemented separately in C, using a Game Boy development toolkit such as GBDK-2020.

Recommended architecture:

Python 3.10.9 = prototype, testing, logic validation
C + GBDK-2020 = final Game Boy Color ROM

⸻

Project Goals

The goal is to create a compact Commander assistant that can be used during real multiplayer Magic games.

The application should allow players to:

* Track life totals.
* Support more than 4 players.
* Add, remove, rename, or reorder players before starting a game.
* Track commander damage from each commander to each player.
* Track poison counters.
* Track rad counters.
* Track energy counters.
* Track experience counters.
* Track commander tax.
* Track monarch and initiative.
* Track storm count and turn count.
* Track optional custom counters.
* Detect common possible loss conditions.
* Confirm eliminations manually.
* Detect the winner when only one player remains.
* Use a readable Game Boy Color-style interface.

⸻

Core Commander Features

Life Tracking

Default Commander life total:

40

Supported actions:

* Increase life.
* Decrease life.
* Set exact life.
* Reset life.
* Allow negative life totals.
* Warn when life is low.
* Trigger possible loss warning at 0 or less life.

Important:

The app should not automatically eliminate a player without confirmation, because Magic has many effects that can replace, prevent, or modify loss conditions.

⸻

Commander Damage

Commander damage is one of the most important Commander-specific features.

A player can lose if they receive:

21 or more combat damage from the same commander

The app must track commander damage separately by source.

Example:

P1 Commander dealt 12 damage to P2.
P3 Commander dealt 9 damage to P2.
P2 does not lose, because commander damage from different commanders does not combine.

Required behavior:

* Track damage from each commander to each player.
* Do not combine damage from different commanders.
* Support warnings at 18 or more commander damage.
* Trigger possible loss warning at 21 or more commander damage.
* Support partner commanders as an advanced feature.

Recommended data model:

commander_damage[target_player][source_player][commander_slot]

⸻

Poison Counters

Default poison loss threshold:

10 poison counters

Required behavior:

* Add poison counters.
* Remove poison counters.
* Set exact poison value.
* Warn at 8 or more poison counters.
* Trigger possible loss warning at 10 or more poison counters.

⸻

Rad Counters

Rad counters are player counters used in Magic: The Gathering — Fallout.

Required behavior:

* Add rad counters.
* Remove rad counters.
* Set exact rad value.
* Show rad counter icon if a player has rad counters.
* Include an optional Rad Helper screen.

Suggested Rad Helper flow:

1. Select player.
2. Show current rad counter count.
3. User physically mills cards.
4. User enters number of nonland cards milled.
5. App subtracts that much life.
6. App removes that many rad counters.
7. App checks possible loss conditions.

Example:

Player has 5 rad counters.
Player mills 5 cards.
3 are nonland cards.
Result:
-3 life
-3 rad counters

The app should not try to identify card types automatically.

⸻

Energy Counters

Energy is a player counter and resource.

Required behavior:

* Add energy.
* Spend energy.
* Set exact energy value.
* Clear energy.
* No default win/loss condition.

⸻

Experience Counters

Experience counters are player counters used by several Commander cards.

Required behavior:

* Add experience counters.
* Remove experience counters.
* Set exact experience value.
* No default win/loss condition.

⸻

Commander Tax

Commander tax is not an official counter, but it is very useful to track.

Required behavior:

* Track commander cast count.
* Track commander tax.
* Increase tax by 2 for each additional cast from the command zone.
* Allow manual override.
* Support partner commanders separately in advanced mode.

Example:

First cast: +0 tax
Second cast: +2 tax
Third cast: +4 tax
Fourth cast: +6 tax

⸻

Monarch

The app should support monarch tracking.

Behavior:

* Only one player can be monarch.
* Monarch can be set to none.
* Show crown icon next to the monarch player.

⸻

Initiative

The app should support initiative tracking.

Behavior:

* Only one player can have the initiative.
* Initiative can be set to none.
* Show torch or dungeon icon next to the player with initiative.

⸻

Storm Count

Storm count is a global turn-based value.

Behavior:

* Increase storm count.
* Decrease storm count.
* Reset manually.
* Optional reset when the turn changes.

⸻

Turn Counter

The app should track:

* Current turn number.
* Active player.
* Optionally rotate active player automatically.

⸻

Additional Optional Features

These features are not required for the MVP but should be considered in later versions.

Player Counters

Optional player counters:

Ticket counters
Custom counter 1
Custom counter 2
Custom counter 3
Custom counter 4

Player Statuses

Optional statuses:

City’s Blessing
Ring-bearer
Ring temptation level
Emblem count
Dungeon progress

Token Helpers

Optional token-count helpers:

Treasure
Clue
Food
Blood
Map
Junk

These should be treated as helper values, not as player counters.

Advanced Game Modes

Possible future modes:

Partner commanders
Background commanders
Doctor’s Companion-style commanders
Planechase
Archenemy
Two-Headed Giant
Infect/Toxic helper
Floating mana tracker
Dice roller
Coin flip
Mulligan tracker
Save/load

⸻

Supported Player Count

The app should not be limited to exactly 4 players.

Target support:

Minimum players: 2
Default players: 4
Target maximum: 8
Stretch maximum: 10

For more than 4 players, the Game Boy Color screen should use a scrollable player list instead of trying to display everyone at once.

Example:

> P1  40
  P2  38
  P3  25
  P4  41

Scroll down:

> P5  40
  P6  40
  P7  40
  P8  40

Implementation rule:

Never assume player_count == 4.
All loops must use the current player_count.
All arrays should support MAX_PLAYERS.

⸻

User Interface Design

The final interface should feel like a native Game Boy Color utility.

Design priorities:

* Readable numbers.
* Fast controls.
* Simple pixel icons.
* Minimal text.
* Clear warnings.
* No unnecessary animations.
* Works on a 160x144 screen.
* Usable with only D-Pad, A, B, Start, and Select.

⸻

Main Screens

Main Overview Screen

Purpose:

Show the current game state at a glance.

Should display:

* Player names.
* Life totals.
* Important warning icons.
* Monarch holder.
* Initiative holder.
* Eliminated players.

Example:

P1  40     P2  38
P3  25     P4  41

For more than 4 players, use scrolling.

⸻

Player Detail Screen

Purpose:

Show detailed information for one player.

Example:

PLAYER 1
LIFE: 40
PSN : 0
RAD : 2
ENE : 5
EXP : 1
TAX : 2
A: EDIT
B: BACK
START: MORE

⸻

Life Adjustment Screen

Purpose:

Quickly change a player’s life total.

Actions:

+1
-1
+5
-5
+10
-10
Set value
Reset

⸻

Commander Damage Screen

Purpose:

Show commander damage received by one player.

Example:

CMD DAMAGE TO P1
From P2: 0
From P3: 12
From P4: 21 !

Actions:

* Add commander damage.
* Remove commander damage.
* Set exact commander damage.
* Switch commander slot if partner mode is enabled.

⸻

Counter Screen

Purpose:

Edit player counters.

Counters:

Poison
Rad
Energy
Experience
Tickets
Custom counters

⸻

Status Screen

Purpose:

Edit non-numeric or unique statuses.

Statuses:

Monarch
Initiative
City’s Blessing
Ring
Day/Night
Emblems

⸻

Global Game State Screen

Purpose:

Track values that affect the whole game.

Values:

Turn number
Active player
Storm count
Monarch player
Initiative player
Day/Night state

⸻

Settings Screen

Purpose:

Configure the game before or during play.

Settings:

Starting life
Player count
Poison threshold
Commander damage threshold
Partner commander mode
Confirm eliminations
Autosave, if implemented

⸻

Controls

Recommended controls:

D-Pad:
- Navigate menus
- Select players
- Increase/decrease values
A:
- Confirm
- Open selected item
B:
- Cancel
- Go back
Start:
- Open main menu or settings
Select:
- Quick actions or switch tab

Optional shortcuts:

Left/Right:
- Change selected value by 1
Hold A + Left/Right:
- Change selected value by 5
Hold B + Left/Right:
- Change selected value by 10
Start + Select:
- Open reset confirmation

⸻

Icon System

The final GBC version should use tile-based icons, not Unicode symbols.

Recommended icon size:

8x8 pixels

Optional large icons:

16x16 pixels

Icons must be readable in monochrome and with limited Game Boy Color palettes.

Recommended icons:

LIFE      HP   Heart
COMMANDER CMD  Sword
POISON    PSN  Skull
RAD       RAD  Radiation symbol
ENERGY    ENE  Lightning bolt
EXP       EXP  Star or medal
TICKET    TIX  Ticket stub
TAX       TAX  Crown plus coin
MONARCH   MON  Crown
INIT      INI  Torch
STORM     STM  Swirl
TURN      TRN  Circular arrow
ACTIVE    ACT  Flag or arrow
DEAD      OUT  X or tombstone
BLESSING  ASC  Sunburst
DAY       DAY  Sun
NIGHT     NGT  Moon
RING      RNG  Ring
DUNGEON   DNG  Door
EMBLEM    EMB  Flag or badge
TREASURE  TRS  Gem
CLUE      CLU  Magnifying glass
FOOD      FOD  Apple
BLOOD     BLD  Drop
MAP       MAP  Folded map
JUNK      JNK  Scrap
MANA      MNA  Diamond

⸻

Data Model

Python Prototype

The Python prototype should use simple structures that can later be ported to C.

Example:

class Player:
    def __init__(self, player_id: int, name: str, starting_life: int = 40):
        self.player_id = player_id
        self.name = name
        self.life = starting_life
        self.poison = 0
        self.rad = 0
        self.energy = 0
        self.experience = 0
        self.ticket = 0
        self.eliminated = False
        self.commander_tax = [0, 0]
        self.commander_cast_count = [0, 0]
        self.custom_counters = [0, 0, 0, 0]
        self.status_flags = set()
class GameState:
    def __init__(self, player_count: int = 4, starting_life: int = 40):
        self.player_count = player_count
        self.starting_life = starting_life
        self.players = [
            Player(i, f"P{i + 1}", starting_life)
            for i in range(player_count)
        ]
        self.poison_threshold = 10
        self.commander_damage_threshold = 21
        self.partner_mode_enabled = False
        # commander_damage[target][source][commander_slot]
        self.commander_damage = [
            [
                [0, 0]
                for _ in range(player_count)
            ]
            for _ in range(player_count)
        ]
        self.turn_number = 1
        self.active_player = 0
        self.monarch_player = None
        self.initiative_player = None
        self.storm_count = 0
        self.day_night_state = None

⸻

C / GBC-Oriented Data Model

The final GBC version should use fixed-size arrays.

Recommended constants:

#define MAX_PLAYERS 8
#define MAX_COMMANDERS_PER_PLAYER 2
#define MAX_CUSTOM_COUNTERS 4
#define MAX_NAME_LENGTH 8

Recommended structure:

typedef struct {
    uint8_t id;
    char name[MAX_NAME_LENGTH];
    int16_t life;
    uint8_t poison;
    uint8_t rad;
    uint8_t energy;
    uint8_t experience;
    uint8_t ticket;
    uint8_t commander_tax[MAX_COMMANDERS_PER_PLAYER];
    uint8_t commander_cast_count[MAX_COMMANDERS_PER_PLAYER];
    uint8_t custom_counter_values[MAX_CUSTOM_COUNTERS];
    uint8_t custom_counter_types[MAX_CUSTOM_COUNTERS];
    uint8_t eliminated;
    uint8_t has_citys_blessing;
    uint8_t ring_level;
    uint8_t emblem_count;
} Player;

Commander damage:

uint8_t commander_damage[MAX_PLAYERS][MAX_PLAYERS][MAX_COMMANDERS_PER_PLAYER];

Game state:

typedef struct {
    uint8_t player_count;
    uint8_t starting_life;
    uint8_t poison_threshold;
    uint8_t commander_damage_threshold;
    uint8_t active_player;
    uint16_t turn_number;
    uint8_t storm_count;
    int8_t monarch_player;
    int8_t initiative_player;
    uint8_t partner_mode_enabled;
    uint8_t confirm_eliminations;
    uint8_t day_night_state;
} GameState;

⸻

Loss and Warning Logic

The app should detect possible loss conditions.

Warnings:

Life <= 5
Poison >= 8
Commander damage >= 18 from one commander

Possible loss conditions:

Life <= 0
Poison >= 10
Commander damage >= 21 from one commander
Manual alternate loss

Prompt example:

P2 MAY LOSE
Reason:
21 commander damage
from P1 commander.
A: Confirm
B: Cancel

Important:

A player should only be marked as eliminated after user confirmation.

⸻

MVP Scope

The minimum viable product should include:

2 to 4 players
Life tracking
Commander damage tracking
Poison counters
Manual elimination
Winner detection
Game reset
Basic settings
Simple main screen
Simple player detail screen

⸻

Version 1.0 Scope

Version 1.0 should include:

Up to 8 players
Rad counters
Energy counters
Experience counters
Commander tax
Monarch
Initiative
Turn counter
Storm counter
Custom counters
Icon system
Warnings and confirmations

⸻

Version 1.1 Scope

Version 1.1 should include:

Rad helper
Partner commander support
Ticket counters
City’s Blessing
Day/Night
Token presets
Improved player overview
Save/load investigation

⸻

Version 1.2 Scope

Version 1.2 may include:

Infect/Toxic helper
Ring tracker
Dungeon simple tracker
Emblem count
Floating mana tracker
Planechase helper
Archenemy helper
Two-Headed Giant mode
Dice roller
Coin flip
Mulligan tracker

⸻

Suggested Folder Structure

commander-gbc-life-counter/
│
├── README.md
├── CODEX_PLANNING.md
├── DESIGN.md
├── RULES.md
├── ROADMAP.md
├── TESTING.md
│
├── prototype_python/
│   ├── main.py
│   ├── player.py
│   ├── game_state.py
│   ├── rules.py
│   ├── actions.py
│   ├── ui_terminal.py
│   └── tests/
│       ├── test_life.py
│       ├── test_poison.py
│       ├── test_rad.py
│       ├── test_energy.py
│       ├── test_commander_damage.py
│       ├── test_elimination.py
│       └── test_winner.py
│
├── gbc_src/
│   ├── main.c
│   ├── game_state.c
│   ├── game_state.h
│   ├── player.c
│   ├── player.h
│   ├── rules.c
│   ├── rules.h
│   ├── input.c
│   ├── input.h
│   ├── screens.c
│   ├── screens.h
│   └── assets/
│       ├── icons.png
│       ├── font.png
│       └── palettes.c
│
└── docs/
    ├── ui_flow.md
    ├── data_model.md
    ├── gbc_constraints.md
    └── porting_notes.md

⸻

Python Prototype Requirements

The first implementation should be the Python rules engine.

It should include:

* Player model.
* GameState model.
* Life actions.
* Poison actions.
* Rad actions.
* Energy actions.
* Experience actions.
* Commander tax actions.
* Commander damage actions.
* Status actions.
* Loss condition checks.
* Winner detection.
* Reset logic.
* Unit tests.

The Python code should avoid designs that are difficult to port to C.

Avoid:

* Heavy inheritance.
* Complex dynamic typing.
* Large dictionaries for core data.
* External dependencies for core logic.
* UI logic mixed with rule logic.

Prefer:

* Simple classes.
* Lists.
* Integers.
* Explicit functions.
* Small modules.
* Easy-to-test behavior.

⸻

Testing Requirements

Python tests should verify:

Life can increase.
Life can decrease.
Life can go below zero.
Player is warned at 0 or less life.
Poison can increase and decrease.
Player is warned at poison threshold.
Rad counters can increase and decrease.
Rad helper applies life loss and removes rad counters.
Energy can increase and decrease.
Experience can increase and decrease.
Commander damage is tracked by source.
Commander damage from different commanders does not combine.
Player is warned at 21 commander damage from the same commander.
Commander tax increases correctly.
Monarch can only belong to one player.
Initiative can only belong to one player.
Winner is detected when only one player remains.
Game reset restores default state.

⸻

Development Guidelines for Codex

Codex should follow these rules:

Read CODEX_PLANNING.md before implementing features.
Do not treat this as only a life counter.
Do not implement the GBC version before the Python rules engine is stable.
Keep Python logic simple enough to port to C.
Do not assume there are exactly 4 players.
Do not combine commander damage from different commanders.
Do not automatically eliminate players without confirmation.
Do not use Unicode symbols as final GBC icons.
Use icon IDs and tile-based graphics for the final GBC version.
Separate rules, actions, UI, and data models.
Write tests before expanding advanced features.

⸻

Suggested First Codex Prompt

You are helping me build Commander GBC Life Counter.
Read README.md and CODEX_PLANNING.md before implementing anything.
First, implement only the Python 3.10.9 rules engine and unit tests. Do not implement the Game Boy Color version yet.
The project is a Magic: The Gathering Commander game-state tracker designed to later be ported to Game Boy Color.
Implement:
- Player model
- GameState model
- Life tracking
- Poison counters
- Rad counters
- Energy counters
- Experience counters
- Commander tax
- Commander damage matrix
- Monarch status
- Initiative status
- Turn counter
- Storm counter
- Manual elimination
- Loss condition detection
- Winner detection
- Reset game
Important:
- Keep the code easy to port to C later.
- Use simple classes, lists, integers, and explicit functions.
- Avoid unnecessary dependencies.
- Add unit tests for all core rules.
- Support more than 4 players in the data model.
- Commander damage from different commanders must not combine.
- A player may lose at 21 damage from the same commander.
- A player may lose at 10 poison counters by default.
- A player may lose at 0 or less life, but elimination should be confirmable.

⸻

Long-Term Vision

The final app should feel like a retro Commander control panel for Game Boy Color.

It should be fast enough to use during a real Commander game, simple enough to navigate with Game Boy buttons, and flexible enough to handle modern Commander mechanics.

The project should prioritize correctness, portability, and usability over visual complexity.

The final goal is a playable .gb or .gbc ROM that can be used in an emulator or compatible hardware as a real Commander life counter and game-state tracker.
