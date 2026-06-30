#ifndef COMMANDER_GBC_TEXT_FORMAT_H
#define COMMANDER_GBC_TEXT_FORMAT_H

#include <stdint.h>

#define LIFE_TEXT_WIDTH 6u
#define LIFE_TEXT_BUFFER_SIZE (LIFE_TEXT_WIDTH + 1u)
#define COUNTER_TEXT_WIDTH 3u
#define COUNTER_TEXT_BUFFER_SIZE (COUNTER_TEXT_WIDTH + 1u)

void format_life_total(
    int16_t value,
    char output[LIFE_TEXT_BUFFER_SIZE]
);
void format_counter_value(
    uint8_t value,
    char output[COUNTER_TEXT_BUFFER_SIZE]
);

#endif
