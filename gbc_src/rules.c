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

RuleStatus rules_check_player(const GameState *game, const Player *player) {
    RuleStatus life_status = rules_check_life(player);
    RuleStatus poison_status = rules_check_poison(game, player);

    return life_status > poison_status ? life_status : poison_status;
}
