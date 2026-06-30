#ifndef COMMANDER_GBC_PLAYER_H
#define COMMANDER_GBC_PLAYER_H

#include <stdint.h>

#include "constants.h"

typedef struct Player {
    uint8_t player_id;
    char name[MAX_NAME_LENGTH + 1u];
    int16_t life;
    uint8_t poison;
    uint8_t rad;
    uint8_t energy;
    uint8_t experience;
    uint8_t ticket;
    uint8_t commander_tax[MAX_COMMANDERS_PER_PLAYER];
    uint8_t commander_cast_count[MAX_COMMANDERS_PER_PLAYER];
    uint8_t custom_counters[MAX_CUSTOM_COUNTERS];
    uint16_t commander_id;
    uint8_t eliminated;
    uint8_t is_monarch;
    uint8_t has_initiative;
    uint8_t has_citys_blessing;
} Player;

void player_init(
    Player *player,
    uint8_t player_id,
    const char *name,
    int16_t starting_life
);
void player_reset(Player *player, int16_t starting_life);

#endif
