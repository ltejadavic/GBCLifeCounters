#ifndef COMMANDER_GBC_ACTIONS_H
#define COMMANDER_GBC_ACTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "game_state.h"

bool action_change_life(GameState *game, uint8_t player_id, int16_t amount);
bool action_set_life(GameState *game, uint8_t player_id, int16_t value);
bool action_change_poison(GameState *game, uint8_t player_id, int16_t amount);
bool action_set_poison(GameState *game, uint8_t player_id, uint8_t value);
bool action_change_commander_damage(
    GameState *game,
    uint8_t target_player,
    uint8_t source_player,
    uint8_t commander_slot,
    int16_t amount
);
bool action_set_commander_damage(
    GameState *game,
    uint8_t target_player,
    uint8_t source_player,
    uint8_t commander_slot,
    uint8_t value
);
bool action_eliminate_player(GameState *game, uint8_t player_id);
bool action_restore_player(GameState *game, uint8_t player_id);
bool action_reset_player(GameState *game, uint8_t player_id);

#endif
