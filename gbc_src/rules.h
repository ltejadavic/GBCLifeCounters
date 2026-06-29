#ifndef COMMANDER_GBC_RULES_H
#define COMMANDER_GBC_RULES_H

#include "player.h"

typedef enum LifeStatus {
    LIFE_STATUS_NORMAL = 0,
    LIFE_STATUS_WARNING = 1,
    LIFE_STATUS_POSSIBLE_LOSS = 2
} LifeStatus;

LifeStatus rules_check_life(const Player *player);

#endif
