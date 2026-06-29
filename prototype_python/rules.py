"""Warning, possible-loss, and winner checks with no state mutations."""

from .constants import (
    COMMANDER_DAMAGE_WARNING_THRESHOLD,
    LIFE_WARNING_THRESHOLD,
    POISON_WARNING_THRESHOLD,
)
from .game_state import GameState

REASON_LIFE = "life"
REASON_POISON = "poison"
REASON_COMMANDER_DAMAGE = "commander_damage"


class RuleResult:
    """One warning or possible loss detected for one player."""

    def __init__(
        self,
        player_id: int,
        has_warning: bool,
        has_loss_condition: bool,
        reason: str,
        value: int,
        threshold: int,
        message: str,
        source_player_id: int | None = None,
        commander_slot: int | None = None,
    ) -> None:
        self.player_id = player_id
        self.has_warning = has_warning
        self.has_loss_condition = has_loss_condition
        self.reason = reason
        self.source_player_id = source_player_id
        self.commander_slot = commander_slot
        self.value = value
        self.threshold = threshold
        self.message = message


def check_life(game: GameState, player_id: int) -> RuleResult | None:
    player = game.get_player(player_id)
    has_warning = player.life <= LIFE_WARNING_THRESHOLD
    has_loss = player.life <= 0
    if not has_warning and not has_loss:
        return None
    message = (
        f"{player.name} may lose: life is {player.life}."
        if has_loss
        else f"{player.name} has low life: {player.life}."
    )
    return RuleResult(
        player_id,
        has_warning,
        has_loss,
        REASON_LIFE,
        player.life,
        0 if has_loss else LIFE_WARNING_THRESHOLD,
        message,
    )


def check_poison(game: GameState, player_id: int) -> RuleResult | None:
    player = game.get_player(player_id)
    has_warning = player.poison >= POISON_WARNING_THRESHOLD
    has_loss = player.poison >= game.poison_threshold
    if not has_warning and not has_loss:
        return None
    message = (
        f"{player.name} may lose: poison is {player.poison}."
        if has_loss
        else f"{player.name} is near the poison limit: {player.poison}."
    )
    return RuleResult(
        player_id,
        has_warning,
        has_loss,
        REASON_POISON,
        player.poison,
        game.poison_threshold if has_loss else POISON_WARNING_THRESHOLD,
        message,
    )


def check_commander_damage(
    game: GameState, target_player_id: int
) -> list[RuleResult]:
    target = game.get_player(target_player_id)
    results: list[RuleResult] = []
    for source_player_id in range(game.player_count):
        for commander_slot in range(len(game.commander_damage[target_player_id][0])):
            value = game.commander_damage[target_player_id][source_player_id][
                commander_slot
            ]
            has_warning = value >= COMMANDER_DAMAGE_WARNING_THRESHOLD
            has_loss = value >= game.commander_damage_threshold
            if not has_warning and not has_loss:
                continue
            source = game.players[source_player_id]
            slot_name = "A" if commander_slot == 0 else "B"
            message = (
                f"{target.name} may lose: {value} commander damage from "
                f"{source.name} commander {slot_name}."
                if has_loss
                else f"{target.name} has {value} commander damage from "
                f"{source.name} commander {slot_name}."
            )
            results.append(
                RuleResult(
                    target_player_id,
                    has_warning,
                    has_loss,
                    REASON_COMMANDER_DAMAGE,
                    value,
                    (
                        game.commander_damage_threshold
                        if has_loss
                        else COMMANDER_DAMAGE_WARNING_THRESHOLD
                    ),
                    message,
                    source_player_id,
                    commander_slot,
                )
            )
    return results


def check_commander_damage_warning(
    game: GameState,
    target_player_id: int,
    source_player_id: int | None = None,
    commander_slot: int | None = None,
) -> bool:
    return _commander_result_matches(
        check_commander_damage(game, target_player_id),
        source_player_id,
        commander_slot,
        require_loss=False,
    )


def check_commander_damage_possible_loss(
    game: GameState,
    target_player_id: int,
    source_player_id: int | None = None,
    commander_slot: int | None = None,
) -> bool:
    return _commander_result_matches(
        check_commander_damage(game, target_player_id),
        source_player_id,
        commander_slot,
        require_loss=True,
    )


def _commander_result_matches(
    results: list[RuleResult],
    source_player_id: int | None,
    commander_slot: int | None,
    require_loss: bool,
) -> bool:
    for result in results:
        if source_player_id is not None and result.source_player_id != source_player_id:
            continue
        if commander_slot is not None and result.commander_slot != commander_slot:
            continue
        if require_loss and not result.has_loss_condition:
            continue
        if not require_loss and not result.has_warning:
            continue
        return True
    return False


def check_player_conditions(game: GameState, player_id: int) -> list[RuleResult]:
    game.get_player(player_id)
    results: list[RuleResult] = []
    life_result = check_life(game, player_id)
    if life_result is not None:
        results.append(life_result)
    poison_result = check_poison(game, player_id)
    if poison_result is not None:
        results.append(poison_result)
    results.extend(check_commander_damage(game, player_id))
    return results


def check_all_players(game: GameState) -> list[RuleResult]:
    results: list[RuleResult] = []
    for player in game.players:
        if not player.eliminated:
            results.extend(check_player_conditions(game, player.player_id))
    return results


def get_active_players(game: GameState) -> list[int]:
    return [player.player_id for player in game.players if not player.eliminated]


def check_winner(game: GameState) -> int | None:
    active_players = get_active_players(game)
    if len(active_players) == 1:
        return active_players[0]
    return None
