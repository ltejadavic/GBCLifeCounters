#ifndef COMMANDER_GBC_GAME_STATE_H
#define COMMANDER_GBC_GAME_STATE_H

#include <stdint.h>

#include "constants.h"
#include "player.h"

typedef struct GameState {
    uint8_t player_count;
    int16_t starting_life;
    Player players[MAX_PLAYERS];
    uint8_t poison_threshold;
    uint8_t commander_damage_threshold;
    uint8_t partner_mode_enabled;
    uint8_t commander_damage[MAX_PLAYERS][MAX_PLAYERS][MAX_COMMANDERS_PER_PLAYER];
    uint16_t turn_number;
    int8_t active_player;
    int8_t monarch_player;
    int8_t initiative_player;
    uint8_t storm_count;
    uint8_t day_night_state;
    uint8_t confirm_eliminations;
} GameState;

void game_state_init(
    GameState *game,
    uint8_t player_count,
    int16_t starting_life
);
void game_state_reset(GameState *game);

#endif
