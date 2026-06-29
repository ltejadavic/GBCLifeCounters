import pytest

from prototype_python.actions import (
    clear_initiative,
    clear_monarch,
    set_initiative,
    set_monarch,
)
from prototype_python.game_state import GameState


def test_only_one_player_can_be_monarch_and_it_can_be_cleared() -> None:
    game = GameState()

    set_monarch(game, 0)
    set_monarch(game, 2)

    assert game.monarch_player == 2
    assert not game.players[0].is_monarch
    assert game.players[2].is_monarch

    clear_monarch(game)
    assert game.monarch_player is None
    assert not any(player.is_monarch for player in game.players)


def test_only_one_player_can_have_initiative_and_it_can_be_cleared() -> None:
    game = GameState()

    set_initiative(game, 1)
    set_initiative(game, 3)

    assert game.initiative_player == 3
    assert not game.players[1].has_initiative
    assert game.players[3].has_initiative

    clear_initiative(game)
    assert game.initiative_player is None
    assert not any(player.has_initiative for player in game.players)


def test_eliminated_player_cannot_receive_unique_status() -> None:
    game = GameState()
    game.players[0].eliminated = True

    with pytest.raises(ValueError):
        set_monarch(game, 0)
    with pytest.raises(ValueError):
        set_initiative(game, 0)
