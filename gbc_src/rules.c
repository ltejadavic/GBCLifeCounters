#include "rules.h"

LifeStatus rules_check_life(const Player *player) {
    if (player->life <= 0) {
        return LIFE_STATUS_POSSIBLE_LOSS;
    }
    if (player->life <= LIFE_WARNING_THRESHOLD) {
        return LIFE_STATUS_WARNING;
    }
    return LIFE_STATUS_NORMAL;
}
