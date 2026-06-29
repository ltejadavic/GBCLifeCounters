CODEX_PLANNING.md

Commander GBC Life Counter — Planning Reference

1. Purpose of This Document

This document defines the extended planning rules, gameplay counters, icon system, UI priorities, and implementation decisions for the Commander GBC Life Counter project.

Codex should consult this file before implementing features, UI screens, counters, icons, or game-state logic.

The project is not only a life counter. It should be a compact Commander game-state assistant for Game Boy Color.

⸻

2. Core Philosophy

The app should track the most important information that Commander players usually need during a game:

* Life total.
* Commander damage.
* Poison counters.
* Rad counters.
* Energy counters.
* Experience counters.
* Commander tax.
* Monarch.
* Initiative.
* Storm count.
* Turn count.
* Active player.
* Optional custom counters.

The app should not try to simulate all Magic rules. It should help players remember and organize visible game-state information.

Magic has many exceptions, replacement effects, prevention effects, alternate win conditions, and card-specific rules. Because of this, automatic loss/win detection should always allow manual confirmation.

⸻

3. Rules Research Notes

A counter in Magic is a marker placed on an object or player, and counters themselves are not objects or tokens. This matters because the app should separate “player counters” from “permanent counters” and from “game status flags.” (yawgatog.com)

Player counters that are relevant for this project include poison counters, energy counters, experience counters, rad counters, and ticket counters. Wizards release notes for Modern Horizons 3 explicitly mention those as examples of counters players can have. (MAGIC: THE GATHERING)

In Commander, a player loses after being dealt 21 or more combat damage by the same commander over the course of the game. Commander damage must therefore be tracked by source commander, not as one shared total. (MAGIC: THE GATHERING)

A player normally loses the game at 10 or more poison counters. For Two-Headed Giant, the poison threshold is different, but for normal Commander this app should default to 10. (mtg.fandom.com)

Rad counters are player counters introduced with Magic: The Gathering — Fallout. At the beginning of a player’s precombat main phase, if that player has rad counters, they mill that many cards; for each nonland card milled this way, they lose 1 life and remove one rad counter. (MAGIC: THE GATHERING)

Energy counters are also player counters, but unlike rad counters, they do not have an inherent automatic rule effect by themselves. They are mainly a resource used to pay costs on cards. (MAGIC: THE GATHERING)

⸻

4. Important Distinction: Counters vs Statuses vs Derived Values

The app should classify tracked information into three categories.

4.1 Player Counters

These are counters that exist on a player.

Recommended built-in player counters:

* Poison counters.
* Rad counters.
* Energy counters.
* Experience counters.
* Ticket counters.

Optional or custom player counters:

* Shield-like custom counter.
* Quest-style custom counter.
* Vote counter.
* Generic custom counter A.
* Generic custom counter B.

4.2 Game Status Flags

These are not always counters, but they are important to track.

Recommended status flags:

* Monarch.
* Initiative.
* City’s Blessing.
* Day/Night.
* The Ring tempts you / Ring-bearer.
* Emblem ownership.
* Dungeon progress.
* Planechase plane active.
* Archenemy scheme active.

4.3 Derived or Utility Values

These are not Magic counters, but they are useful game-tracking values.

Recommended utility values:

* Life total.
* Commander tax.
* Commander cast count.
* Commander damage received.
* Storm count.
* Turn number.
* Active player.
* Mulligan count.
* Floating mana, optional.
* Treasures, Clues, Food, Blood, Maps, Junk tokens, optional.
* Number of commander deaths, optional.
* Number of times commander returned to command zone, optional.

⸻

5. Built-In Counter List

The app should include these as first-class supported counters.

5.1 Life

Type:

* Core value.

Default:

* 40 in Commander.

Icon concept:

* Heart.

Suggested 8x8 icon:

* Pixel heart.
* If color is available, use red/pink palette.
* In monochrome, use a filled heart shape.

UI label:

LIFE

Short label:

HP

Behavior:

* Can increase.
* Can decrease.
* Can go below 0.
* If life <= 0, show possible loss warning.
* Do not automatically eliminate without confirmation.

⸻

5.2 Commander Damage

Type:

* Commander-specific damage matrix.

Default threshold:

21 combat damage from the same commander

Icon concept:

* Sword.
* Alternative: helmet, crown-sword, commander star.

Suggested 8x8 icon:

* Diagonal sword.
* For partner mode, use Sword A and Sword B markers.

UI label:

CMD DMG

Short label:

CMD

Behavior:

* Track damage from each commander to each player.
* Damage from different commanders must not combine.
* Damage should remain even if the player gains life.
* Warning at 18+.
* Possible loss at 21+.
* Must support partner commanders as optional advanced mode.

Data model:

commander_damage[target_player][source_player][commander_slot]

Where:

commander_slot = 0 for main commander
commander_slot = 1 for partner/background/second commander

If partner mode is disabled, only slot 0 is used.

⸻

5.3 Poison Counters

Type:

* Player counter.

Default threshold:

10 poison counters

Icon concept:

* Skull.
* Alternative: toxic drop, biohazard, snake fang.

Suggested 8x8 icon:

* Small skull.
* For readability, skull should be simple: two eye pixels and jaw line.
* If color is available, use green/purple palette.

UI label:

POISON

Short label:

PSN

Behavior:

* Can increase.
* Can decrease.
* Warning at 8+.
* Possible loss at 10+.
* Should be affected by proliferate helper if implemented.
* Should be included in “player counters” menu.

Important note:

* Infect commander damage may create poison counters and still count as commander damage because damage was dealt by the commander. The app should allow the user to add both poison and commander damage when needed.

Recommended shortcut:

When adding commander damage, optionally ask:
"Also add poison?"
Only if infect/toxic helper mode is enabled.

⸻

5.4 Rad Counters

Type:

* Player counter.

Icon concept:

* Radiation symbol.
* Alternative: three-blade hazard symbol.

Suggested 8x8 icon:

* Simple radiation trefoil:
    * center dot
    * three small triangular blades

UI label:

RAD

Short label:

RAD

Behavior:

* Can increase.
* Can decrease.
* Does not directly cause loss at a fixed threshold.
* Has an optional helper for the precombat main phase trigger.

Suggested helper flow:

RAD TRIGGER — Player X has N rad counters.
1. Mill N cards manually.
2. Enter number of nonland cards milled.
3. App subtracts that much life.
4. App removes that many rad counters.

Example:

Player has 5 rad counters.
Player mills 5 cards.
3 are nonland.
App applies:
-3 life
-3 rad counters

Important:

* Do not automatically mill or determine card types.
* The app only asks the user to enter the number of nonland cards milled.

Priority:

* Version 1.0 or 1.1.
* Very relevant because Fallout Commander decks introduced rad counters and they are common in The Wise Mothman-style Commander decks.

⸻

5.5 Energy Counters

Type:

* Player counter / resource.

Icon concept:

* Lightning bolt.
* Alternative: battery, spark.

Suggested 8x8 icon:

* Lightning bolt.

UI label:

ENERGY

Short label:

ENE

Behavior:

* Can increase.
* Can decrease.
* No default win/loss condition.
* Used as a spendable resource.
* Should support fast add/subtract.

Recommended actions:

+1
-1
+3
-3
Set
Clear

Priority:

* Version 1.0 or 1.1.

⸻

5.6 Experience Counters

Type:

* Player counter.

Icon concept:

* Star.
* Alternative: medal, upward arrow.

Suggested 8x8 icon:

* Star.
* If too detailed, use a small medal/ribbon.

UI label:

EXP

Short label:

EXP

Behavior:

* Can increase.
* Can decrease.
* No default win/loss condition.
* Stays on the player.
* Multiple cards can care about the same experience counter total.

Priority:

* Version 1.0.

⸻

5.7 Ticket Counters

Type:

* Player counter.

Icon concept:

* Ticket stub.
* Alternative: small rectangle with notch.

Suggested 8x8 icon:

* Rectangle ticket with one pixel cut from each side.

UI label:

TICKET

Short label:

TIX

Behavior:

* Can increase.
* Can decrease.
* No default win/loss condition.
* Optional because ticket counters are less common in Commander than poison, energy, rad, or experience.

Priority:

* Version 1.2 or optional custom counter preset.

⸻

5.8 Commander Tax

Type:

* Utility value, not an official player counter.

Icon concept:

* Coin plus crown.
* Alternative: command zone symbol, small crown.

Suggested 8x8 icon:

* Crown with plus sign.
* Or small coin stack.

UI label:

CMD TAX

Short label:

TAX

Behavior:

* Tracks how much extra mana a commander costs due to being cast from the command zone.
* Usually increases by 2 each time the commander is cast from the command zone after the first.
* Should allow manual increase/decrease.
* Should support partner commanders separately.

Recommended model:

commander_cast_count[player][commander_slot]
commander_tax[player][commander_slot]

Possible calculation:

tax = 2 * max(0, cast_count - 1)

But manual override should exist.

Priority:

* Version 1.0.

⸻

5.9 Storm Count

Type:

* Global utility value.

Icon concept:

* Swirl.
* Alternative: tornado, spiral.

Suggested 8x8 icon:

* Spiral/swirling line.

UI label:

STORM

Short label:

STM

Behavior:

* Global for the current turn.
* Can increase/decrease.
* Should reset manually at end of turn.
* Optional auto-reset when active player changes, but confirm first.

Priority:

* Version 1.1.

⸻

5.10 Monarch

Type:

* Game status flag.

Icon concept:

* Crown.

Suggested 8x8 icon:

* Crown.

UI label:

MONARCH

Short label:

MON

Behavior:

* Only one player can be monarch at a time.
* Can be set to none.
* Show crown icon next to monarch player on main screen.

Priority:

* Version 1.0.

⸻

5.11 Initiative

Type:

* Game status flag.

Icon concept:

* Torch.
* Alternative: dungeon door.

Suggested 8x8 icon:

* Torch.

UI label:

INITIATIVE

Short label:

INI

Behavior:

* Only one player can have the initiative at a time.
* Can be set to none.
* Show torch icon next to the player with initiative.

Priority:

* Version 1.0 or 1.1.

⸻

5.12 City’s Blessing

Type:

* Player status flag.

Icon concept:

* Sun.
* Alternative: small city silhouette.

Suggested 8x8 icon:

* Sunburst.

UI label:

BLESSING

Short label:

ASC

Behavior:

* Player either has it or does not.
* Once gained, normally stays for the rest of the game.
* No numeric value required.

Priority:

* Version 1.2.

⸻

5.13 Day/Night

Type:

* Global game status.

Icon concept:

* Sun/moon toggle.

Suggested 8x8 icons:

* Sun for day.
* Crescent moon for night.

UI label:

DAY/NIGHT

Short label:

DAY
NGT

Behavior:

* Global state.
* Values:
    * None
    * Day
    * Night
* Can be toggled manually.

Priority:

* Version 1.2.

⸻

5.14 The Ring / Ring-Bearer

Type:

* Player status and level-like state.

Icon concept:

* Ring.

Suggested 8x8 icon:

* Small circle/ring.

UI label:

RING

Short label:

RNG

Behavior:

* Track which player has a Ring-bearer.
* Optional: track Ring temptation level from 0 to 4.
* Since the actual Ring-bearer is a creature, the app should only track the player and level, not the creature.

Priority:

* Version 1.2 or optional preset.

⸻

5.15 Dungeon / Initiative Progress

Type:

* Player progress tracker.

Icon concept:

* Door.
* Alternative: stairs.

Suggested 8x8 icon:

* Dungeon door.

UI label:

DUNGEON

Short label:

DNG

Behavior:

* Complex if fully implemented.
* MVP should not implement full dungeon maps.
* Simple version:
    * Track whether player is in a dungeon.
    * Track room number manually.
    * Optional string/ID for dungeon type.

Priority:

* Stretch goal.

⸻

5.16 Emblems

Type:

* Player status collection.

Icon concept:

* Flag.
* Alternative: badge.

Suggested 8x8 icon:

* Small banner/flag.

UI label:

EMBLEM

Short label:

EMB

Behavior:

* Players can have multiple emblems.
* Since GBC storage and UI are limited, use only a count or custom notes are not recommended.
* Simple tracker:
    * Emblem count per player.
    * Optional “has emblem” flag.

Priority:

* Stretch goal.

⸻

5.17 Floating Mana

Type:

* Temporary resource tracker.

Icon concept:

* Mana drop.
* Alternative: five small colored pips.

Suggested 8x8 icon:

* Generic mana diamond.
* Avoid trying to fit all five mana symbols unless there is enough tile space.

UI label:

MANA

Short label:

MNA

Behavior:

* Optional.
* Track W/U/B/R/G/C values.
* This may be too much for the main app.
* If implemented, place it in a separate advanced screen.

Priority:

* Stretch goal.

⸻

5.18 Tokens: Treasure, Clue, Food, Blood, Map, Junk

Type:

* Permanent/token count helper, not player counters.

Icon concepts:

* Treasure: gem.
* Clue: magnifying glass.
* Food: apple.
* Blood: droplet.
* Map: folded map.
* Junk: scrap pile.

Behavior:

* Optional because these are actual tokens/permanents, not player counters.
* Useful for Commander but may clutter the app.
* Recommended as custom counter presets rather than main built-in values.

Priority:

* Version 1.2 as optional presets.

⸻

6. Recommended Icon Table

Use short 3-letter labels internally and 8x8 pixel icons visually.

LIFE      HP   Heart
COMMANDER CMD  Sword
POISON    PSN  Skull
RAD       RAD  Radiation trefoil
ENERGY    ENE  Lightning bolt
EXP       EXP  Star / medal
TICKET    TIX  Ticket stub
TAX       TAX  Crown + plus / coin
MONARCH   MON  Crown
INIT      INI  Torch
STORM     STM  Swirl
TURN      TRN  Circular arrow
ACTIVE    ACT  Flag / arrow
DEAD      OUT  X / tombstone
BLESSING  ASC  Sunburst
DAY       DAY  Sun
NIGHT     NGT  Moon
RING      RNG  Ring
DUNGEON   DNG  Door
EMBLEM    EMB  Flag / badge
TREASURE  TRS  Gem
CLUE      CLU  Magnifying glass
FOOD      FOD  Apple
BLOOD     BLD  Drop
MAP       MAP  Folded map
JUNK      JNK  Scrap
MANA      MNA  Diamond / mana pip

⸻

7. Icon Design Rules for Game Boy Color

All icons should follow these rules:

* Size: 8x8 pixels by default.
* Optional large version: 16x16 for detail screens.
* Use simple silhouettes.
* Avoid tiny internal details.
* Must be readable in monochrome.
* Must work with limited palettes.
* Should be recognizable even without color.
* Use labels when icon meaning may be unclear.

Recommended visual priority:

1. Life
2. Commander damage
3. Poison
4. Rad
5. Energy
6. Experience
7. Monarch
8. Initiative
9. Commander tax
10. Storm

Do not overload the main screen with too many icons. The main screen should show only the most important active icons.

⸻

8. Main Screen Display Priorities

The main screen should show:

* Player name.
* Life total.
* Poison warning icon if poison > 0.
* Commander damage warning icon if near lethal.
* Rad icon if rad > 0.
* Monarch icon if player is monarch.
* Initiative icon if player has initiative.
* Eliminated marker if eliminated.

For 2–4 players:

P1  40   P2  38
P3  25   P4  41

For 5–8 players:

> P1  40  ♡
  P2  38  ☠
  P3  25  ⚔
  P4  41  ☢

Scroll down:

> P5  40
  P6  40
  P7  40
  P8  40

Since real symbols may not exist in the final tileset, use custom 8x8 tiles instead of Unicode.

⸻

9. Player Detail Screen

The player detail screen should show more information.

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

If space is limited, split into tabs:

TAB 1: Life / Poison / Commander Damage
TAB 2: Counters
TAB 3: Statuses
TAB 4: Commander Tax

⸻

10. Counter Menu Structure

The counter menu should be grouped to avoid clutter.

Recommended groups:

Core
- Life
- Commander damage
- Poison
- Commander tax
Player counters
- Rad
- Energy
- Experience
- Tickets
- Custom 1
- Custom 2
Statuses
- Monarch
- Initiative
- City’s Blessing
- Ring
- Day/Night
Global
- Turn
- Storm
- Active player
Tokens / optional
- Treasure
- Clue
- Food
- Blood
- Map
- Junk

⸻

11. Counter Preset System

Instead of hardcoding every possible counter as a full independent feature, the app should support counter presets.

Each preset should have:

id
short_label
long_label
icon_id
type
default_value
min_value
max_value
has_loss_condition
warning_threshold
loss_threshold
scope

Example:

id: POISON
short_label: PSN
long_label: Poison
icon_id: ICON_SKULL
type: PLAYER_COUNTER
default_value: 0
min_value: 0
max_value: 99
has_loss_condition: true
warning_threshold: 8
loss_threshold: 10
scope: PLAYER

Example:

id: ENERGY
short_label: ENE
long_label: Energy
icon_id: ICON_LIGHTNING
type: PLAYER_COUNTER
default_value: 0
min_value: 0
max_value: 99
has_loss_condition: false
scope: PLAYER

Example:

id: MONARCH
short_label: MON
long_label: Monarch
icon_id: ICON_CROWN
type: UNIQUE_PLAYER_STATUS
default_value: none
scope: GAME

This structure makes it easier to add new counters later.

⸻

12. Recommended Built-In Presets for Version 1.0

Version 1.0 should include:

LIFE
COMMANDER_DAMAGE
POISON
RAD
ENERGY
EXPERIENCE
COMMANDER_TAX
MONARCH
INITIATIVE
TURN
ACTIVE_PLAYER
STORM
CUSTOM_COUNTER_1
CUSTOM_COUNTER_2

Version 1.1 should add:

TICKET
CITYS_BLESSING
DAY_NIGHT
RING
TREASURE
CLUE
FOOD
BLOOD

Stretch goals:

MAP
JUNK
DUNGEON
EMBLEM
FLOATING_MANA
PLANECHASE
ARCHENEMY
TWO_HEADED_GIANT

⸻

13. Loss and Warning Logic

The app should detect possible loss conditions but ask for confirmation.

Automatic warnings:

Life <= 5
Poison >= 8
Commander damage >= 18 from one commander
Library loss manually triggered

Possible loss prompts:

Life <= 0
Poison >= 10
Commander damage >= 21 from same commander
Manual alternate loss

Prompt example:

P2 MAY LOSE
Reason:
21 commander damage
from P1 commander.
A: Confirm
B: Cancel

The player should not be removed immediately. They should be marked as eliminated only after confirmation.

⸻

14. Rad Counter Helper

Rad counters deserve a special helper because they have a repeating trigger.

The app should include a Rad Helper screen.

Flow:

1. Select player.
2. App shows current rad counter count.
3. User confirms rad trigger.
4. User mills cards physically.
5. User enters number of nonland cards milled.
6. App subtracts that much life.
7. App removes that many rad counters.
8. App checks possible life loss condition.

Example:

P1 RAD: 6
Mill 6 cards.
How many nonlands?
Value: 4
Apply:
-4 life
-4 rad

Confirm:

A: Apply
B: Cancel

Do not automate card identification.

⸻

15. Infect / Toxic Helper

The app should optionally support an infect/toxic shortcut.

This is not required for MVP, but it is useful.

Modes:

Normal damage:
- subtract life
- optionally add commander damage if source is commander
Infect damage:
- add poison counters
- if source is commander combat damage, also add commander damage
Toxic:
- subtract combat damage from life normally
- add poison counters equal to toxic value
- if source is commander combat damage, also add commander damage

Because this can become complex, keep it optional.

Recommended implementation:

Damage Entry Screen:
1. Target player
2. Source player
3. Source is commander? yes/no
4. Damage amount
5. Damage type:
   - normal
   - infect
   - toxic
6. Toxic value if needed
7. Apply results

Priority:

* Version 1.2.

⸻

16. More Than Four Players

The app must not be designed around only four players.

Default target:

2 to 8 players

Stretch target:

10 players

GBC limitation:

* The screen is small.
* The app should use scrolling lists and focused detail screens.

Implementation rule:

Never assume player_count == 4.
All loops must use current player_count.
All arrays should support MAX_PLAYERS.

Recommended constant:

MAX_PLAYERS = 8

If memory allows:

MAX_PLAYERS = 10

⸻

17. Partner Commander and Background Support

Commander damage and commander tax should support more than one commander per player.

Modes:

Single commander mode
Partner commander mode

In single commander mode:

commander_slot = 0

In partner mode:

commander_slot = 0 for Commander A
commander_slot = 1 for Commander B

Each commander should track damage separately.

Example:

P1 Commander A dealt 12 damage to P2.
P1 Commander B dealt 9 damage to P2.
P2 does not lose because these do not combine.

Commander tax should also be separate per commander.

⸻

18. Data Model Requirements

The final GBC version should use fixed-size arrays.

Recommended constants:

MAX_PLAYERS = 8
MAX_COMMANDERS_PER_PLAYER = 2
MAX_CUSTOM_COUNTERS_PER_PLAYER = 4
MAX_NAME_LENGTH = 8
MAX_COUNTER_VALUE = 99

Recommended player fields:

id
name
life
poison
rad
energy
experience
ticket
commander_tax[2]
commander_cast_count[2]
custom_counter_values[4]
custom_counter_types[4]
eliminated
has_citys_blessing
ring_level
has_ring_bearer
emblem_count

Recommended game fields:

player_count
starting_life
active_player
turn_number
storm_count
monarch_player
initiative_player
day_night_state
partner_mode_enabled
confirm_eliminations
poison_threshold
commander_damage_threshold

Commander damage:

commander_damage[target_player][source_player][commander_slot]

⸻

19. UI Design Principle

Do not place everything on one screen.

Use layered screens:

Main Overview
Player Detail
Core Actions
Commander Damage
Player Counters
Statuses
Global State
Settings
Advanced Helpers

The main screen should answer:

Who is alive?
How much life do they have?
Is anyone close to losing?
Who has monarch/initiative?
Who has dangerous counters?

The detail screens should answer:

Exactly how much commander damage?
How many rad/energy/experience counters?
What statuses does this player have?
What should be edited?

⸻

20. Button Design

Recommended controls:

D-Pad:
- Move selection
- Increase/decrease values
A:
- Confirm
- Open selected item
B:
- Cancel
- Go back
Start:
- Main menu / settings
Select:
- Quick actions / switch tab

Recommended shortcuts:

Left/Right:
- Change selected value by 1
Hold A + Left/Right:
- Change selected value by 5
Hold B + Left/Right:
- Change selected value by 10
Start + Select:
- Open reset confirmation

If button combinations are hard to implement, keep shortcuts for later.

⸻

21. Feature Priority

MVP

Must include:

2 to 4 players
Life tracking
Commander damage tracking
Poison counters
Manual elimination
Winner detection
Reset game
Basic settings

Version 1.0

Should include:

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

Version 1.1

Should include:

Rad helper
Partner commander support
Ticket counters
City’s Blessing
Day/Night
Token presets
Improved player overview
Save/load investigation

Version 1.2

Could include:

Infect/Toxic helper
Ring tracker
Dungeon simple tracker
Emblem count
Floating mana tracker
Planechase helper
Archenemy helper
Two-Headed Giant mode

⸻

22. What Codex Should Avoid

Codex should avoid:

* Assuming only 4 players.
* Mixing Python prototype code with final GBC C code.
* Implementing complex Magic rules simulation.
* Creating large dynamic structures that are hard to port to C.
* Depending on Unicode symbols for the final GBC version.
* Using long text labels everywhere.
* Automatically eliminating players without confirmation.
* Combining commander damage from multiple commanders.
* Treating Monarch or Initiative as numeric counters.
* Treating Treasure/Clue/Food/Blood as player counters.
* Overloading the main screen with too much information.

⸻

23. What Codex Should Prioritize

Codex should prioritize:

* Correct game-state model.
* Simple portable logic.
* Fixed-size arrays.
* Clear screen state machine.
* Readable GBC UI.
* Fast value editing.
* Good tests in Python.
* Easy porting from Python to C.
* Manual override for rule exceptions.
* Icon IDs instead of hardcoded visual symbols.
* Configurable player count.
* Configurable thresholds.

⸻

24. Suggested Codex Prompt Addition

Use this instruction in the main Codex prompt:

Before implementing any feature, read CODEX_PLANNING.md and follow its counter classification, icon rules, and feature priorities.
Do not treat this project as only a life counter. It must be designed as a Commander game-state tracker for Game Boy Color.
The first implementation should still be the Python 3.10.9 prototype, but the code must be simple enough to port later to C/GBDK-2020.
Counters and statuses must be classified correctly:
- Life and commander damage are core values.
- Poison, rad, energy, experience, and ticket are player counters.
- Monarch, initiative, city’s blessing, day/night, ring, and emblems are statuses.
- Treasure, clue, food, blood, map, and junk are token-count helper presets, not player counters.
The final GBC version must use icon IDs and tile-based graphics, not Unicode symbols.

⸻

25. Final Design Direction

The final app should feel like a retro Commander control panel.

It should be simple enough to use during a real Commander game, but complete enough to handle modern Commander mechanics such as poison, rad counters, energy, experience counters, commander damage, partner commanders, monarch, initiative, and custom counters.

The project should be developed in this order:

1. Python rules engine.
2. Python tests.
3. Python UI mockup.
4. GBC hello-world technical test.
5. GBC one-player life counter.
6. GBC 4-player MVP.
7. GBC 8-player support.
8. Extra counters and statuses.
9. Icons and polish.
10. Save/load and advanced helpers.