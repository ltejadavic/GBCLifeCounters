#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "actions.h"
#include "constants.h"
#include "game_state.h"
#include "rules.h"
#include "text_format.h"

static void test_initial_state(void) {
    GameState game;

    game_state_init(&game, 1u, DEFAULT_STARTING_LIFE);
    assert(game.player_count == 1u);
    assert(game.players[0].life == 40);
    assert(game.players[0].name[0] == 'P');
    assert(game.players[0].name[1] == '1');
    assert(game.turn_number == 1u);
    assert(game.active_player == 0);
}

static void test_life_actions_and_rules(void) {
    GameState game;

    game_state_init(&game, 1u, DEFAULT_STARTING_LIFE);
    assert(action_change_life(&game, 0u, -35));
    assert(game.players[0].life == 5);
    assert(rules_check_life(&game.players[0]) == LIFE_STATUS_WARNING);

    assert(action_change_life(&game, 0u, -5));
    assert(game.players[0].life == 0);
    assert(rules_check_life(&game.players[0]) == LIFE_STATUS_POSSIBLE_LOSS);
    assert(game.players[0].eliminated == 0u);

    assert(action_change_life(&game, 0u, -10));
    assert(game.players[0].life == -10);
    assert(game.players[0].eliminated == 0u);

    assert(action_reset_player(&game, 0u));
    assert(game.players[0].life == DEFAULT_STARTING_LIFE);
    assert(rules_check_life(&game.players[0]) == LIFE_STATUS_NORMAL);
}

static void test_validation_and_life_bounds(void) {
    GameState game;

    game_state_init(&game, 1u, DEFAULT_STARTING_LIFE);
    assert(!action_change_life(&game, 1u, 1));
    assert(action_set_life(&game, 0u, MAX_LIFE_TOTAL));
    assert(action_change_life(&game, 0u, 10));
    assert(game.players[0].life == MAX_LIFE_TOTAL);
    assert(action_set_life(&game, 0u, MIN_LIFE_TOTAL));
    assert(action_change_life(&game, 0u, -10));
    assert(game.players[0].life == MIN_LIFE_TOTAL);
}

static void test_eight_player_capacity_and_reset(void) {
    GameState game;

    game_state_init(&game, MAX_PLAYERS + 1u, 30);
    assert(game.player_count == MAX_PLAYERS);
    assert(game.players[7].name[0] == 'P');
    assert(game.players[7].name[1] == '8');

    game.players[3].poison = 9u;
    game.players[3].energy = 4u;
    game.players[3].commander_tax[1] = 6u;
    game.players[3].eliminated = 1u;
    game.commander_damage[3][1][0] = 18u;
    game.storm_count = 7u;
    game.turn_number = 12u;

    game_state_reset(&game);

    assert(game.players[3].life == 30);
    assert(game.players[3].poison == 0u);
    assert(game.players[3].energy == 0u);
    assert(game.players[3].commander_tax[1] == 0u);
    assert(game.players[3].eliminated == 0u);
    assert(game.commander_damage[3][1][0] == 0u);
    assert(game.storm_count == 0u);
    assert(game.turn_number == 1u);
}

static void assert_life_text(int16_t value, const char *expected) {
    char output[LIFE_TEXT_BUFFER_SIZE];

    format_life_total(value, output);
    assert(strcmp(output, expected) == 0);
}

static void test_life_text_replaces_the_entire_field(void) {
    assert_life_text(40, "    40");
    assert_life_text(10, "    10");
    assert_life_text(5, "     5");
    assert_life_text(0, "     0");
    assert_life_text(-5, "    -5");
    assert_life_text(900, "   900");
    assert_life_text(MAX_LIFE_TOTAL, " 32767");
    assert_life_text(MIN_LIFE_TOTAL, "-32768");
}

int main(void) {
    test_initial_state();
    test_life_actions_and_rules();
    test_validation_and_life_bounds();
    test_eight_player_capacity_and_reset();
    test_life_text_replaces_the_entire_field();
    printf("GBC core tests passed.\n");
    return 0;
}
