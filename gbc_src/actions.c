#include "actions.h"

static bool is_valid_player(const GameState *game, uint8_t player_id) {
    return player_id < game->player_count;
}

static uint8_t change_uint8_value(uint8_t value, int16_t amount) {
    int16_t current = (int16_t)value;

    if (amount > (255 - current)) {
        return 255u;
    }
    if (amount < -current) {
        return 0u;
    }
    return (uint8_t)(current + amount);
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

    if (!is_valid_player(game, player_id)) {
        return false;
    }

    player = &game->players[player_id];
    player->poison = change_uint8_value(player->poison, amount);
    return true;
}

bool action_set_poison(GameState *game, uint8_t player_id, uint8_t value) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    game->players[player_id].poison = value;
    return true;
}

bool action_change_commander_damage(
    GameState *game,
    uint8_t target_player,
    uint8_t source_player,
    uint8_t commander_slot,
    int16_t amount
) {
    uint8_t *damage;

    if (
        !is_valid_player(game, target_player)
        || !is_valid_player(game, source_player)
        || (commander_slot >= MAX_COMMANDERS_PER_PLAYER)
    ) {
        return false;
    }

    damage = &game->commander_damage[target_player][source_player][commander_slot];
    *damage = change_uint8_value(*damage, amount);
    return true;
}

bool action_set_commander_damage(
    GameState *game,
    uint8_t target_player,
    uint8_t source_player,
    uint8_t commander_slot,
    uint8_t value
) {
    if (
        !is_valid_player(game, target_player)
        || !is_valid_player(game, source_player)
        || (commander_slot >= MAX_COMMANDERS_PER_PLAYER)
    ) {
        return false;
    }
    game->commander_damage[target_player][source_player][commander_slot] = value;
    return true;
}

bool action_reset_player(GameState *game, uint8_t player_id) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    player_reset(&game->players[player_id], game->starting_life);
    return true;
}
