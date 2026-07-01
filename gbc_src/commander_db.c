#include <stdint.h>

#include "commander_db.h"

#ifdef __SDCC
#pragma bank 2
#endif

typedef struct CommanderWordIndex {
    uint16_t commander_id;
    uint8_t name_start;
} CommanderWordIndex;

#include "commander_data.inc"

static char cached_substring_query[COMMANDER_QUERY_MAX + 1u];
static uint16_t substring_candidates[COMMANDER_RECORD_COUNT];
static uint16_t substring_candidate_count = 0u;
static uint8_t substring_cache_valid = 0u;

static char upper_ascii(char value) {
    if ((value >= 'a') && (value <= 'z')) {
        return (char)(value - ('a' - 'A'));
    }
    return value;
}

static uint8_t is_search_character(char value) {
    value = upper_ascii(value);
    return (
        ((value >= 'A') && (value <= 'Z'))
        || ((value >= '0') && (value <= '9'))
    );
}

static const char *commander_name(uint16_t commander_id) {
    return &commander_name_pool[commander_name_offsets[commander_id]];
}

static uint8_t normalize_query(const char *query, char *normalized) {
    uint8_t length = 0u;

    while ((*query != '\0') && (length < COMMANDER_QUERY_MAX)) {
        if (is_search_character(*query)) {
            normalized[length] = upper_ascii(*query);
            length++;
        }
        query++;
    }
    normalized[length] = '\0';
    return length;
}

static int8_t compare_segment_to_query(
    const char *name,
    const char *query,
    uint8_t name_start
) {
    uint8_t query_index = 0u;
    uint8_t name_index = name_start;

    while (query[query_index] != '\0') {
        while (
            (name[name_index] != '\0')
            && !is_search_character(name[name_index])
        ) {
            name_index++;
        }
        if (name[name_index] == '\0') {
            return -1;
        }
        if (upper_ascii(name[name_index]) < query[query_index]) {
            return -1;
        }
        if (upper_ascii(name[name_index]) > query[query_index]) {
            return 1;
        }
        name_index++;
        query_index++;
    }
    return 0;
}

static uint8_t substring_matches(const char *name, const char *query) {
    uint8_t name_start;

    for (name_start = 0u; name[name_start] != '\0'; name_start++) {
        if (
            is_search_character(name[name_start])
            && (compare_segment_to_query(name, query, name_start) == 0)
        ) {
            return 1u;
        }
    }
    return 0u;
}

static uint8_t result_contains(
    const uint16_t results[COMMANDER_SUGGESTION_COUNT],
    uint8_t count,
    uint16_t commander_id
) {
    uint8_t index;
    for (index = 0u; index < count; index++) {
        if (results[index] == commander_id) {
            return 1u;
        }
    }
    return 0u;
}

static uint16_t prefix_lower_bound(const char *query) {
    uint16_t low = 0u;
    uint16_t high = COMMANDER_RECORD_COUNT;

    while (low < high) {
        uint16_t middle = (uint16_t)(low + ((high - low) / 2u));
        uint16_t commander_id = commander_prefix_order[middle];
        if (compare_segment_to_query(commander_name(commander_id), query, 0u) < 0) {
            low = (uint16_t)(middle + 1u);
        } else {
            high = middle;
        }
    }
    return low;
}

static uint16_t word_lower_bound(const char *query) {
    uint16_t low = 0u;
    uint16_t high = COMMANDER_WORD_INDEX_COUNT;

    while (low < high) {
        uint16_t middle = (uint16_t)(low + ((high - low) / 2u));
        const CommanderWordIndex *entry = &commander_word_index[middle];
        if (
            compare_segment_to_query(
                commander_name(entry->commander_id),
                query,
                entry->name_start
            ) < 0
        ) {
            low = (uint16_t)(middle + 1u);
        } else {
            high = middle;
        }
    }
    return low;
}

static uint8_t query_extends_cached_query(const char *query) {
    uint8_t index = 0u;

    if (!substring_cache_valid) {
        return 0u;
    }
    while (cached_substring_query[index] != '\0') {
        if (cached_substring_query[index] != query[index]) {
            return 0u;
        }
        index++;
    }
    return query[index] != '\0';
}

static void cache_substring_matches(const char *query) {
    uint16_t read_index;
    uint16_t write_index = 0u;
    uint16_t source_count = COMMANDER_RECORD_COUNT;
    uint8_t filter_previous = query_extends_cached_query(query);

    if (filter_previous) {
        source_count = substring_candidate_count;
    }
    for (read_index = 0u; read_index < source_count; read_index++) {
        uint16_t commander_id = filter_previous
            ? substring_candidates[read_index]
            : read_index;
        if (substring_matches(commander_name(commander_id), query)) {
            substring_candidates[write_index] = commander_id;
            write_index++;
        }
    }
    substring_candidate_count = write_index;
    for (read_index = 0u; query[read_index] != '\0'; read_index++) {
        cached_substring_query[read_index] = query[read_index];
    }
    cached_substring_query[read_index] = '\0';
    substring_cache_valid = 1u;
}

uint16_t commander_db_count(void) BANKED {
    return COMMANDER_RECORD_COUNT;
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
        && (commander_name(commander_id)[index] != '\0')
    ) {
        output[index] = commander_name(commander_id)[index];
        index++;
    }
    output[index] = '\0';
}

uint8_t commander_db_get_archetype(uint16_t commander_id) BANKED {
    if (commander_id >= commander_db_count()) {
        return ARCHETYPE_CONTROL;
    }
    return commander_archetypes[commander_id];
}

uint8_t commander_db_find_matches(
    const char *query,
    uint16_t results[COMMANDER_SUGGESTION_COUNT]
) BANKED {
    char normalized_query[COMMANDER_QUERY_MAX + 1u];
    uint16_t index;
    uint8_t count = 0u;
    uint8_t normalized_length = normalize_query(query, normalized_query);

    index = prefix_lower_bound(normalized_query);
    while ((index < COMMANDER_RECORD_COUNT) && (count < COMMANDER_SUGGESTION_COUNT)) {
        uint16_t commander_id = commander_prefix_order[index];
        if (compare_segment_to_query(commander_name(commander_id), normalized_query, 0u) != 0) {
            break;
        }
        results[count] = commander_id;
        count++;
        index++;
    }

    if ((count < COMMANDER_SUGGESTION_COUNT) && (normalized_length > 0u)) {
        index = word_lower_bound(normalized_query);
        while ((index < COMMANDER_WORD_INDEX_COUNT) && (count < COMMANDER_SUGGESTION_COUNT)) {
            const CommanderWordIndex *entry = &commander_word_index[index];
            if (
                compare_segment_to_query(
                    commander_name(entry->commander_id),
                    normalized_query,
                    entry->name_start
                ) != 0
            ) {
                break;
            }
            if (!result_contains(results, count, entry->commander_id)) {
                results[count] = entry->commander_id;
                count++;
            }
            index++;
        }
    }

    if ((count < COMMANDER_SUGGESTION_COUNT) && (normalized_length >= 2u)) {
        cache_substring_matches(normalized_query);
        for (
            index = 0u;
            (index < substring_candidate_count)
            && (count < COMMANDER_SUGGESTION_COUNT);
            index++
        ) {
            if (!result_contains(results, count, substring_candidates[index])) {
                results[count] = substring_candidates[index];
                count++;
            }
        }
    }

    normalized_length = count;
    while (count < COMMANDER_SUGGESTION_COUNT) {
        results[count] = NO_COMMANDER_ID;
        count++;
    }
    return normalized_length;
}
