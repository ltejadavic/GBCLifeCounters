#ifndef COMMANDER_GBC_RULES_H
#define COMMANDER_GBC_RULES_H

#include "game_state.h"

typedef enum RuleStatus {
    RULE_STATUS_NORMAL = 0,
    RULE_STATUS_WARNING = 1,
    RULE_STATUS_POSSIBLE_LOSS = 2
} RuleStatus;

RuleStatus rules_check_life(const Player *player);
RuleStatus rules_check_poison(const GameState *game, const Player *player);
RuleStatus rules_check_commander_damage(
    const GameState *game,
    uint8_t target_player,
    uint8_t source_player,
    uint8_t commander_slot
);
RuleStatus rules_check_commander_damage_for_player(
    const GameState *game,
    uint8_t target_player
);
uint8_t rules_get_highest_commander_damage(
    const GameState *game,
    uint8_t target_player
);
RuleStatus rules_check_player(const GameState *game, uint8_t player_id);
uint8_t rules_get_active_player_count(const GameState *game);
int8_t rules_check_winner(const GameState *game);

#endif
