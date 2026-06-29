from prototype_python.actions import mark_player_eliminated
from prototype_python.game_state import GameState
from prototype_python.rules import check_winner, get_active_players


def test_no_winner_when_more_than_one_player_remains() -> None:
    game = GameState()

    assert check_winner(game) is None
    assert get_active_players(game) == [0, 1, 2, 3]


def test_winner_when_one_player_remains() -> None:
    game = GameState()
    for player_id in [0, 1, 3]:
        mark_player_eliminated(game, player_id)

    assert get_active_players(game) == [2]
    assert check_winner(game) == 2


def test_no_winner_when_no_players_remain() -> None:
    game = GameState(player_count=2)
    mark_player_eliminated(game, 0)
    mark_player_eliminated(game, 1)

    assert check_winner(game) is None
