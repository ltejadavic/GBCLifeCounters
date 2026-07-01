#include <gb/gb.h>
#include <stdint.h>

#include "actions.h"
#include "commander_db.h"
#include "constants.h"
#include "game_state.h"
#include "navigation.h"
#include "rules.h"
#include "sound.h"
#include "ui.h"

#define VISIBLE_PLAYER_COUNT 4u
#define SCREEN_SETUP 0u
#define SCREEN_OVERVIEW 1u
#define SCREEN_RESET_CONFIRMATION 2u
#define SCREEN_PLAYER_DETAIL 3u
#define SCREEN_COMMANDER_DAMAGE 4u
#define SCREEN_ELIMINATION_CONFIRMATION 5u
#define SCREEN_SPLASH 6u
#define SCREEN_DEVELOPER_CREDIT 7u
#define SCREEN_COMMANDER_SEARCH 8u
#define PROMPT_BLINK_FRAMES 30u

static GameState game;
static uint8_t setup_player_count = 4u;
static int16_t setup_starting_life = DEFAULT_STARTING_LIFE;
static uint8_t setup_field = SETUP_FIELD_PLAYER_COUNT;
static uint8_t setup_can_cancel = 0u;
static uint8_t selected_player = 0u;
static uint8_t first_visible_player = 0u;
static uint8_t selected_field = DETAIL_FIELD_LIFE;
static uint8_t selected_source = 0u;
static uint8_t first_visible_source = 0u;
static uint8_t adjustment_step = LIFE_STEP_SMALL;
static uint8_t screen_state = SCREEN_SPLASH;
static uint8_t prompt_blink_frames = 0u;
static uint8_t prompt_is_visible = 1u;
static char commander_query[COMMANDER_QUERY_MAX + 1u];
static uint8_t commander_query_length = 0u;
static uint8_t commander_keyboard_index = 0u;
static uint8_t commander_suggestion = 0u;
static uint8_t commander_list_focus = 0u;
static uint8_t commander_match_count = 0u;
static uint16_t commander_suggestions[COMMANDER_SUGGESTION_COUNT];

static void update_player_window(void) {
    first_visible_player = navigation_update_window_start(
        &game,
        selected_player,
        first_visible_player,
        VISIBLE_PLAYER_COUNT
    );
}

static void update_source_window(void) {
    first_visible_source = navigation_update_window_start(
        &game,
        selected_source,
        first_visible_source,
        VISIBLE_PLAYER_COUNT
    );
}

static void refresh_overview(void) {
    ui_refresh_overview(
        &game,
        selected_player,
        first_visible_player,
        adjustment_step
    );
}

static void refresh_commander_search(void) {
    commander_match_count = commander_db_find_matches(
        commander_query,
        commander_suggestions
    );
    if (
        (commander_match_count == 0u)
        || (commander_suggestion >= commander_match_count)
    ) {
        commander_suggestion = 0u;
    }
    ui_show_commander_search(
        &game.players[selected_player],
        commander_query,
        commander_suggestions,
        commander_suggestion,
        commander_keyboard_index,
        commander_list_focus
    );
}

static void open_commander_search(void) {
    commander_query_length = 0u;
    commander_query[0] = '\0';
    commander_keyboard_index = 0u;
    commander_suggestion = 0u;
    commander_list_focus = 0u;
    screen_state = SCREEN_COMMANDER_SEARCH;
    refresh_commander_search();
}

static void handle_commander_search_input(uint8_t pressed) {
    static const char keyboard[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 '-,";
    uint8_t previous_keyboard_index = commander_keyboard_index;

    if ((pressed & J_SELECT) || (pressed & J_START)) {
        commander_list_focus = (uint8_t)(!commander_list_focus);
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        ui_update_commander_search_cursor(
            previous_keyboard_index,
            commander_keyboard_index,
            commander_suggestion,
            commander_list_focus
        );
        return;
    } else if (commander_list_focus && (pressed & J_UP)) {
        if (commander_match_count > 0u) {
            commander_suggestion = commander_suggestion == 0u
                ? (uint8_t)(commander_match_count - 1u)
                : (uint8_t)(commander_suggestion - 1u);
        }
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        ui_update_commander_search_cursor(
            previous_keyboard_index,
            commander_keyboard_index,
            commander_suggestion,
            commander_list_focus
        );
        return;
    } else if (commander_list_focus && (pressed & J_DOWN)) {
        if (commander_match_count > 0u) {
            commander_suggestion = (uint8_t)(
                (commander_suggestion + 1u) % commander_match_count
            );
        }
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        ui_update_commander_search_cursor(
            previous_keyboard_index,
            commander_keyboard_index,
            commander_suggestion,
            commander_list_focus
        );
        return;
    } else if (!commander_list_focus && (pressed & J_LEFT)) {
        commander_keyboard_index = commander_keyboard_index == 0u
            ? (COMMANDER_KEY_COUNT - 1u)
            : (uint8_t)(commander_keyboard_index - 1u);
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        ui_update_commander_search_cursor(
            previous_keyboard_index,
            commander_keyboard_index,
            commander_suggestion,
            commander_list_focus
        );
        return;
    } else if (!commander_list_focus && (pressed & J_RIGHT)) {
        commander_keyboard_index = (uint8_t)(
            (commander_keyboard_index + 1u) % COMMANDER_KEY_COUNT
        );
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        ui_update_commander_search_cursor(
            previous_keyboard_index,
            commander_keyboard_index,
            commander_suggestion,
            commander_list_focus
        );
        return;
    } else if (!commander_list_focus && (pressed & J_UP)) {
        commander_keyboard_index = (uint8_t)(
            (commander_keyboard_index + 32u) % COMMANDER_KEY_COUNT
        );
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        ui_update_commander_search_cursor(
            previous_keyboard_index,
            commander_keyboard_index,
            commander_suggestion,
            commander_list_focus
        );
        return;
    } else if (!commander_list_focus && (pressed & J_DOWN)) {
        commander_keyboard_index = (uint8_t)(
            (commander_keyboard_index + 8u) % COMMANDER_KEY_COUNT
        );
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        ui_update_commander_search_cursor(
            previous_keyboard_index,
            commander_keyboard_index,
            commander_suggestion,
            commander_list_focus
        );
        return;
    } else if ((pressed & J_A) && !commander_list_focus) {
        if (commander_query_length < COMMANDER_QUERY_MAX) {
            commander_query[commander_query_length] = keyboard[
                commander_keyboard_index
            ];
            commander_query_length++;
            commander_query[commander_query_length] = '\0';
            commander_suggestion = 0u;
            sound_play_effect(SOUND_EFFECT_CONFIRM);
        } else {
            sound_play_effect(SOUND_EFFECT_CANCEL);
            return;
        }
    } else if (
        (pressed & J_A)
        && commander_list_focus
        && (commander_match_count > 0u)
    ) {
        action_set_commander(
            &game,
            selected_player,
            commander_suggestions[commander_suggestion]
        );
        sound_play_effect(SOUND_EFFECT_CONFIRM);
        screen_state = SCREEN_PLAYER_DETAIL;
        ui_show_player_detail(
            &game,
            selected_player,
            selected_field,
            adjustment_step
        );
        return;
    } else if (pressed & J_B) {
        if (commander_list_focus) {
            commander_list_focus = 0u;
            sound_play_effect(SOUND_EFFECT_CANCEL);
            ui_update_commander_search_cursor(
                previous_keyboard_index,
                commander_keyboard_index,
                commander_suggestion,
                commander_list_focus
            );
            return;
        } else if (commander_query_length > 0u) {
            commander_query_length--;
            commander_query[commander_query_length] = '\0';
            commander_suggestion = 0u;
        } else {
            screen_state = SCREEN_PLAYER_DETAIL;
            ui_show_player_detail(
                &game,
                selected_player,
                selected_field,
                adjustment_step
            );
            return;
        }
        sound_play_effect(SOUND_EFFECT_CANCEL);
    } else {
        return;
    }
    refresh_commander_search();
}

static void play_value_feedback(
    RuleStatus before_status,
    RuleStatus after_status,
    int16_t before_value,
    int16_t after_value
) {
    if (before_value == after_value) {
        sound_play_effect(SOUND_EFFECT_CANCEL);
    } else if (
        (before_status != RULE_STATUS_POSSIBLE_LOSS)
        && (after_status == RULE_STATUS_POSSIBLE_LOSS)
    ) {
        sound_play_effect(SOUND_EFFECT_LOSS);
    } else if (after_value > before_value) {
        sound_play_effect(SOUND_EFFECT_VALUE_UP);
    } else {
        sound_play_effect(SOUND_EFFECT_VALUE_DOWN);
    }
}

static void handle_splash_input(uint8_t pressed) {
    if (pressed & J_START) {
        sound_play_effect(SOUND_EFFECT_START);
        prompt_blink_frames = 0u;
        prompt_is_visible = 1u;
        screen_state = SCREEN_DEVELOPER_CREDIT;
        ui_show_developer_credit();
    }
}

static void handle_developer_credit_input(uint8_t pressed) {
    if (pressed & (J_A | J_START)) {
        sound_play_effect(
            (pressed & J_START) ? SOUND_EFFECT_START : SOUND_EFFECT_CONFIRM
        );
        sound_stop_intro_music();
        screen_state = SCREEN_SETUP;
        ui_show_setup(
            setup_player_count,
            setup_starting_life,
            setup_field,
            setup_can_cancel
        );
    }
}

static void update_blinking_prompt(void) {
    if (
        (screen_state != SCREEN_SPLASH)
        && (screen_state != SCREEN_DEVELOPER_CREDIT)
    ) {
        return;
    }

    prompt_blink_frames++;
    if (prompt_blink_frames < PROMPT_BLINK_FRAMES) {
        return;
    }
    prompt_blink_frames = 0u;
    prompt_is_visible = (uint8_t)(!prompt_is_visible);
    if (screen_state == SCREEN_SPLASH) {
        ui_set_splash_prompt_visible(prompt_is_visible);
    } else {
        ui_set_developer_prompt_visible(prompt_is_visible);
    }
}

static void handle_setup_input(uint8_t pressed) {
    if ((pressed & J_UP) || (pressed & J_DOWN)) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        setup_field = setup_field == SETUP_FIELD_PLAYER_COUNT
            ? SETUP_FIELD_STARTING_LIFE
            : SETUP_FIELD_PLAYER_COUNT;
    } else if (pressed & J_LEFT) {
        uint8_t changed = 0u;

        if (
            (setup_field == SETUP_FIELD_PLAYER_COUNT)
            && (setup_player_count > MIN_PLAYERS)
        ) {
            setup_player_count--;
            changed = 1u;
        } else if (
            (setup_field == SETUP_FIELD_STARTING_LIFE)
            && (setup_starting_life > MIN_STARTING_LIFE)
        ) {
            setup_starting_life -= STARTING_LIFE_STEP;
            changed = 1u;
        }
        sound_play_effect(
            changed ? SOUND_EFFECT_VALUE_DOWN : SOUND_EFFECT_CANCEL
        );
    } else if (pressed & J_RIGHT) {
        uint8_t changed = 0u;

        if (
            (setup_field == SETUP_FIELD_PLAYER_COUNT)
            && (setup_player_count < MAX_PLAYERS)
        ) {
            setup_player_count++;
            changed = 1u;
        } else if (
            (setup_field == SETUP_FIELD_STARTING_LIFE)
            && (setup_starting_life < MAX_STARTING_LIFE)
        ) {
            setup_starting_life += STARTING_LIFE_STEP;
            changed = 1u;
        }
        sound_play_effect(
            changed ? SOUND_EFFECT_VALUE_UP : SOUND_EFFECT_CANCEL
        );
    } else if (pressed & J_B) {
        sound_play_effect(SOUND_EFFECT_CANCEL);
        if (setup_can_cancel) {
            setup_can_cancel = 0u;
            screen_state = SCREEN_OVERVIEW;
            ui_show_overview(
                &game,
                selected_player,
                first_visible_player,
                adjustment_step
            );
            return;
        } else {
            setup_player_count = 4u;
            setup_starting_life = DEFAULT_STARTING_LIFE;
            setup_field = SETUP_FIELD_PLAYER_COUNT;
        }
    } else if (pressed & J_A) {
        sound_play_effect(SOUND_EFFECT_CONFIRM);
        game_state_init(&game, setup_player_count, setup_starting_life);
        selected_player = 0u;
        first_visible_player = 0u;
        selected_field = DETAIL_FIELD_LIFE;
        adjustment_step = LIFE_STEP_SMALL;
        setup_can_cancel = 0u;
        screen_state = SCREEN_OVERVIEW;
        ui_show_overview(
            &game,
            selected_player,
            first_visible_player,
            adjustment_step
        );
        return;
    } else {
        return;
    }

    ui_show_setup(
        setup_player_count,
        setup_starting_life,
        setup_field,
        setup_can_cancel
    );
}

static void handle_overview_input(uint8_t pressed) {
    if (pressed & J_UP) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        selected_player = navigation_previous_player(&game, selected_player);
        update_player_window();
        refresh_overview();
    } else if (pressed & J_DOWN) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        selected_player = navigation_next_player(&game, selected_player);
        update_player_window();
        refresh_overview();
    } else if (pressed & J_LEFT) {
        if (!game.players[selected_player].eliminated) {
            int16_t before_life = game.players[selected_player].life;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_life(
                &game,
                selected_player,
                (int16_t)(-((int16_t)adjustment_step))
            );
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                before_life,
                game.players[selected_player].life
            );
        } else {
            sound_play_effect(SOUND_EFFECT_CANCEL);
        }
        refresh_overview();
    } else if (pressed & J_RIGHT) {
        if (!game.players[selected_player].eliminated) {
            int16_t before_life = game.players[selected_player].life;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_life(&game, selected_player, (int16_t)adjustment_step);
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                before_life,
                game.players[selected_player].life
            );
        } else {
            sound_play_effect(SOUND_EFFECT_CANCEL);
        }
        refresh_overview();
    } else if (pressed & J_SELECT) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        adjustment_step = navigation_next_life_step(adjustment_step);
        refresh_overview();
    } else if (pressed & J_A) {
        sound_play_effect(SOUND_EFFECT_CONFIRM);
        selected_field = DETAIL_FIELD_LIFE;
        screen_state = SCREEN_PLAYER_DETAIL;
        ui_show_player_detail(
            &game,
            selected_player,
            selected_field,
            adjustment_step
        );
    } else if (pressed & J_START) {
        sound_play_effect(SOUND_EFFECT_START);
        screen_state = SCREEN_RESET_CONFIRMATION;
        ui_draw_reset_prompt();
    }
}

static uint8_t toggle_selected_player_status(void) {
    int8_t player_id = (int8_t)selected_player;

    if (game.players[selected_player].eliminated) {
        return 0u;
    }
    if (selected_field == DETAIL_FIELD_MONARCH) {
        return action_set_monarch(
            &game,
            game.monarch_player == player_id ? NO_PLAYER : player_id
        );
    } else if (selected_field == DETAIL_FIELD_INITIATIVE) {
        return action_set_initiative(
            &game,
            game.initiative_player == player_id ? NO_PLAYER : player_id
        );
    }
    return 0u;
}

static void handle_detail_input(uint8_t pressed) {
    if (pressed & J_UP) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        selected_field = navigation_previous_detail_field(selected_field);
    } else if (pressed & J_DOWN) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        selected_field = navigation_next_detail_field(selected_field);
    } else if (pressed & J_LEFT) {
        if (game.players[selected_player].eliminated) {
            sound_play_effect(SOUND_EFFECT_CANCEL);
        } else if (selected_field == DETAIL_FIELD_POISON) {
            uint8_t before_poison = game.players[selected_player].poison;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_poison(
                &game,
                selected_player,
                (int16_t)(-((int16_t)adjustment_step))
            );
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                (int16_t)before_poison,
                (int16_t)game.players[selected_player].poison
            );
        } else if (selected_field == DETAIL_FIELD_RAD) {
            uint8_t before_rad = game.players[selected_player].rad;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_rad(
                &game,
                selected_player,
                (int16_t)(-((int16_t)adjustment_step))
            );
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                (int16_t)before_rad,
                (int16_t)game.players[selected_player].rad
            );
        } else if (selected_field == DETAIL_FIELD_LIFE) {
            int16_t before_life = game.players[selected_player].life;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_life(
                &game,
                selected_player,
                (int16_t)(-((int16_t)adjustment_step))
            );
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                before_life,
                game.players[selected_player].life
            );
        } else {
            sound_play_effect(
                toggle_selected_player_status()
                    ? SOUND_EFFECT_CONFIRM
                    : SOUND_EFFECT_CANCEL
            );
        }
    } else if (pressed & J_RIGHT) {
        if (game.players[selected_player].eliminated) {
            sound_play_effect(SOUND_EFFECT_CANCEL);
        } else if (selected_field == DETAIL_FIELD_POISON) {
            uint8_t before_poison = game.players[selected_player].poison;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_poison(
                &game,
                selected_player,
                (int16_t)adjustment_step
            );
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                (int16_t)before_poison,
                (int16_t)game.players[selected_player].poison
            );
        } else if (selected_field == DETAIL_FIELD_RAD) {
            uint8_t before_rad = game.players[selected_player].rad;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_rad(
                &game,
                selected_player,
                (int16_t)adjustment_step
            );
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                (int16_t)before_rad,
                (int16_t)game.players[selected_player].rad
            );
        } else if (selected_field == DETAIL_FIELD_LIFE) {
            int16_t before_life = game.players[selected_player].life;
            RuleStatus before_status = rules_check_player(
                &game,
                selected_player
            );

            action_change_life(
                &game,
                selected_player,
                (int16_t)adjustment_step
            );
            play_value_feedback(
                before_status,
                rules_check_player(&game, selected_player),
                before_life,
                game.players[selected_player].life
            );
        } else {
            sound_play_effect(
                toggle_selected_player_status()
                    ? SOUND_EFFECT_CONFIRM
                    : SOUND_EFFECT_CANCEL
            );
        }
    } else if (pressed & J_SELECT) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        adjustment_step = navigation_next_life_step(adjustment_step);
    } else if (pressed & J_START) {
        sound_play_effect(SOUND_EFFECT_START);
        screen_state = SCREEN_ELIMINATION_CONFIRMATION;
        ui_draw_elimination_prompt(&game.players[selected_player]);
        return;
    } else if (pressed & J_A) {
        if (
            (selected_field == DETAIL_FIELD_COMMANDER)
            && !game.players[selected_player].eliminated
        ) {
            sound_play_effect(SOUND_EFFECT_CONFIRM);
            open_commander_search();
            return;
        } else if (
            (selected_field == DETAIL_FIELD_COMMANDER_DAMAGE)
            && !game.players[selected_player].eliminated
        ) {
            sound_play_effect(SOUND_EFFECT_CONFIRM);
            selected_source = selected_player == 0u ? 1u : 0u;
            first_visible_source = 0u;
            screen_state = SCREEN_COMMANDER_DAMAGE;
            ui_show_commander_damage(
                &game,
                selected_player,
                selected_source,
                first_visible_source,
                adjustment_step
            );
            return;
        }
        sound_play_effect(
            toggle_selected_player_status()
                ? SOUND_EFFECT_CONFIRM
                : SOUND_EFFECT_CANCEL
        );
    } else if (pressed & J_B) {
        sound_play_effect(SOUND_EFFECT_CANCEL);
        screen_state = SCREEN_OVERVIEW;
        ui_show_overview(
            &game,
            selected_player,
            first_visible_player,
            adjustment_step
        );
        return;
    } else {
        return;
    }

    ui_refresh_player_detail(
        &game,
        selected_player,
        selected_field,
        adjustment_step
    );
}

static void handle_elimination_input(uint8_t pressed) {
    if (pressed & J_A) {
        if (game.players[selected_player].eliminated) {
            action_restore_player(&game, selected_player);
            sound_play_effect(SOUND_EFFECT_CONFIRM);
        } else {
            action_eliminate_player(&game, selected_player);
            sound_play_effect(SOUND_EFFECT_ELIMINATION);
        }
        screen_state = SCREEN_OVERVIEW;
        ui_show_overview(
            &game,
            selected_player,
            first_visible_player,
            adjustment_step
        );
    } else if (pressed & J_B) {
        sound_play_effect(SOUND_EFFECT_CANCEL);
        screen_state = SCREEN_PLAYER_DETAIL;
        ui_show_player_detail(
            &game,
            selected_player,
            selected_field,
            adjustment_step
        );
    }
}

static void handle_commander_damage_input(uint8_t pressed) {
    if (pressed & J_UP) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        selected_source = navigation_previous_other_player(
            &game,
            selected_source,
            selected_player
        );
        update_source_window();
    } else if (pressed & J_DOWN) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        selected_source = navigation_next_other_player(
            &game,
            selected_source,
            selected_player
        );
        update_source_window();
    } else if (pressed & J_LEFT) {
        uint8_t before_damage = game.commander_damage[
            selected_player
        ][selected_source][0u];
        RuleStatus before_status = rules_check_player(&game, selected_player);

        action_change_commander_damage(
            &game,
            selected_player,
            selected_source,
            0u,
            (int16_t)(-((int16_t)adjustment_step))
        );
        play_value_feedback(
            before_status,
            rules_check_player(&game, selected_player),
            (int16_t)before_damage,
            (int16_t)game.commander_damage[
                selected_player
            ][selected_source][0u]
        );
    } else if (pressed & J_RIGHT) {
        uint8_t before_damage = game.commander_damage[
            selected_player
        ][selected_source][0u];
        RuleStatus before_status = rules_check_player(&game, selected_player);

        action_change_commander_damage(
            &game,
            selected_player,
            selected_source,
            0u,
            (int16_t)adjustment_step
        );
        play_value_feedback(
            before_status,
            rules_check_player(&game, selected_player),
            (int16_t)before_damage,
            (int16_t)game.commander_damage[
                selected_player
            ][selected_source][0u]
        );
    } else if (pressed & J_SELECT) {
        sound_play_effect(SOUND_EFFECT_NAVIGATION);
        adjustment_step = navigation_next_life_step(adjustment_step);
    } else if (pressed & J_B) {
        sound_play_effect(SOUND_EFFECT_CANCEL);
        screen_state = SCREEN_PLAYER_DETAIL;
        ui_show_player_detail(
            &game,
            selected_player,
            selected_field,
            adjustment_step
        );
        return;
    } else {
        return;
    }

    ui_refresh_commander_damage(
        &game,
        selected_player,
        selected_source,
        first_visible_source,
        adjustment_step
    );
}

static void handle_reset_input(uint8_t pressed) {
    if (pressed & J_A) {
        sound_play_effect(SOUND_EFFECT_CONFIRM);
        game_state_reset(&game);
        selected_player = 0u;
        first_visible_player = 0u;
        selected_field = DETAIL_FIELD_LIFE;
        adjustment_step = LIFE_STEP_SMALL;
        screen_state = SCREEN_OVERVIEW;
        ui_show_overview(
            &game,
            selected_player,
            first_visible_player,
            adjustment_step
        );
    } else if (pressed & J_B) {
        sound_play_effect(SOUND_EFFECT_CANCEL);
        screen_state = SCREEN_OVERVIEW;
        refresh_overview();
    } else if (pressed & J_SELECT) {
        sound_play_effect(SOUND_EFFECT_CONFIRM);
        setup_player_count = game.player_count;
        setup_starting_life = game.starting_life;
        setup_field = SETUP_FIELD_PLAYER_COUNT;
        setup_can_cancel = 1u;
        screen_state = SCREEN_SETUP;
        ui_show_setup(
            setup_player_count,
            setup_starting_life,
            setup_field,
            setup_can_cancel
        );
    }
}

void main(void) {
    uint8_t previous_keys = 0u;

    ui_initialize();
    ui_show_splash();
    sound_initialize();
    sound_start_intro_music();

    while (1) {
        uint8_t keys = joypad();
        uint8_t pressed = keys & (uint8_t)(~previous_keys);

        if (screen_state == SCREEN_SPLASH) {
            handle_splash_input(pressed);
        } else if (screen_state == SCREEN_DEVELOPER_CREDIT) {
            handle_developer_credit_input(pressed);
        } else if (screen_state == SCREEN_SETUP) {
            handle_setup_input(pressed);
        } else if (screen_state == SCREEN_RESET_CONFIRMATION) {
            handle_reset_input(pressed);
        } else if (screen_state == SCREEN_ELIMINATION_CONFIRMATION) {
            handle_elimination_input(pressed);
        } else if (screen_state == SCREEN_PLAYER_DETAIL) {
            handle_detail_input(pressed);
        } else if (screen_state == SCREEN_COMMANDER_DAMAGE) {
            handle_commander_damage_input(pressed);
        } else if (screen_state == SCREEN_COMMANDER_SEARCH) {
            handle_commander_search_input(pressed);
        } else {
            handle_overview_input(pressed);
        }

        update_blinking_prompt();
        sound_update();
        previous_keys = keys;
        vsync();
    }
}
