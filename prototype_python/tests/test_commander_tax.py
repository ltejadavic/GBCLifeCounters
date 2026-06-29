from prototype_python.actions import (
    calculate_commander_tax,
    decrease_commander_cast_count,
    increase_commander_cast_count,
    set_commander_cast_count,
    set_commander_tax,
)
from prototype_python.game_state import GameState


def test_commander_tax_follows_cast_count() -> None:
    game = GameState()

    expected_taxes = [0, 2, 4, 6]
    for expected_tax in expected_taxes:
        increase_commander_cast_count(game, 0)
        assert game.players[0].commander_tax[0] == expected_tax

    assert game.players[0].commander_cast_count[0] == 4


def test_commander_tax_calculation_and_count_decrease() -> None:
    game = GameState()

    assert calculate_commander_tax(0) == 0
    assert calculate_commander_tax(3) == 4
    set_commander_cast_count(game, 0, 3)
    assert decrease_commander_cast_count(game, 0) == 2
    assert game.players[0].commander_tax[0] == 2


def test_partner_commander_tax_is_separate() -> None:
    game = GameState(partner_mode_enabled=True)

    set_commander_cast_count(game, 0, 3, commander_slot=0)
    set_commander_cast_count(game, 0, 2, commander_slot=1)

    assert game.players[0].commander_tax == [4, 2]


def test_commander_tax_can_be_overridden_manually() -> None:
    game = GameState()

    set_commander_cast_count(game, 0, 3)
    assert set_commander_tax(game, 0, 7) == 7
    assert game.players[0].commander_tax[0] == 7
