"""Explicit mutations that can be applied to a :class:`GameState`."""

from typing import TYPE_CHECKING

from .constants import DAY_NIGHT_NONE, MAX_CUSTOM_COUNTERS
from .game_state import GameState

if TYPE_CHECKING:
    from .rules import RuleResult


def _require_nonnegative(value: int, label: str = "amount") -> None:
    if value < 0:
        raise ValueError(f"{label} cannot be negative")


def increase_life(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.life += amount
    return player.life


def decrease_life(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.life -= amount
    return player.life


def set_life(game: GameState, player_id: int, value: int) -> int:
    player = game.get_player(player_id)
    player.life = value
    return player.life


def reset_life(game: GameState, player_id: int) -> int:
    return set_life(game, player_id, game.starting_life)


def add_poison(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.poison += amount
    return player.poison


def remove_poison(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.poison = max(0, player.poison - amount)
    return player.poison


def set_poison(game: GameState, player_id: int, value: int) -> int:
    _require_nonnegative(value, "poison")
    player = game.get_player(player_id)
    player.poison = value
    return player.poison


def add_rad(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.rad += amount
    return player.rad


def remove_rad(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.rad = max(0, player.rad - amount)
    return player.rad


def set_rad(game: GameState, player_id: int, value: int) -> int:
    _require_nonnegative(value, "rad")
    player = game.get_player(player_id)
    player.rad = value
    return player.rad


def apply_rad_helper(
    game: GameState, player_id: int, nonland_cards_milled: int
) -> list["RuleResult"]:
    """Apply the life loss and rad removal after cards are milled physically."""
    _require_nonnegative(nonland_cards_milled, "nonland_cards_milled")
    player = game.get_player(player_id)
    if nonland_cards_milled > player.rad:
        raise ValueError("nonland cards milled cannot exceed current rad counters")

    player.life -= nonland_cards_milled
    player.rad -= nonland_cards_milled

    from .rules import check_player_conditions

    return check_player_conditions(game, player_id)


def add_energy(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.energy += amount
    return player.energy


def spend_energy(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    if amount > player.energy:
        raise ValueError("cannot spend more energy than the player has")
    player.energy -= amount
    return player.energy


def set_energy(
    game: GameState, player_id: int, value: int, allow_negative: bool = False
) -> int:
    if value < 0 and not allow_negative:
        raise ValueError("energy cannot be negative")
    player = game.get_player(player_id)
    player.energy = value
    return player.energy


def clear_energy(game: GameState, player_id: int) -> int:
    return set_energy(game, player_id, 0)


def add_experience(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.experience += amount
    return player.experience


def remove_experience(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.experience = max(0, player.experience - amount)
    return player.experience


def set_experience(game: GameState, player_id: int, value: int) -> int:
    _require_nonnegative(value, "experience")
    player = game.get_player(player_id)
    player.experience = value
    return player.experience


def add_ticket(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.ticket += amount
    return player.ticket


def remove_ticket(game: GameState, player_id: int, amount: int = 1) -> int:
    _require_nonnegative(amount)
    player = game.get_player(player_id)
    player.ticket = max(0, player.ticket - amount)
    return player.ticket


def set_ticket(game: GameState, player_id: int, value: int) -> int:
    _require_nonnegative(value, "ticket")
    player = game.get_player(player_id)
    player.ticket = value
    return player.ticket


def add_custom_counter(
    game: GameState, player_id: int, counter_slot: int, amount: int = 1
) -> int:
    _require_nonnegative(amount)
    _validate_custom_counter_slot(counter_slot)
    player = game.get_player(player_id)
    player.custom_counters[counter_slot] += amount
    return player.custom_counters[counter_slot]


def remove_custom_counter(
    game: GameState, player_id: int, counter_slot: int, amount: int = 1
) -> int:
    _require_nonnegative(amount)
    _validate_custom_counter_slot(counter_slot)
    player = game.get_player(player_id)
    player.custom_counters[counter_slot] = max(
        0, player.custom_counters[counter_slot] - amount
    )
    return player.custom_counters[counter_slot]


def set_custom_counter(
    game: GameState, player_id: int, counter_slot: int, value: int
) -> int:
    _require_nonnegative(value, "custom counter")
    _validate_custom_counter_slot(counter_slot)
    player = game.get_player(player_id)
    player.custom_counters[counter_slot] = value
    return player.custom_counters[counter_slot]


def _validate_custom_counter_slot(counter_slot: int) -> None:
    if counter_slot < 0 or counter_slot >= MAX_CUSTOM_COUNTERS:
        raise ValueError(f"invalid custom counter slot: {counter_slot}")


def add_commander_damage(
    game: GameState,
    target_player_id: int,
    source_player_id: int,
    amount: int = 1,
    commander_slot: int = 0,
) -> int:
    _require_nonnegative(amount)
    current = get_commander_damage(
        game, target_player_id, source_player_id, commander_slot
    )
    return set_commander_damage(
        game,
        target_player_id,
        source_player_id,
        current + amount,
        commander_slot,
    )


def remove_commander_damage(
    game: GameState,
    target_player_id: int,
    source_player_id: int,
    amount: int = 1,
    commander_slot: int = 0,
) -> int:
    _require_nonnegative(amount)
    current = get_commander_damage(
        game, target_player_id, source_player_id, commander_slot
    )
    return set_commander_damage(
        game,
        target_player_id,
        source_player_id,
        max(0, current - amount),
        commander_slot,
    )


def set_commander_damage(
    game: GameState,
    target_player_id: int,
    source_player_id: int,
    value: int,
    commander_slot: int = 0,
) -> int:
    _require_nonnegative(value, "commander damage")
    game.get_player(target_player_id)
    game.get_player(source_player_id)
    game.validate_commander_slot(commander_slot)
    game.commander_damage[target_player_id][source_player_id][commander_slot] = value
    return value


def get_commander_damage(
    game: GameState,
    target_player_id: int,
    source_player_id: int,
    commander_slot: int = 0,
) -> int:
    game.get_player(target_player_id)
    game.get_player(source_player_id)
    game.validate_commander_slot(commander_slot)
    return game.commander_damage[target_player_id][source_player_id][commander_slot]


def get_commander_damage_received(
    game: GameState, target_player_id: int
) -> list[list[int]]:
    game.get_player(target_player_id)
    return [values.copy() for values in game.commander_damage[target_player_id]]


def calculate_commander_tax(cast_count: int) -> int:
    _require_nonnegative(cast_count, "cast_count")
    return 2 * max(0, cast_count - 1)


def increase_commander_cast_count(
    game: GameState, player_id: int, commander_slot: int = 0
) -> int:
    player = game.get_player(player_id)
    game.validate_commander_slot(commander_slot)
    return set_commander_cast_count(
        game,
        player_id,
        player.commander_cast_count[commander_slot] + 1,
        commander_slot,
    )


def decrease_commander_cast_count(
    game: GameState, player_id: int, commander_slot: int = 0
) -> int:
    player = game.get_player(player_id)
    game.validate_commander_slot(commander_slot)
    return set_commander_cast_count(
        game,
        player_id,
        max(0, player.commander_cast_count[commander_slot] - 1),
        commander_slot,
    )


def set_commander_cast_count(
    game: GameState,
    player_id: int,
    cast_count: int,
    commander_slot: int = 0,
) -> int:
    _require_nonnegative(cast_count, "cast_count")
    player = game.get_player(player_id)
    game.validate_commander_slot(commander_slot)
    player.commander_cast_count[commander_slot] = cast_count
    player.commander_tax[commander_slot] = calculate_commander_tax(cast_count)
    return cast_count


def set_commander_tax(
    game: GameState, player_id: int, value: int, commander_slot: int = 0
) -> int:
    _require_nonnegative(value, "commander tax")
    player = game.get_player(player_id)
    game.validate_commander_slot(commander_slot)
    player.commander_tax[commander_slot] = value
    return value


def set_monarch(game: GameState, player_id: int | None) -> None:
    if player_id is not None:
        player = game.get_player(player_id)
        if player.eliminated:
            raise ValueError("an eliminated player cannot become monarch")
    for player in game.players:
        player.is_monarch = False
    game.monarch_player = player_id
    if player_id is not None:
        game.players[player_id].is_monarch = True


def clear_monarch(game: GameState) -> None:
    set_monarch(game, None)


def set_initiative(game: GameState, player_id: int | None) -> None:
    if player_id is not None:
        player = game.get_player(player_id)
        if player.eliminated:
            raise ValueError("an eliminated player cannot have the initiative")
    for player in game.players:
        player.has_initiative = False
    game.initiative_player = player_id
    if player_id is not None:
        game.players[player_id].has_initiative = True


def clear_initiative(game: GameState) -> None:
    set_initiative(game, None)


def increase_storm_count(game: GameState, amount: int = 1) -> int:
    _require_nonnegative(amount)
    game.storm_count += amount
    return game.storm_count


def decrease_storm_count(game: GameState, amount: int = 1) -> int:
    _require_nonnegative(amount)
    game.storm_count = max(0, game.storm_count - amount)
    return game.storm_count


def set_storm_count(game: GameState, value: int) -> int:
    _require_nonnegative(value, "storm count")
    game.storm_count = value
    return game.storm_count


def reset_storm_count(game: GameState) -> int:
    return set_storm_count(game, 0)


def set_active_player(game: GameState, player_id: int) -> int:
    player = game.get_player(player_id)
    if player.eliminated:
        raise ValueError("active player cannot be eliminated")
    game.active_player = player_id
    return player_id


def _find_non_eliminated_player(
    game: GameState, start_player_id: int, direction: int
) -> int | None:
    for offset in range(1, game.player_count + 1):
        candidate = (start_player_id + direction * offset) % game.player_count
        if not game.players[candidate].eliminated:
            return candidate
    return None


def next_turn(game: GameState) -> int | None:
    if game.active_player is None:
        active_players = [p.player_id for p in game.players if not p.eliminated]
        game.active_player = active_players[0] if active_players else None
    else:
        game.active_player = _find_non_eliminated_player(game, game.active_player, 1)
    game.turn_number += 1
    return game.active_player


def previous_turn(game: GameState) -> int | None:
    if game.turn_number <= 1:
        return game.active_player
    if game.active_player is None:
        active_players = [p.player_id for p in game.players if not p.eliminated]
        game.active_player = active_players[-1] if active_players else None
    else:
        game.active_player = _find_non_eliminated_player(game, game.active_player, -1)
    game.turn_number -= 1
    return game.active_player


def mark_player_eliminated(game: GameState, player_id: int) -> None:
    player = game.get_player(player_id)
    player.eliminated = True
    if game.monarch_player == player_id:
        clear_monarch(game)
    if game.initiative_player == player_id:
        clear_initiative(game)
    if game.active_player == player_id:
        game.active_player = _find_non_eliminated_player(game, player_id, 1)


def restore_eliminated_player(game: GameState, player_id: int) -> None:
    player = game.get_player(player_id)
    player.eliminated = False
    if game.active_player is None:
        game.active_player = player_id


def confirm_possible_loss(game: GameState, player_id: int) -> bool:
    from .rules import check_player_conditions

    results = check_player_conditions(game, player_id)
    if not any(result.has_loss_condition for result in results):
        return False
    mark_player_eliminated(game, player_id)
    return True


def reset_player(game: GameState, player_id: int) -> None:
    player = game.get_player(player_id)
    player.reset(game.starting_life)

    for other_player_id in range(game.player_count):
        for commander_slot in range(len(player.commander_tax)):
            game.commander_damage[player_id][other_player_id][commander_slot] = 0
            game.commander_damage[other_player_id][player_id][commander_slot] = 0

    if game.monarch_player == player_id:
        clear_monarch(game)
    if game.initiative_player == player_id:
        clear_initiative(game)
    if game.active_player is None:
        game.active_player = player_id


def reset_game(game: GameState) -> None:
    for player in game.players:
        player.reset(game.starting_life)
    game.reset_commander_damage()
    game.turn_number = 1
    game.active_player = 0
    game.monarch_player = None
    game.initiative_player = None
    game.storm_count = 0
    game.day_night_state = DAY_NIGHT_NONE
