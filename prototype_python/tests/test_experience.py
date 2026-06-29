import pytest

from prototype_python.actions import add_experience, remove_experience, set_experience
from prototype_python.game_state import GameState


def test_experience_can_increase_decrease_and_set() -> None:
    game = GameState()

    assert add_experience(game, 0, 5) == 5
    assert remove_experience(game, 0, 2) == 3
    assert set_experience(game, 0, 8) == 8


def test_experience_cannot_go_below_zero() -> None:
    game = GameState()

    add_experience(game, 0, 2)
    assert remove_experience(game, 0, 5) == 0
    with pytest.raises(ValueError):
        set_experience(game, 0, -1)
