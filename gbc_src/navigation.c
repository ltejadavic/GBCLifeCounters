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
    if (
        (current_field == DETAIL_FIELD_COMMANDER)
        || (current_field >= DETAIL_FIELD_COUNT)
    ) {
        return (DETAIL_FIELD_COUNT - 1u);
    }
    return (uint8_t)(current_field - 1u);
}

uint8_t navigation_next_detail_field(uint8_t current_field) {
    if (current_field >= (DETAIL_FIELD_COUNT - 1u)) {
        return DETAIL_FIELD_COMMANDER;
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
