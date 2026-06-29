import pytest

from prototype_python.constants import MAX_PLAYERS, MIN_PLAYERS
from prototype_python.game_state import GameState


def test_default_game_has_four_named_players() -> None:
    game = GameState()

    assert game.player_count == 4
    assert [player.name for player in game.players] == ["P1", "P2", "P3", "P4"]
    assert game.active_player == 0
    assert game.turn_number == 1


def test_game_supports_target_maximum_players() -> None:
    game = GameState(player_count=MAX_PLAYERS)

    assert len(game.players) == 8
    assert len(game.commander_damage) == 8
    assert len(game.commander_damage[0]) == 8
    assert len(game.commander_damage[0][0]) == 2


@pytest.mark.parametrize("player_count", [MIN_PLAYERS - 1, MAX_PLAYERS + 1])
def test_player_count_outside_supported_range_is_rejected(player_count: int) -> None:
    with pytest.raises(ValueError):
        GameState(player_count=player_count)
