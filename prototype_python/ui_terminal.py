"""Small text UI that delegates all game logic to the rules engine."""

from collections.abc import Callable

from . import actions
from .constants import DEFAULT_PLAYERS, MAX_PLAYERS, MIN_PLAYERS
from .game_state import GameState
from .rules import check_all_players, check_winner


def _read_int(prompt: str, default: int | None = None) -> int:
    while True:
        suffix = f" [{default}]" if default is not None else ""
        text = input(f"{prompt}{suffix}: ").strip()
        if not text and default is not None:
            return default
        try:
            return int(text)
        except ValueError:
            print("Enter a whole number.")


def _choose_player(game: GameState, prompt: str = "Player") -> int:
    while True:
        number = _read_int(f"{prompt} (1-{game.player_count})")
        player_id = number - 1
        try:
            game.get_player(player_id)
            return player_id
        except ValueError as error:
            print(error)


def _show_game(game: GameState) -> None:
    print("\n=== Commander GBC Life Counter ===")
    active_label = (
        game.players[game.active_player].name if game.active_player is not None else "none"
    )
    print(
        f"Turn {game.turn_number} | Active: {active_label} | "
        f"Storm: {game.storm_count}"
    )
    for player in game.players:
        markers: list[str] = []
        if player.player_id == game.monarch_player:
            markers.append("MON")
        if player.player_id == game.initiative_player:
            markers.append("INI")
        if player.eliminated:
            markers.append("OUT")
        marker_text = f" [{' '.join(markers)}]" if markers else ""
        print(
            f"{player.player_id + 1}. {player.name:<8} LIFE {player.life:>3}  "
            f"PSN {player.poison:>2}  RAD {player.rad:>2}{marker_text}"
        )
    winner_id = check_winner(game)
    if winner_id is not None:
        print(f"Winner: {game.players[winner_id].name}")


def _change_value(
    add_action: Callable[[GameState, int, int], int],
    remove_action: Callable[[GameState, int, int], int],
    game: GameState,
    player_id: int,
) -> None:
    amount = _read_int("Change (positive adds, negative removes)")
    if amount >= 0:
        value = add_action(game, player_id, amount)
    else:
        value = remove_action(game, player_id, -amount)
    print(f"New value: {value}")


def _edit_player(game: GameState) -> None:
    player_id = _choose_player(game)
    while True:
        player = game.players[player_id]
        print(
            f"\n{player.name}: life={player.life}, poison={player.poison}, "
            f"rad={player.rad}, energy={player.energy}, "
            f"experience={player.experience}, tickets={player.ticket}"
        )
        print(
            "1 Life  2 Poison  3 Rad  4 Rad helper  5 Energy  "
            "6 Experience  7 Tickets  8 Commander tax  9 Back"
        )
        choice = input("> ").strip().lower()
        try:
            if choice == "1":
                amount = _read_int("Life change")
                if amount >= 0:
                    actions.increase_life(game, player_id, amount)
                else:
                    actions.decrease_life(game, player_id, -amount)
            elif choice == "2":
                _change_value(actions.add_poison, actions.remove_poison, game, player_id)
            elif choice == "3":
                _change_value(actions.add_rad, actions.remove_rad, game, player_id)
            elif choice == "4":
                nonlands = _read_int("Nonland cards milled")
                results = actions.apply_rad_helper(game, player_id, nonlands)
                for result in results:
                    print(result.message)
            elif choice == "5":
                _change_value(actions.add_energy, actions.spend_energy, game, player_id)
            elif choice == "6":
                _change_value(
                    actions.add_experience,
                    actions.remove_experience,
                    game,
                    player_id,
                )
            elif choice == "7":
                _change_value(actions.add_ticket, actions.remove_ticket, game, player_id)
            elif choice == "8":
                _edit_commander_tax(game, player_id)
            elif choice in {"9", "b", "back"}:
                return
            else:
                print("Unknown option.")
        except ValueError as error:
            print(error)


def _edit_commander_tax(game: GameState, player_id: int) -> None:
    slot = _read_int("Commander slot (1=A, 2=B)", 1) - 1
    print("1 Record cast  2 Undo cast  3 Set cast count  4 Set tax manually")
    choice = input("> ").strip()
    if choice == "1":
        actions.increase_commander_cast_count(game, player_id, slot)
    elif choice == "2":
        actions.decrease_commander_cast_count(game, player_id, slot)
    elif choice == "3":
        actions.set_commander_cast_count(
            game, player_id, _read_int("Cast count"), slot
        )
    elif choice == "4":
        actions.set_commander_tax(game, player_id, _read_int("Commander tax"), slot)
    player = game.players[player_id]
    print(
        f"Cast count: {player.commander_cast_count[slot]}, "
        f"tax: {player.commander_tax[slot]}"
    )


def _edit_commander_damage(game: GameState) -> None:
    target_id = _choose_player(game, "Target player")
    source_id = _choose_player(game, "Source player")
    slot = _read_int("Source commander slot (1=A, 2=B)", 1) - 1
    print("1 Add  2 Remove  3 Set exact")
    choice = input("> ").strip()
    value = _read_int("Amount")
    if choice == "1":
        new_value = actions.add_commander_damage(
            game, target_id, source_id, value, slot
        )
    elif choice == "2":
        new_value = actions.remove_commander_damage(
            game, target_id, source_id, value, slot
        )
    elif choice == "3":
        new_value = actions.set_commander_damage(
            game, target_id, source_id, value, slot
        )
    else:
        print("Unknown option.")
        return
    print(f"Commander damage is now {new_value}.")


def _set_unique_status(game: GameState, status: str) -> None:
    text = input(f"{status} player (1-{game.player_count}, 0 to clear): ").strip()
    try:
        number = int(text)
        player_id = None if number == 0 else number - 1
        if status == "Monarch":
            actions.set_monarch(game, player_id)
        else:
            actions.set_initiative(game, player_id)
    except ValueError as error:
        print(error)


def _show_warnings(game: GameState) -> None:
    results = check_all_players(game)
    if not results:
        print("No warnings or possible loss conditions.")
        return
    for result in results:
        prefix = "POSSIBLE LOSS" if result.has_loss_condition else "WARNING"
        print(f"{prefix}: {result.message}")


def _elimination_menu(game: GameState) -> None:
    player_id = _choose_player(game)
    player = game.players[player_id]
    if player.eliminated:
        actions.restore_eliminated_player(game, player_id)
        print(f"{player.name} restored.")
        return

    if actions.confirm_possible_loss(game, player_id):
        print(f"{player.name} eliminated after confirming a possible loss.")
        return

    answer = input("No detected loss. Mark eliminated manually? [y/N]: ").strip().lower()
    if answer == "y":
        actions.mark_player_eliminated(game, player_id)


def _new_game() -> GameState:
    while True:
        player_count = _read_int(
            f"Player count ({MIN_PLAYERS}-{MAX_PLAYERS})", DEFAULT_PLAYERS
        )
        try:
            return GameState(player_count=player_count)
        except ValueError as error:
            print(error)


def run_terminal() -> None:
    """Run the interactive terminal prototype until the user quits."""
    print("Commander GBC Life Counter - Python prototype")
    game = _new_game()
    while True:
        _show_game(game)
        print(
            "\n1 Player counters  2 Commander damage  3 Monarch  4 Initiative\n"
            "5 Next turn       6 Storm            7 Warnings  8 Eliminate/restore\n"
            "9 Reset game      0 Quit"
        )
        try:
            choice = input("> ").strip().lower()
            if choice == "1":
                _edit_player(game)
            elif choice == "2":
                _edit_commander_damage(game)
            elif choice == "3":
                _set_unique_status(game, "Monarch")
            elif choice == "4":
                _set_unique_status(game, "Initiative")
            elif choice == "5":
                actions.next_turn(game)
            elif choice == "6":
                actions.set_storm_count(game, _read_int("Storm count"))
            elif choice == "7":
                _show_warnings(game)
            elif choice == "8":
                _elimination_menu(game)
            elif choice == "9":
                if input("Reset the game? [y/N]: ").strip().lower() == "y":
                    actions.reset_game(game)
            elif choice in {"0", "q", "quit"}:
                return
            else:
                print("Unknown option.")
        except ValueError as error:
            print(error)
        except (EOFError, KeyboardInterrupt):
            print()
            return
