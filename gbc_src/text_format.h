#ifndef COMMANDER_GBC_TEXT_FORMAT_H
#define COMMANDER_GBC_TEXT_FORMAT_H

#include <stdint.h>

#define LIFE_TEXT_WIDTH 6u
#define LIFE_TEXT_BUFFER_SIZE (LIFE_TEXT_WIDTH + 1u)

void format_life_total(
    int16_t value,
    char output[LIFE_TEXT_BUFFER_SIZE]
);

#endif
