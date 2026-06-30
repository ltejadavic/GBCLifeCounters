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

static void test_player_count_is_clamped_to_supported_range(void) {
    GameState game;

    game_state_init(&game, 0u, DEFAULT_STARTING_LIFE);
    assert(game.player_count == MIN_PLAYERS);
    game_state_init(&game, MAX_PLAYERS + 1u, DEFAULT_STARTING_LIFE);
    assert(game.player_count == MAX_PLAYERS);
}

static void test_life_actions_are_independent(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(action_change_life(&game, 1u, -35));
    assert(game.players[0].life == 40);
    assert(game.players[1].life == 5);
    assert(game.players[2].life == 40);
    assert(game.players[3].life == 40);
    assert(rules_check_life(&game.players[1]) == RULE_STATUS_WARNING);

    assert(action_change_life(&game, 2u, -40));
    assert(game.players[2].life == 0);
    assert(rules_check_life(&game.players[2]) == RULE_STATUS_POSSIBLE_LOSS);
    assert(game.players[2].eliminated == 0u);

    assert(action_change_life(&game, 2u, -10));
    assert(game.players[2].life == -10);
    assert(game.players[2].eliminated == 0u);

    assert(action_reset_player(&game, 2u));
    assert(game.players[2].life == DEFAULT_STARTING_LIFE);
    assert(rules_check_life(&game.players[2]) == RULE_STATUS_NORMAL);
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
    assert(navigation_previous_other_player(&game, 1u, 0u) == 3u);
    assert(navigation_next_other_player(&game, 3u, 0u) == 1u);
    assert(navigation_next_other_player(&game, 1u, 2u) == 3u);
    assert(navigation_next_life_step(LIFE_STEP_SMALL) == LIFE_STEP_MEDIUM);
    assert(navigation_next_life_step(LIFE_STEP_MEDIUM) == LIFE_STEP_LARGE);
    assert(navigation_next_life_step(LIFE_STEP_LARGE) == LIFE_STEP_SMALL);
    assert(navigation_next_detail_field(DETAIL_FIELD_LIFE) == DETAIL_FIELD_POISON);
    assert(
        navigation_next_detail_field(DETAIL_FIELD_POISON)
        == DETAIL_FIELD_COMMANDER_DAMAGE
    );
    assert(
        navigation_next_detail_field(DETAIL_FIELD_COMMANDER_DAMAGE)
        == DETAIL_FIELD_LIFE
    );
    assert(
        navigation_previous_detail_field(DETAIL_FIELD_LIFE)
        == DETAIL_FIELD_COMMANDER_DAMAGE
    );

    game_state_init(&game, 8u, DEFAULT_STARTING_LIFE);
    assert(navigation_update_window_start(&game, 0u, 0u, 4u) == 0u);
    assert(navigation_update_window_start(&game, 4u, 0u, 4u) == 1u);
    assert(navigation_update_window_start(&game, 7u, 1u, 4u) == 4u);
    assert(navigation_update_window_start(&game, 0u, 4u, 4u) == 0u);
}

static void test_poison_actions_and_rules_are_independent(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(action_change_poison(&game, 1u, 8));
    assert(game.players[0].poison == 0u);
    assert(game.players[1].poison == 8u);
    assert(rules_check_poison(&game, &game.players[1]) == RULE_STATUS_WARNING);
    assert(rules_check_player(&game, 1u) == RULE_STATUS_WARNING);

    assert(action_change_poison(&game, 1u, 2));
    assert(game.players[1].poison == 10u);
    assert(
        rules_check_poison(&game, &game.players[1])
        == RULE_STATUS_POSSIBLE_LOSS
    );
    assert(game.players[1].eliminated == 0u);

    assert(action_change_poison(&game, 1u, -20));
    assert(game.players[1].poison == 0u);
    assert(action_set_poison(&game, 1u, 250u));
    assert(action_change_poison(&game, 1u, 20));
    assert(game.players[1].poison == 255u);
    assert(!action_change_poison(&game, 4u, 1));
}

static void test_commander_damage_is_separate_by_source_and_slot(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(action_set_commander_damage(&game, 1u, 0u, 0u, 12u));
    assert(action_set_commander_damage(&game, 1u, 0u, 1u, 9u));
    assert(rules_get_highest_commander_damage(&game, 1u) == 12u);
    assert(
        rules_check_commander_damage_for_player(&game, 1u)
        == RULE_STATUS_NORMAL
    );

    assert(action_set_commander_damage(&game, 1u, 2u, 0u, 9u));
    assert(
        rules_check_commander_damage_for_player(&game, 1u)
        == RULE_STATUS_NORMAL
    );

    assert(action_set_commander_damage(&game, 1u, 2u, 0u, 18u));
    assert(
        rules_check_commander_damage(&game, 1u, 2u, 0u)
        == RULE_STATUS_WARNING
    );
    assert(rules_check_player(&game, 1u) == RULE_STATUS_WARNING);

    assert(action_change_commander_damage(&game, 1u, 2u, 0u, 3));
    assert(game.commander_damage[1][2][0] == 21u);
    assert(
        rules_check_commander_damage(&game, 1u, 2u, 0u)
        == RULE_STATUS_POSSIBLE_LOSS
    );
    assert(game.players[1].eliminated == 0u);

    assert(action_change_commander_damage(&game, 1u, 2u, 0u, -30));
    assert(game.commander_damage[1][2][0] == 0u);
    assert(!action_change_commander_damage(&game, 4u, 2u, 0u, 1));
    assert(!action_change_commander_damage(&game, 1u, 4u, 0u, 1));
    assert(!action_change_commander_damage(&game, 1u, 2u, 2u, 1));
}

static void test_commander_damage_changes_life_by_applied_delta(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(action_change_life(&game, 0u, -3));
    assert(game.players[0].life == 37);

    assert(action_change_commander_damage(&game, 0u, 1u, 0u, 5));
    assert(game.commander_damage[0][1][0] == 5u);
    assert(game.players[0].life == 32);

    assert(action_change_commander_damage(&game, 0u, 1u, 0u, -2));
    assert(game.commander_damage[0][1][0] == 3u);
    assert(game.players[0].life == 34);

    assert(action_change_commander_damage(&game, 0u, 1u, 0u, -10));
    assert(game.commander_damage[0][1][0] == 0u);
    assert(game.players[0].life == 37);
}

static void test_self_commander_damage_is_rejected(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(!action_change_commander_damage(&game, 0u, 0u, 0u, 5));
    assert(!action_set_commander_damage(&game, 0u, 0u, 0u, 5u));
    assert(game.commander_damage[0][0][0] == 0u);
    assert(game.players[0].life == DEFAULT_STARTING_LIFE);
    assert(
        rules_check_commander_damage(&game, 0u, 0u, 0u)
        == RULE_STATUS_NORMAL
    );
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

static void test_manual_elimination_restore_and_winner(void) {
    GameState game;

    game_state_init(&game, 4u, DEFAULT_STARTING_LIFE);
    assert(action_set_life(&game, 0u, 0));
    assert(rules_check_player(&game, 0u) == RULE_STATUS_POSSIBLE_LOSS);
    assert(game.players[0].eliminated == 0u);
    assert(rules_check_winner(&game) == NO_PLAYER);

    assert(action_eliminate_player(&game, 0u));
    assert(game.active_player == 1);
    assert(action_eliminate_player(&game, 1u));
    assert(action_eliminate_player(&game, 2u));
    assert(rules_get_active_player_count(&game) == 1u);
    assert(rules_check_winner(&game) == 3);

    assert(action_restore_player(&game, 1u));
    assert(rules_get_active_player_count(&game) == 2u);
    assert(rules_check_winner(&game) == NO_PLAYER);

    assert(action_eliminate_player(&game, 1u));
    assert(action_eliminate_player(&game, 3u));
    assert(rules_get_active_player_count(&game) == 0u);
    assert(rules_check_winner(&game) == NO_PLAYER);
    assert(game.active_player == NO_PLAYER);

    assert(action_restore_player(&game, 2u));
    assert(game.active_player == 2);
    assert(rules_check_winner(&game) == 2);
    assert(!action_eliminate_player(&game, 4u));
    assert(!action_restore_player(&game, 4u));

    game_state_reset(&game);
    assert(rules_get_active_player_count(&game) == 4u);
    assert(rules_check_winner(&game) == NO_PLAYER);
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

static void test_counter_text_replaces_the_entire_field(void) {
    char output[COUNTER_TEXT_BUFFER_SIZE];

    format_counter_value(0u, output);
    assert(strcmp(output, "  0") == 0);
    format_counter_value(8u, output);
    assert(strcmp(output, "  8") == 0);
    format_counter_value(10u, output);
    assert(strcmp(output, " 10") == 0);
    format_counter_value(255u, output);
    assert(strcmp(output, "255") == 0);
}

int main(void) {
    test_four_player_initial_state();
    test_player_count_is_clamped_to_supported_range();
    test_life_actions_are_independent();
    test_validation_and_life_bounds();
    test_navigation_wraps_and_cycles_steps();
    test_poison_actions_and_rules_are_independent();
    test_commander_damage_is_separate_by_source_and_slot();
    test_commander_damage_changes_life_by_applied_delta();
    test_self_commander_damage_is_rejected();
    test_eight_player_capacity_and_game_reset();
    test_manual_elimination_restore_and_winner();
    test_life_text_replaces_the_entire_field();
    test_counter_text_replaces_the_entire_field();
    printf("GBC core tests passed.\n");
    return 0;
}
