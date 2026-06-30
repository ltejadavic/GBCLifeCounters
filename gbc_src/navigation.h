#ifndef COMMANDER_GBC_NAVIGATION_H
#define COMMANDER_GBC_NAVIGATION_H

#include <stdint.h>

#include "game_state.h"

#define LIFE_STEP_SMALL 1u
#define LIFE_STEP_MEDIUM 5u
#define LIFE_STEP_LARGE 10u
#define DETAIL_FIELD_LIFE 0u
#define DETAIL_FIELD_POISON 1u
#define DETAIL_FIELD_COMMANDER_DAMAGE 2u
#define SETUP_FIELD_PLAYER_COUNT 0u
#define SETUP_FIELD_STARTING_LIFE 1u

uint8_t navigation_previous_player(
    const GameState *game,
    uint8_t current_player
);
uint8_t navigation_next_player(
    const GameState *game,
    uint8_t current_player
);
uint8_t navigation_previous_other_player(
    const GameState *game,
    uint8_t current_player,
    uint8_t excluded_player
);
uint8_t navigation_next_other_player(
    const GameState *game,
    uint8_t current_player,
    uint8_t excluded_player
);
uint8_t navigation_next_life_step(uint8_t current_step);
uint8_t navigation_previous_detail_field(uint8_t current_field);
uint8_t navigation_next_detail_field(uint8_t current_field);
uint8_t navigation_update_window_start(
    const GameState *game,
    uint8_t selected_player,
    uint8_t current_start,
    uint8_t visible_count
);

#endif
