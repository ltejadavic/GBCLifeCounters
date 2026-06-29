import pytest

from prototype_python.actions import add_poison, remove_poison, set_poison
from prototype_python.game_state import GameState
from prototype_python.rules import REASON_POISON, check_poison


def test_poison_can_increase_decrease_and_set() -> None:
    game = GameState()

    assert add_poison(game, 0, 5) == 5
    assert remove_poison(game, 0, 2) == 3
    assert set_poison(game, 0, 7) == 7


def test_poison_cannot_go_below_zero() -> None:
    game = GameState()

    add_poison(game, 0, 2)
    assert remove_poison(game, 0, 20) == 0
    with pytest.raises(ValueError):
        set_poison(game, 0, -1)


def test_poison_warning_at_eight() -> None:
    game = GameState()

    set_poison(game, 0, 8)
    result = check_poison(game, 0)

    assert result is not None
    assert result.reason == REASON_POISON
    assert result.has_warning
    assert not result.has_loss_condition


def test_poison_possible_loss_at_configured_threshold_is_not_automatic() -> None:
    game = GameState(poison_threshold=10)

    set_poison(game, 0, 10)
    result = check_poison(game, 0)

    assert result is not None
    assert result.has_loss_condition
    assert not game.players[0].eliminated
