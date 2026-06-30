#ifndef COMMANDER_GBC_COMMANDER_DB_H
#define COMMANDER_GBC_COMMANDER_DB_H

#include <gbdk/platform.h>
#include <stdint.h>

#include "constants.h"

#define COMMANDER_NAME_MAX 28u
#define COMMANDER_DETAIL_NAME_MAX 8u
#define COMMANDER_QUERY_MAX 8u
#define COMMANDER_SUGGESTION_COUNT 3u

typedef enum CommanderArchetype {
    ARCHETYPE_CONTROL = 0u,
    ARCHETYPE_ARTIFACTS = 1u,
    ARCHETYPE_COUNTERS = 2u,
    ARCHETYPE_ENCHANTMENTS = 3u,
    ARCHETYPE_GRAVEYARD = 4u,
    ARCHETYPE_LANDS = 5u,
    ARCHETYPE_LIFEGAIN = 6u,
    ARCHETYPE_SACRIFICE = 7u,
    ARCHETYPE_SPELLS = 8u,
    ARCHETYPE_TOKENS = 9u,
    ARCHETYPE_TYPAL = 10u,
    ARCHETYPE_VOLTRON = 11u,
    ARCHETYPE_COUNT = 12u
} CommanderArchetype;

uint16_t commander_db_count(void) BANKED;
void commander_db_copy_name(
    uint16_t commander_id,
    char *output,
    uint8_t maximum_characters
) BANKED;
uint8_t commander_db_get_archetype(uint16_t commander_id) BANKED;
uint8_t commander_db_find_matches(
    const char *query,
    uint16_t results[COMMANDER_SUGGESTION_COUNT]
) BANKED;

#endif
