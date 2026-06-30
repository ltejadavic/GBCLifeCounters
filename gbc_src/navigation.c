#include "navigation.h"

uint8_t navigation_previous_player(
    const GameState *game,
    uint8_t current_player
) {
    if ((game->player_count == 0u) || (current_player >= game->player_count)) {
        return 0u;
    }
    if (current_player == 0u) {
        return (uint8_t)(game->player_count - 1u);
    }
    return (uint8_t)(current_player - 1u);
}

uint8_t navigation_next_player(
    const GameState *game,
    uint8_t current_player
) {
    if ((game->player_count == 0u) || (current_player >= game->player_count)) {
        return 0u;
    }
    current_player++;
    if (current_player >= game->player_count) {
        return 0u;
    }
    return current_player;
}

uint8_t navigation_previous_other_player(
    const GameState *game,
    uint8_t current_player,
    uint8_t excluded_player
) {
    uint8_t candidate = navigation_previous_player(game, current_player);

    if (candidate == excluded_player) {
        candidate = navigation_previous_player(game, candidate);
    }
    return candidate;
}

uint8_t navigation_next_other_player(
    const GameState *game,
    uint8_t current_player,
    uint8_t excluded_player
) {
    uint8_t candidate = navigation_next_player(game, current_player);

    if (candidate == excluded_player) {
        candidate = navigation_next_player(game, candidate);
    }
    return candidate;
}

int8_t navigation_previous_active_player(
    const GameState *game,
    int8_t current_player
) {
    uint8_t offset;
    uint8_t start = current_player == NO_PLAYER
        ? 0u
        : (uint8_t)current_player;

    for (offset = 1u; offset <= game->player_count; offset++) {
        uint8_t candidate = (uint8_t)(
            (start + game->player_count - offset) % game->player_count
        );
        if (!game->players[candidate].eliminated) {
            return (int8_t)candidate;
        }
    }
    return NO_PLAYER;
}

int8_t navigation_next_active_player(
    const GameState *game,
    int8_t current_player
) {
    uint8_t offset;
    uint8_t start = current_player == NO_PLAYER
        ? (uint8_t)(game->player_count - 1u)
        : (uint8_t)current_player;

    for (offset = 1u; offset <= game->player_count; offset++) {
        uint8_t candidate = (uint8_t)(
            (start + offset) % game->player_count
        );
        if (!game->players[candidate].eliminated) {
            return (int8_t)candidate;
        }
    }
    return NO_PLAYER;
}

int8_t navigation_previous_status_player(
    const GameState *game,
    int8_t current_player
) {
    int8_t candidate;

    if (current_player == NO_PLAYER) {
        candidate = (int8_t)(game->player_count - 1u);
    } else {
        candidate = (int8_t)(current_player - 1);
    }
    while (candidate >= 0) {
        if (!game->players[(uint8_t)candidate].eliminated) {
            return candidate;
        }
        candidate--;
    }
    return NO_PLAYER;
}

int8_t navigation_next_status_player(
    const GameState *game,
    int8_t current_player
) {
    uint8_t candidate = current_player == NO_PLAYER
        ? 0u
        : (uint8_t)(current_player + 1);

    while (candidate < game->player_count) {
        if (!game->players[candidate].eliminated) {
            return (int8_t)candidate;
        }
        candidate++;
    }
    return NO_PLAYER;
}

uint8_t navigation_next_life_step(uint8_t current_step) {
    if (current_step == LIFE_STEP_SMALL) {
        return LIFE_STEP_MEDIUM;
    }
    if (current_step == LIFE_STEP_MEDIUM) {
        return LIFE_STEP_LARGE;
    }
    return LIFE_STEP_SMALL;
}

uint8_t navigation_previous_detail_field(uint8_t current_field) {
    if (current_field == DETAIL_FIELD_LIFE) {
        return DETAIL_FIELD_COMMANDER_DAMAGE;
    }
    return (uint8_t)(current_field - 1u);
}

uint8_t navigation_next_detail_field(uint8_t current_field) {
    if (current_field >= DETAIL_FIELD_COMMANDER_DAMAGE) {
        return DETAIL_FIELD_LIFE;
    }
    return (uint8_t)(current_field + 1u);
}

uint8_t navigation_update_window_start(
    const GameState *game,
    uint8_t selected_player,
    uint8_t current_start,
    uint8_t visible_count
) {
    uint8_t maximum_start;

    if (
        (visible_count == 0u)
        || (game->player_count <= visible_count)
        || (selected_player >= game->player_count)
    ) {
        return 0u;
    }

    maximum_start = (uint8_t)(game->player_count - visible_count);
    if (current_start > maximum_start) {
        current_start = maximum_start;
    }
    if (selected_player < current_start) {
        return selected_player;
    }
    if (selected_player >= (uint8_t)(current_start + visible_count)) {
        uint8_t new_start = (uint8_t)(selected_player - visible_count + 1u);
        return new_start > maximum_start ? maximum_start : new_start;
    }
    return current_start;
}
