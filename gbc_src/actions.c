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

bool action_change_rad(GameState *game, uint8_t player_id, int16_t amount) {
    Player *player;

    if (!is_valid_player(game, player_id)) {
        return false;
    }

    player = &game->players[player_id];
    player->rad = change_uint8_value(player->rad, amount);
    return true;
}

bool action_set_rad(GameState *game, uint8_t player_id, uint8_t value) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    game->players[player_id].rad = value;
    return true;
}

bool action_set_commander(
    GameState *game,
    uint8_t player_id,
    uint16_t commander_id
) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    game->players[player_id].commander_id = commander_id;
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
    uint8_t previous_damage;
    int16_t applied_damage;

    if (
        !is_valid_player(game, target_player)
        || !is_valid_player(game, source_player)
        || (target_player == source_player)
        || (commander_slot >= MAX_COMMANDERS_PER_PLAYER)
    ) {
        return false;
    }

    damage = &game->commander_damage[target_player][source_player][commander_slot];
    previous_damage = *damage;
    *damage = change_uint8_value(*damage, amount);
    applied_damage = (int16_t)(*damage) - (int16_t)previous_damage;
    action_change_life(game, target_player, (int16_t)(-applied_damage));
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
        || (target_player == source_player)
        || (commander_slot >= MAX_COMMANDERS_PER_PLAYER)
    ) {
        return false;
    }
    game->commander_damage[target_player][source_player][commander_slot] = value;
    return true;
}

static int8_t find_next_active_player(
    const GameState *game,
    uint8_t eliminated_player
) {
    uint8_t offset;

    for (offset = 1u; offset <= game->player_count; offset++) {
        uint8_t candidate = (uint8_t)(
            (eliminated_player + offset) % game->player_count
        );
        if (!game->players[candidate].eliminated) {
            return (int8_t)candidate;
        }
    }
    return NO_PLAYER;
}

bool action_eliminate_player(GameState *game, uint8_t player_id) {
    Player *player;

    if (!is_valid_player(game, player_id)) {
        return false;
    }
    player = &game->players[player_id];
    player->eliminated = 1u;
    player->is_monarch = 0u;
    player->has_initiative = 0u;

    if (game->monarch_player == (int8_t)player_id) {
        game->monarch_player = NO_PLAYER;
    }
    if (game->initiative_player == (int8_t)player_id) {
        game->initiative_player = NO_PLAYER;
    }
    if (game->active_player == (int8_t)player_id) {
        game->active_player = find_next_active_player(game, player_id);
    }
    return true;
}

bool action_restore_player(GameState *game, uint8_t player_id) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    game->players[player_id].eliminated = 0u;
    if (game->active_player == NO_PLAYER) {
        game->active_player = (int8_t)player_id;
    }
    return true;
}

bool action_set_monarch(GameState *game, int8_t player_id) {
    uint8_t index;

    if (
        (player_id != NO_PLAYER)
        && (
            (player_id < 0)
            || ((uint8_t)player_id >= game->player_count)
            || game->players[(uint8_t)player_id].eliminated
        )
    ) {
        return false;
    }
    for (index = 0u; index < game->player_count; index++) {
        game->players[index].is_monarch = 0u;
    }
    game->monarch_player = player_id;
    if (player_id != NO_PLAYER) {
        game->players[(uint8_t)player_id].is_monarch = 1u;
    }
    return true;
}

bool action_set_initiative(GameState *game, int8_t player_id) {
    uint8_t index;

    if (
        (player_id != NO_PLAYER)
        && (
            (player_id < 0)
            || ((uint8_t)player_id >= game->player_count)
            || game->players[(uint8_t)player_id].eliminated
        )
    ) {
        return false;
    }
    for (index = 0u; index < game->player_count; index++) {
        game->players[index].has_initiative = 0u;
    }
    game->initiative_player = player_id;
    if (player_id != NO_PLAYER) {
        game->players[(uint8_t)player_id].has_initiative = 1u;
    }
    return true;
}

bool action_reset_player(GameState *game, uint8_t player_id) {
    if (!is_valid_player(game, player_id)) {
        return false;
    }
    player_reset(&game->players[player_id], game->starting_life);
    return true;
}
