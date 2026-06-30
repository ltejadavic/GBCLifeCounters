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
RuleStatus rules_check_player(const GameState *game, const Player *player);

#endif
