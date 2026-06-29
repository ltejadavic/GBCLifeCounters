"""Player data model with fields that map cleanly to a future C struct."""

from .constants import (
    DEFAULT_STARTING_LIFE,
    MAX_COMMANDERS_PER_PLAYER,
    MAX_CUSTOM_COUNTERS,
    MAX_NAME_LENGTH,
)


class Player:
    """Mutable state owned by one player."""

    def __init__(
        self,
        player_id: int,
        name: str,
        starting_life: int = DEFAULT_STARTING_LIFE,
    ) -> None:
        if player_id < 0:
            raise ValueError("player_id cannot be negative")
        if not name:
            raise ValueError("player name cannot be empty")
        if len(name) > MAX_NAME_LENGTH:
            raise ValueError(f"player name cannot exceed {MAX_NAME_LENGTH} characters")

        self.player_id = player_id
        self.name = name
        self.life = starting_life
        self.poison = 0
        self.rad = 0
        self.energy = 0
        self.experience = 0
        self.ticket = 0
        self.eliminated = False
        self.commander_tax = [0] * MAX_COMMANDERS_PER_PLAYER
        self.commander_cast_count = [0] * MAX_COMMANDERS_PER_PLAYER
        self.custom_counters = [0] * MAX_CUSTOM_COUNTERS

        # Unique statuses are mirrored here for convenient player display. GameState
        # remains authoritative and ensures that only one holder exists.
        self.is_monarch = False
        self.has_initiative = False
        self.has_citys_blessing = False

    def reset(self, starting_life: int) -> None:
        """Reset gameplay values while keeping the player's ID and name."""
        self.life = starting_life
        self.poison = 0
        self.rad = 0
        self.energy = 0
        self.experience = 0
        self.ticket = 0
        self.eliminated = False
        self.commander_tax = [0] * MAX_COMMANDERS_PER_PLAYER
        self.commander_cast_count = [0] * MAX_COMMANDERS_PER_PLAYER
        self.custom_counters = [0] * MAX_CUSTOM_COUNTERS
        self.is_monarch = False
        self.has_initiative = False
        self.has_citys_blessing = False
