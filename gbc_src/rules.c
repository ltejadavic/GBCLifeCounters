#include "rules.h"

RuleStatus rules_check_life(const Player *player) {
    if (player->life <= 0) {
        return RULE_STATUS_POSSIBLE_LOSS;
    }
    if (player->life <= LIFE_WARNING_THRESHOLD) {
        return RULE_STATUS_WARNING;
    }
    return RULE_STATUS_NORMAL;
}

RuleStatus rules_check_poison(const GameState *game, const Player *player) {
    if (player->poison >= game->poison_threshold) {
        return RULE_STATUS_POSSIBLE_LOSS;
    }
    if (player->poison >= POISON_WARNING_THRESHOLD) {
        return RULE_STATUS_WARNING;
    }
    return RULE_STATUS_NORMAL;
}

RuleStatus rules_check_commander_damage(
    const GameState *game,
    uint8_t target_player,
    uint8_t source_player,
    uint8_t commander_slot
) {
    uint8_t damage;

    if (
        (target_player >= game->player_count)
        || (source_player >= game->player_count)
        || (commander_slot >= MAX_COMMANDERS_PER_PLAYER)
    ) {
        return RULE_STATUS_NORMAL;
    }

    damage = game->commander_damage[target_player][source_player][commander_slot];
    if (damage >= game->commander_damage_threshold) {
        return RULE_STATUS_POSSIBLE_LOSS;
    }
    if (damage >= COMMANDER_DAMAGE_WARNING_THRESHOLD) {
        return RULE_STATUS_WARNING;
    }
    return RULE_STATUS_NORMAL;
}

RuleStatus rules_check_commander_damage_for_player(
    const GameState *game,
    uint8_t target_player
) {
    uint8_t source_player;
    uint8_t commander_slot;
    RuleStatus highest_status = RULE_STATUS_NORMAL;

    if (target_player >= game->player_count) {
        return RULE_STATUS_NORMAL;
    }
    for (source_player = 0u; source_player < game->player_count; source_player++) {
        for (
            commander_slot = 0u;
            commander_slot < MAX_COMMANDERS_PER_PLAYER;
            commander_slot++
        ) {
            RuleStatus status = rules_check_commander_damage(
                game,
                target_player,
                source_player,
                commander_slot
            );
            if (status > highest_status) {
                highest_status = status;
            }
        }
    }
    return highest_status;
}

uint8_t rules_get_highest_commander_damage(
    const GameState *game,
    uint8_t target_player
) {
    uint8_t source_player;
    uint8_t commander_slot;
    uint8_t highest_damage = 0u;

    if (target_player >= game->player_count) {
        return 0u;
    }
    for (source_player = 0u; source_player < game->player_count; source_player++) {
        for (
            commander_slot = 0u;
            commander_slot < MAX_COMMANDERS_PER_PLAYER;
            commander_slot++
        ) {
            uint8_t damage = game->commander_damage
                [target_player][source_player][commander_slot];
            if (damage > highest_damage) {
                highest_damage = damage;
            }
        }
    }
    return highest_damage;
}

RuleStatus rules_check_player(const GameState *game, uint8_t player_id) {
    const Player *player;
    RuleStatus life_status;
    RuleStatus poison_status;
    RuleStatus commander_status;
    RuleStatus highest_status;

    if (player_id >= game->player_count) {
        return RULE_STATUS_NORMAL;
    }
    player = &game->players[player_id];
    life_status = rules_check_life(player);
    poison_status = rules_check_poison(game, player);
    commander_status = rules_check_commander_damage_for_player(game, player_id);
    highest_status = life_status > poison_status ? life_status : poison_status;

    return commander_status > highest_status ? commander_status : highest_status;
}

uint8_t rules_get_active_player_count(const GameState *game) {
    uint8_t player_id;
    uint8_t active_count = 0u;

    for (player_id = 0u; player_id < game->player_count; player_id++) {
        if (!game->players[player_id].eliminated) {
            active_count++;
        }
    }
    return active_count;
}

int8_t rules_check_winner(const GameState *game) {
    uint8_t player_id;
    int8_t winner = NO_PLAYER;

    if (rules_get_active_player_count(game) != 1u) {
        return NO_PLAYER;
    }
    for (player_id = 0u; player_id < game->player_count; player_id++) {
        if (!game->players[player_id].eliminated) {
            winner = (int8_t)player_id;
            break;
        }
    }
    return winner;
}
