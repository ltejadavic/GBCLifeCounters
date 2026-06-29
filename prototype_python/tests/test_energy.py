import pytest

from prototype_python.actions import add_energy, clear_energy, set_energy, spend_energy
from prototype_python.game_state import GameState


def test_energy_can_be_added_spent_set_and_cleared() -> None:
    game = GameState()

    assert add_energy(game, 0, 6) == 6
    assert spend_energy(game, 0, 2) == 4
    assert set_energy(game, 0, 9) == 9
    assert clear_energy(game, 0) == 0


def test_energy_cannot_go_below_zero_by_default() -> None:
    game = GameState()
    add_energy(game, 0, 2)

    with pytest.raises(ValueError):
        spend_energy(game, 0, 3)
    with pytest.raises(ValueError):
        set_energy(game, 0, -1)

    assert game.players[0].energy == 2


def test_controlled_setter_can_allow_negative_energy() -> None:
    game = GameState()

    assert set_energy(game, 0, -2, allow_negative=True) == -2
