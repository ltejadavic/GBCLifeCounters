#ifndef COMMANDER_GBC_UI_H
#define COMMANDER_GBC_UI_H

#include <stdint.h>

#include "game_state.h"

void ui_initialize(void);
void ui_show_overview(
    const GameState *game,
    uint8_t selected_player,
    uint8_t life_step
);
void ui_refresh_overview(
    const GameState *game,
    uint8_t selected_player,
    uint8_t life_step
);
void ui_show_player_detail(
    const GameState *game,
    uint8_t player_id,
    uint8_t selected_field,
    uint8_t adjustment_step
);
void ui_refresh_player_detail(
    const GameState *game,
    uint8_t player_id,
    uint8_t selected_field,
    uint8_t adjustment_step
);
void ui_show_commander_damage(
    const GameState *game,
    uint8_t target_player,
    uint8_t selected_source,
    uint8_t adjustment_step
);
void ui_refresh_commander_damage(
    const GameState *game,
    uint8_t target_player,
    uint8_t selected_source,
    uint8_t adjustment_step
);
void ui_draw_reset_prompt(void);
void ui_draw_elimination_prompt(const Player *player);

#endif
