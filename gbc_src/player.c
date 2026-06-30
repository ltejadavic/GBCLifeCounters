#include "player.h"

static void copy_short_name(char *destination, const char *source) {
    uint8_t index = 0u;

    while ((index < MAX_NAME_LENGTH) && (source[index] != '\0')) {
        destination[index] = source[index];
        index++;
    }
    destination[index] = '\0';
}

void player_reset(Player *player, int16_t starting_life) {
    uint8_t index;

    player->life = starting_life;
    player->poison = 0u;
    player->rad = 0u;
    player->energy = 0u;
    player->experience = 0u;
    player->ticket = 0u;
    player->commander_id = NO_COMMANDER_ID;
    player->eliminated = 0u;
    player->is_monarch = 0u;
    player->has_initiative = 0u;
    player->has_citys_blessing = 0u;

    for (index = 0u; index < MAX_COMMANDERS_PER_PLAYER; index++) {
        player->commander_tax[index] = 0u;
        player->commander_cast_count[index] = 0u;
    }
    for (index = 0u; index < MAX_CUSTOM_COUNTERS; index++) {
        player->custom_counters[index] = 0u;
    }
}

void player_init(
    Player *player,
    uint8_t player_id,
    const char *name,
    int16_t starting_life
) {
    player->player_id = player_id;
    copy_short_name(player->name, name);
    player_reset(player, starting_life);
}
