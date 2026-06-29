import pytest

from prototype_python.actions import add_ticket, remove_ticket, set_ticket
from prototype_python.game_state import GameState


def test_ticket_can_increase_decrease_and_set() -> None:
    game = GameState()

    assert add_ticket(game, 0, 4) == 4
    assert remove_ticket(game, 0, 1) == 3
    assert set_ticket(game, 0, 7) == 7


def test_ticket_cannot_go_below_zero() -> None:
    game = GameState()

    add_ticket(game, 0, 2)
    assert remove_ticket(game, 0, 9) == 0
    with pytest.raises(ValueError):
        set_ticket(game, 0, -1)
