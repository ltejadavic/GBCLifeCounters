#ifndef COMMANDER_GBC_UI_H
#define COMMANDER_GBC_UI_H

#include "game_state.h"

void ui_initialize(void);
void ui_refresh_player(const GameState *game, uint8_t player_id);

#endif
