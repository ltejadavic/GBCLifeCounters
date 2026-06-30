#include "actions.h"

static bool is_valid_player(const GameState *game, uint8_t player_id) {
    return player_id < game->player_count;
}

bool action_change_life(GameState *game, uint8_t player_id, int16_t amount) {
    Player *player;

    if (!is_valid_player(game, player_id)) {
        return false;
    }

    player = &game->players[player_id];
    if ((amount > 0) && (player->life > (MAX_LIFE_TOTAL - amount))) {
        player->life = MAX_LIFE_TOTAL;
    } else if ((amount < 0) && (player->life < (MIN_LIFE_TOTAL - amount))) {
        player->life = MIN_LIFE_TOTAL;
    } else {
        player->life = (int16_t)(player->life + amount);
    }
    return true;
}

bool action_set_life(GameState *game, uint8_t player_id, int16_t value) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    game->players[player_id].life = value;
    return true;
}

bool action_change_poison(GameState *game, uint8_t player_id, int16_t amount) {
    Player *player;
    int16_t current;

    if (!is_valid_player(game, player_id)) {
        return false;
    }

    player = &game->players[player_id];
    current = (int16_t)player->poison;
    if (amount > (255 - current)) {
        player->poison = 255u;
    } else if (amount < -current) {
        player->poison = 0u;
    } else {
        player->poison = (uint8_t)(current + amount);
    }
    return true;
}

bool action_set_poison(GameState *game, uint8_t player_id, uint8_t value) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    game->players[player_id].poison = value;
    return true;
}

bool action_reset_player(GameState *game, uint8_t player_id) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    player_reset(&game->players[player_id], game->starting_life);
    return true;
}
