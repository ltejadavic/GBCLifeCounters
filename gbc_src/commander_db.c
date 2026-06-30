#include <stdint.h>

#include "commander_db.h"

#pragma bank 255

typedef struct CommanderRecord {
    char name[COMMANDER_NAME_MAX + 1u];
    uint8_t archetype;
} CommanderRecord;

static const CommanderRecord commander_records[] = {
#include "commander_data.inc"
};

static char upper_ascii(char value) {
    if ((value >= 'a') && (value <= 'z')) {
        return (char)(value - ('a' - 'A'));
    }
    return value;
}

static uint8_t contains_query(const char *name, const char *query) {
    uint8_t name_start;

    if (query[0] == '\0') {
        return 1u;
    }
    for (name_start = 0u; name[name_start] != '\0'; name_start++) {
        uint8_t query_index = 0u;
        while (
            (query[query_index] != '\0')
            && (name[name_start + query_index] != '\0')
            && (
                upper_ascii(name[name_start + query_index])
                == upper_ascii(query[query_index])
            )
        ) {
            query_index++;
        }
        if (query[query_index] == '\0') {
            return 1u;
        }
    }
    return 0u;
}

uint16_t commander_db_count(void) BANKED {
    return (uint16_t)(sizeof(commander_records) / sizeof(CommanderRecord));
}

void commander_db_copy_name(
    uint16_t commander_id,
    char *output,
    uint8_t maximum_characters
) BANKED {
    uint8_t index = 0u;

    if (commander_id >= commander_db_count()) {
        output[0] = '-';
        output[1] = '-';
        output[2] = '\0';
        return;
    }
    while (
        (index < maximum_characters)
        && (commander_records[commander_id].name[index] != '\0')
    ) {
        output[index] = commander_records[commander_id].name[index];
        index++;
    }
    output[index] = '\0';
}

uint8_t commander_db_get_archetype(uint16_t commander_id) BANKED {
    if (commander_id >= commander_db_count()) {
        return ARCHETYPE_CONTROL;
    }
    return commander_records[commander_id].archetype;
}

uint8_t commander_db_find_matches(
    const char *query,
    uint16_t results[COMMANDER_SUGGESTION_COUNT]
) BANKED {
    uint16_t commander_id;
    uint8_t count = 0u;

    for (
        commander_id = 0u;
        (commander_id < commander_db_count())
        && (count < COMMANDER_SUGGESTION_COUNT);
        commander_id++
    ) {
        if (contains_query(commander_records[commander_id].name, query)) {
            results[count] = commander_id;
            count++;
        }
    }
    while (count < COMMANDER_SUGGESTION_COUNT) {
        results[count] = NO_COMMANDER_ID;
        count++;
    }
    return count;
}
