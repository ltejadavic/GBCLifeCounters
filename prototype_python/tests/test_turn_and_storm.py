import pytest

from prototype_python.actions import (
    decrease_storm_count,
    increase_storm_count,
    mark_player_eliminated,
    next_turn,
    previous_turn,
    reset_storm_count,
    set_active_player,
    set_storm_count,
)
from prototype_python.game_state import GameState


def test_storm_count_changes_and_never_goes_below_zero() -> None:
    game = GameState()

    assert increase_storm_count(game, 4) == 4
    assert decrease_storm_count(game, 2) == 2
    assert decrease_storm_count(game, 9) == 0
    assert set_storm_count(game, 5) == 5
    assert reset_storm_count(game) == 0
    with pytest.raises(ValueError):
        set_storm_count(game, -1)


def test_turn_advances_and_skips_eliminated_players() -> None:
    game = GameState()
    mark_player_eliminated(game, 1)

    assert next_turn(game) == 2
    assert game.turn_number == 2
    assert previous_turn(game) == 0
    assert game.turn_number == 1


def test_active_player_must_be_valid_and_not_eliminated() -> None:
    game = GameState(player_count=8)

    assert set_active_player(game, 7) == 7
    mark_player_eliminated(game, 6)
    with pytest.raises(ValueError):
        set_active_player(game, 6)
    with pytest.raises(ValueError):
        set_active_player(game, 8)
