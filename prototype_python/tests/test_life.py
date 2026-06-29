from prototype_python.actions import decrease_life, increase_life, reset_life, set_life
from prototype_python.game_state import GameState
from prototype_python.rules import REASON_LIFE, check_life


def test_life_can_increase_and_decrease() -> None:
    game = GameState()

    assert increase_life(game, 0, 5) == 45
    assert decrease_life(game, 0, 7) == 38


def test_life_can_go_below_zero_without_elimination() -> None:
    game = GameState()

    assert decrease_life(game, 0, 45) == -5
    result = check_life(game, 0)

    assert result is not None
    assert result.reason == REASON_LIFE
    assert result.has_warning
    assert result.has_loss_condition
    assert not game.players[0].eliminated


def test_life_warning_starts_at_five() -> None:
    game = GameState()

    set_life(game, 0, 6)
    assert check_life(game, 0) is None

    set_life(game, 0, 5)
    result = check_life(game, 0)
    assert result is not None
    assert result.has_warning
    assert not result.has_loss_condition


def test_set_and_reset_life() -> None:
    game = GameState(starting_life=30)

    set_life(game, 0, 12)
    assert reset_life(game, 0) == 30
