from prototype_python.actions import (
    add_commander_damage,
    add_energy,
    add_experience,
    add_poison,
    add_rad,
    add_ticket,
    increase_commander_cast_count,
    increase_storm_count,
    mark_player_eliminated,
    next_turn,
    reset_game,
    reset_player,
    set_initiative,
    set_monarch,
)
from prototype_python.constants import DAY, DAY_NIGHT_NONE
from prototype_python.game_state import GameState


def test_reset_player_clears_player_state_and_related_damage() -> None:
    game = GameState()
    add_poison(game, 1, 4)
    add_energy(game, 1, 3)
    add_commander_damage(game, 1, 0, 8)
    add_commander_damage(game, 0, 1, 5)
    increase_commander_cast_count(game, 1)
    set_monarch(game, 1)

    reset_player(game, 1)

    player = game.players[1]
    assert player.life == 40
    assert player.poison == 0
    assert player.energy == 0
    assert player.commander_cast_count == [0, 0]
    assert game.commander_damage[1][0] == [0, 0]
    assert game.commander_damage[0][1] == [0, 0]
    assert game.monarch_player is None


def test_reset_game_restores_starting_state() -> None:
    game = GameState(player_count=5, starting_life=30, partner_mode_enabled=True)
    add_poison(game, 0, 9)
    add_rad(game, 1, 4)
    add_energy(game, 2, 6)
    add_experience(game, 3, 2)
    add_ticket(game, 4, 3)
    add_commander_damage(game, 1, 0, 19, commander_slot=1)
    increase_commander_cast_count(game, 0, commander_slot=1)
    increase_commander_cast_count(game, 0, commander_slot=1)
    set_monarch(game, 2)
    set_initiative(game, 3)
    increase_storm_count(game, 5)
    next_turn(game)
    mark_player_eliminated(game, 4)
    game.day_night_state = DAY

    reset_game(game)

    assert game.player_count == 5
    assert game.partner_mode_enabled
    assert game.turn_number == 1
    assert game.active_player == 0
    assert game.storm_count == 0
    assert game.monarch_player is None
    assert game.initiative_player is None
    assert game.day_night_state == DAY_NIGHT_NONE
    assert all(player.life == 30 for player in game.players)
    assert all(not player.eliminated for player in game.players)
    assert all(player.poison == 0 for player in game.players)
    assert all(player.rad == 0 for player in game.players)
    assert all(player.energy == 0 for player in game.players)
    assert all(player.experience == 0 for player in game.players)
    assert all(player.ticket == 0 for player in game.players)
    assert all(player.commander_tax == [0, 0] for player in game.players)
    assert all(
        value == 0
        for target in game.commander_damage
        for source in target
        for value in source
    )
