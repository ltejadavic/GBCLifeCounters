from prototype_python.actions import (
    add_commander_damage,
    get_commander_damage,
    get_commander_damage_received,
    remove_commander_damage,
    set_commander_damage,
)
from prototype_python.game_state import GameState
from prototype_python.rules import (
    REASON_COMMANDER_DAMAGE,
    check_commander_damage,
    check_commander_damage_possible_loss,
    check_commander_damage_warning,
)


def test_damage_is_tracked_by_source_player() -> None:
    game = GameState()

    add_commander_damage(game, 1, 0, 7)
    add_commander_damage(game, 1, 2, 4)

    assert get_commander_damage(game, 1, 0) == 7
    assert get_commander_damage(game, 1, 2) == 4


def test_damage_is_tracked_by_commander_slot() -> None:
    game = GameState(partner_mode_enabled=True)

    add_commander_damage(game, 1, 0, 12, commander_slot=0)
    add_commander_damage(game, 1, 0, 9, commander_slot=1)

    assert get_commander_damage(game, 1, 0, 0) == 12
    assert get_commander_damage(game, 1, 0, 1) == 9
    assert not check_commander_damage_possible_loss(game, 1)


def test_damage_from_different_commanders_does_not_combine() -> None:
    game = GameState(partner_mode_enabled=True)

    add_commander_damage(game, 1, 0, 12, 0)
    add_commander_damage(game, 1, 0, 9, 1)

    assert check_commander_damage(game, 1) == []


def test_commander_damage_warning_at_eighteen() -> None:
    game = GameState()
    set_commander_damage(game, 1, 0, 18)

    results = check_commander_damage(game, 1)

    assert len(results) == 1
    assert results[0].reason == REASON_COMMANDER_DAMAGE
    assert results[0].has_warning
    assert not results[0].has_loss_condition
    assert check_commander_damage_warning(game, 1, 0, 0)


def test_commander_damage_possible_loss_at_twenty_one_is_not_automatic() -> None:
    game = GameState()
    set_commander_damage(game, 1, 0, 21)

    results = check_commander_damage(game, 1)

    assert results[0].has_loss_condition
    assert results[0].source_player_id == 0
    assert results[0].commander_slot == 0
    assert check_commander_damage_possible_loss(game, 1, 0, 0)
    assert not game.players[1].eliminated


def test_commander_damage_can_be_removed_and_read_as_matrix() -> None:
    game = GameState()
    set_commander_damage(game, 1, 0, 10)

    assert remove_commander_damage(game, 1, 0, 3) == 7
    assert remove_commander_damage(game, 1, 0, 20) == 0
    assert get_commander_damage_received(game, 1)[0] == [0, 0]


def test_commander_damage_changes_life_and_removal_corrects_it() -> None:
    game = GameState()

    add_commander_damage(game, 1, 0, 5)
    assert game.players[1].life == 35

    remove_commander_damage(game, 1, 0, 2)
    assert game.players[1].life == 37

    remove_commander_damage(game, 1, 0, 20)
    assert game.players[1].life == 40
