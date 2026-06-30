#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "actions.h"
#include "constants.h"
#include "game_state.h"
#include "navigation.h"
#include "rules.h"
#include "text_format.h"

static void test_four_player_initial_state(void) {
    GameState game;
    uint8_t player_id;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);

    assert(game.player_count == 4u);
    for (player_id = 0u; player_id < game.player_count; player_id++) {
        assert(game.players[player_id].player_id == player_id);
        assert(game.players[player_id].life == DEFAULT_STARTING_LIFE);
        assert(game.players[player_id].name[0] == 'P');
        assert(game.players[player_id].name[1] == (char)('1' + player_id));
    }
    assert(game.turn_number == 1u);
    assert(game.active_player == 0);
}

static void test_life_actions_are_independent(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(action_change_life(&game, 1u, -35));
    assert(game.players[0].life == 40);
    assert(game.players[1].life == 5);
    assert(game.players[2].life == 40);
    assert(game.players[3].life == 40);
    assert(rules_check_life(&game.players[1]) == LIFE_STATUS_WARNING);

    assert(action_change_life(&game, 2u, -40));
    assert(game.players[2].life == 0);
    assert(rules_check_life(&game.players[2]) == LIFE_STATUS_POSSIBLE_LOSS);
    assert(game.players[2].eliminated == 0u);

    assert(action_change_life(&game, 2u, -10));
    assert(game.players[2].life == -10);
    assert(game.players[2].eliminated == 0u);

    assert(action_reset_player(&game, 2u));
    assert(game.players[2].life == DEFAULT_STARTING_LIFE);
    assert(rules_check_life(&game.players[2]) == LIFE_STATUS_NORMAL);
}

static void test_validation_and_life_bounds(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(!action_change_life(&game, 4u, 1));
    assert(action_set_life(&game, 0u, MAX_LIFE_TOTAL));
    assert(action_change_life(&game, 0u, 10));
    assert(game.players[0].life == MAX_LIFE_TOTAL);
    assert(action_set_life(&game, 0u, MIN_LIFE_TOTAL));
    assert(action_change_life(&game, 0u, -10));
    assert(game.players[0].life == MIN_LIFE_TOTAL);
}

static void test_navigation_wraps_and_cycles_steps(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(navigation_previous_player(&game, 0u) == 3u);
    assert(navigation_next_player(&game, 3u) == 0u);
    assert(navigation_next_player(&game, 1u) == 2u);
    assert(navigation_next_life_step(LIFE_STEP_SMALL) == LIFE_STEP_MEDIUM);
    assert(navigation_next_life_step(LIFE_STEP_MEDIUM) == LIFE_STEP_LARGE);
    assert(navigation_next_life_step(LIFE_STEP_LARGE) == LIFE_STEP_SMALL);
}

static void test_eight_player_capacity_and_game_reset(void) {
    GameState game;

    game_state_init(&game, MAX_PLAYERS + 1u, 30);
    assert(game.player_count == MAX_PLAYERS);
    assert(game.players[7].name[0] == 'P');
    assert(game.players[7].name[1] == '8');

    game.players[3].life = 2;
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
    test_four_player_initial_state();
    test_life_actions_are_independent();
    test_validation_and_life_bounds();
    test_navigation_wraps_and_cycles_steps();
    test_eight_player_capacity_and_game_reset();
    test_life_text_replaces_the_entire_field();
    printf("GBC core tests passed.\n");
    return 0;
}
