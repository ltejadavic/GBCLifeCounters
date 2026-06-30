#ifndef COMMANDER_GBC_UI_H
#define COMMANDER_GBC_UI_H

#include <stdint.h>

#include "game_state.h"

void ui_initialize(void);
void ui_draw_overview(
    const GameState *game,
    uint8_t selected_player,
    uint8_t life_step
);
void ui_draw_reset_prompt(void);

#endif
