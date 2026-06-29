from prototype_python.actions import (
    confirm_possible_loss,
    mark_player_eliminated,
    restore_eliminated_player,
    set_life,
)
from prototype_python.game_state import GameState


def test_manual_elimination_and_restore() -> None:
    game = GameState()

    mark_player_eliminated(game, 1)
    assert game.players[1].eliminated

    restore_eliminated_player(game, 1)
    assert not game.players[1].eliminated


def test_confirm_possible_loss_requires_a_current_loss_condition() -> None:
    game = GameState()

    assert not confirm_possible_loss(game, 0)
    assert not game.players[0].eliminated

    set_life(game, 0, 0)
    assert confirm_possible_loss(game, 0)
    assert game.players[0].eliminated


def test_eliminating_active_player_selects_next_valid_player() -> None:
    game = GameState()

    mark_player_eliminated(game, 0)

    assert game.active_player == 1
    assert not game.players[game.active_player].eliminated
