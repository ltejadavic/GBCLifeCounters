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
    return current_field == DETAIL_FIELD_LIFE
        ? DETAIL_FIELD_POISON
        : DETAIL_FIELD_LIFE;
}

uint8_t navigation_next_detail_field(uint8_t current_field) {
    return current_field == DETAIL_FIELD_POISON
        ? DETAIL_FIELD_LIFE
        : DETAIL_FIELD_POISON;
}
