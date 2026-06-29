"""Top-level game-state model for the portable rules engine."""

from typing import Optional

from .constants import (
    DAY_NIGHT_NONE,
    DEFAULT_COMMANDER_DAMAGE_THRESHOLD,
    DEFAULT_PLAYERS,
    DEFAULT_POISON_THRESHOLD,
    DEFAULT_STARTING_LIFE,
    MAX_COMMANDERS_PER_PLAYER,
    MAX_PLAYERS,
    MIN_PLAYERS,
)
from .player import Player


class GameState:
    """All state for one Commander game.

    Player IDs are zero-based indexes. The commander damage matrix is indexed as
    ``[target_player][source_player][commander_slot]``.
    """

    def __init__(
        self,
        player_count: int = DEFAULT_PLAYERS,
        starting_life: int = DEFAULT_STARTING_LIFE,
        poison_threshold: int = DEFAULT_POISON_THRESHOLD,
        commander_damage_threshold: int = DEFAULT_COMMANDER_DAMAGE_THRESHOLD,
        partner_mode_enabled: bool = False,
        confirm_eliminations: bool = True,
    ) -> None:
        if player_count < MIN_PLAYERS or player_count > MAX_PLAYERS:
            raise ValueError(
                f"player_count must be between {MIN_PLAYERS} and {MAX_PLAYERS}"
            )
        if poison_threshold <= 0:
            raise ValueError("poison_threshold must be positive")
        if commander_damage_threshold <= 0:
            raise ValueError("commander_damage_threshold must be positive")

        self.player_count = player_count
        self.starting_life = starting_life
        self.players = [
            Player(player_id, f"P{player_id + 1}", starting_life)
            for player_id in range(player_count)
        ]
        self.poison_threshold = poison_threshold
        self.commander_damage_threshold = commander_damage_threshold
        self.partner_mode_enabled = partner_mode_enabled
        self.commander_damage = self._new_commander_damage_matrix()
        self.turn_number = 1
        self.active_player: Optional[int] = 0
        self.monarch_player: Optional[int] = None
        self.initiative_player: Optional[int] = None
        self.storm_count = 0
        self.day_night_state = DAY_NIGHT_NONE
        self.confirm_eliminations = confirm_eliminations

    def _new_commander_damage_matrix(self) -> list[list[list[int]]]:
        return [
            [
                [0] * MAX_COMMANDERS_PER_PLAYER
                for _source in range(self.player_count)
            ]
            for _target in range(self.player_count)
        ]

    def get_player(self, player_id: int) -> Player:
        if player_id < 0 or player_id >= self.player_count:
            raise ValueError(f"invalid player_id: {player_id}")
        return self.players[player_id]

    def validate_commander_slot(self, commander_slot: int) -> None:
        if commander_slot < 0 or commander_slot >= MAX_COMMANDERS_PER_PLAYER:
            raise ValueError(f"invalid commander_slot: {commander_slot}")

    def reset_commander_damage(self) -> None:
        self.commander_damage = self._new_commander_damage_matrix()
