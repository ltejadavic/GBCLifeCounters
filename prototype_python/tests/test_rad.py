import pytest

from prototype_python.actions import add_rad, apply_rad_helper, remove_rad, set_rad
from prototype_python.game_state import GameState
from prototype_python.rules import REASON_LIFE


def test_rad_can_increase_decrease_and_set() -> None:
    game = GameState()

    assert add_rad(game, 0, 5) == 5
    assert remove_rad(game, 0, 2) == 3
    assert set_rad(game, 0, 8) == 8


def test_rad_does_not_go_below_zero() -> None:
    game = GameState()

    add_rad(game, 0, 2)
    assert remove_rad(game, 0, 5) == 0
    with pytest.raises(ValueError):
        set_rad(game, 0, -1)


def test_rad_helper_removes_life_and_rad() -> None:
    game = GameState()
    set_rad(game, 0, 5)

    results = apply_rad_helper(game, 0, 3)

    assert game.players[0].life == 37
    assert game.players[0].rad == 2
    assert results == []


def test_rad_helper_checks_possible_loss() -> None:
    game = GameState(starting_life=1)
    set_rad(game, 0, 3)

    results = apply_rad_helper(game, 0, 2)

    assert any(
        result.reason == REASON_LIFE and result.has_loss_condition
        for result in results
    )
    assert not game.players[0].eliminated


def test_rad_helper_rejects_more_nonlands_than_rad() -> None:
    game = GameState()
    set_rad(game, 0, 2)

    with pytest.raises(ValueError):
        apply_rad_helper(game, 0, 3)
