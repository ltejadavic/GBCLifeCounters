#include "text_format.h"

void format_life_total(
    int16_t value,
    char output[LIFE_TEXT_BUFFER_SIZE]
) {
    uint8_t index;
    uint8_t position = LIFE_TEXT_WIDTH;
    uint16_t magnitude;
    uint8_t is_negative = (value < 0);

    for (index = 0u; index < LIFE_TEXT_WIDTH; index++) {
        output[index] = ' ';
    }
    output[LIFE_TEXT_WIDTH] = '\0';

    if (is_negative) {
        /* Adding one before negating keeps INT16_MIN representable. */
        magnitude = (uint16_t)(-(value + 1));
        magnitude++;
    } else {
        magnitude = (uint16_t)value;
    }

    do {
        position--;
        output[position] = (char)('0' + (magnitude % 10u));
        magnitude /= 10u;
    } while (magnitude > 0u);

    if (is_negative) {
        position--;
        output[position] = '-';
    }
}

void format_counter_value(
    uint8_t value,
    char output[COUNTER_TEXT_BUFFER_SIZE]
) {
    uint8_t index;
    uint8_t position = COUNTER_TEXT_WIDTH;

    for (index = 0u; index < COUNTER_TEXT_WIDTH; index++) {
        output[index] = ' ';
    }
    output[COUNTER_TEXT_WIDTH] = '\0';

    do {
        position--;
        output[position] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value > 0u);
}

void format_uint16_value(
    uint16_t value,
    char output[UINT16_TEXT_BUFFER_SIZE]
) {
    uint8_t index;
    uint8_t position = UINT16_TEXT_WIDTH;

    for (index = 0u; index < UINT16_TEXT_WIDTH; index++) {
        output[index] = ' ';
    }
    output[UINT16_TEXT_WIDTH] = '\0';

    do {
        position--;
        output[position] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value > 0u);
}
