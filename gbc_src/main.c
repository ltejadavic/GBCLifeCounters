#include <gb/gb.h>
#include <stdint.h>

#include "actions.h"
#include "constants.h"
#include "game_state.h"
#include "navigation.h"
#include "ui.h"

#define VISIBLE_PLAYER_COUNT 4u
#define SCREEN_SETUP 0u
#define SCREEN_OVERVIEW 1u
#define SCREEN_RESET_CONFIRMATION 2u
#define SCREEN_PLAYER_DETAIL 3u
#define SCREEN_COMMANDER_DAMAGE 4u
#define SCREEN_ELIMINATION_CONFIRMATION 5u

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
static uint8_t screen_state = SCREEN_SETUP;

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

static void handle_setup_input(uint8_t pressed) {
    if ((pressed & J_UP) || (pressed & J_DOWN)) {
        setup_field = setup_field == SETUP_FIELD_PLAYER_COUNT
            ? SETUP_FIELD_STARTING_LIFE
            : SETUP_FIELD_PLAYER_COUNT;
    } else if (pressed & J_LEFT) {
        if (
            (setup_field == SETUP_FIELD_PLAYER_COUNT)
            && (setup_player_count > MIN_PLAYERS)
        ) {
            setup_player_count--;
        } else if (
            (setup_field == SETUP_FIELD_STARTING_LIFE)
            && (setup_starting_life > MIN_STARTING_LIFE)
        ) {
            setup_starting_life -= STARTING_LIFE_STEP;
        }
    } else if (pressed & J_RIGHT) {
        if (
            (setup_field == SETUP_FIELD_PLAYER_COUNT)
            && (setup_player_count < MAX_PLAYERS)
        ) {
            setup_player_count++;
        } else if (
            (setup_field == SETUP_FIELD_STARTING_LIFE)
            && (setup_starting_life < MAX_STARTING_LIFE)
        ) {
            setup_starting_life += STARTING_LIFE_STEP;
        }
    } else if (pressed & J_B) {
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
        selected_player = navigation_previous_player(&game, selected_player);
        update_player_window();
        refresh_overview();
    } else if (pressed & J_DOWN) {
        selected_player = navigation_next_player(&game, selected_player);
        update_player_window();
        refresh_overview();
    } else if (pressed & J_LEFT) {
        if (!game.players[selected_player].eliminated) {
            action_change_life(
                &game,
                selected_player,
                (int16_t)(-((int16_t)adjustment_step))
            );
        }
        refresh_overview();
    } else if (pressed & J_RIGHT) {
        if (!game.players[selected_player].eliminated) {
            action_change_life(&game, selected_player, (int16_t)adjustment_step);
        }
        refresh_overview();
    } else if (pressed & J_SELECT) {
        adjustment_step = navigation_next_life_step(adjustment_step);
        refresh_overview();
    } else if (pressed & J_A) {
        selected_field = DETAIL_FIELD_LIFE;
        screen_state = SCREEN_PLAYER_DETAIL;
        ui_show_player_detail(
            &game,
            selected_player,
            selected_field,
            adjustment_step
        );
    } else if (pressed & J_START) {
        screen_state = SCREEN_RESET_CONFIRMATION;
        ui_draw_reset_prompt();
    }
}

static void toggle_selected_player_status(void) {
    int8_t player_id = (int8_t)selected_player;

    if (game.players[selected_player].eliminated) {
        return;
    }
    if (selected_field == DETAIL_FIELD_MONARCH) {
        action_set_monarch(
            &game,
            game.monarch_player == player_id ? NO_PLAYER : player_id
        );
    } else if (selected_field == DETAIL_FIELD_INITIATIVE) {
        action_set_initiative(
            &game,
            game.initiative_player == player_id ? NO_PLAYER : player_id
        );
    }
}

static void handle_detail_input(uint8_t pressed) {
    if (pressed & J_UP) {
        selected_field = navigation_previous_detail_field(selected_field);
    } else if (pressed & J_DOWN) {
        selected_field = navigation_next_detail_field(selected_field);
    } else if (pressed & J_LEFT) {
        if (game.players[selected_player].eliminated) {
            /* Eliminated players must be restored before editing. */
        } else if (selected_field == DETAIL_FIELD_POISON) {
            action_change_poison(
                &game,
                selected_player,
                (int16_t)(-((int16_t)adjustment_step))
            );
        } else if (selected_field == DETAIL_FIELD_LIFE) {
            action_change_life(
                &game,
                selected_player,
                (int16_t)(-((int16_t)adjustment_step))
            );
        } else {
            toggle_selected_player_status();
        }
    } else if (pressed & J_RIGHT) {
        if (game.players[selected_player].eliminated) {
            /* Eliminated players must be restored before editing. */
        } else if (selected_field == DETAIL_FIELD_POISON) {
            action_change_poison(
                &game,
                selected_player,
                (int16_t)adjustment_step
            );
        } else if (selected_field == DETAIL_FIELD_LIFE) {
            action_change_life(
                &game,
                selected_player,
                (int16_t)adjustment_step
            );
        } else {
            toggle_selected_player_status();
        }
    } else if (pressed & J_SELECT) {
        adjustment_step = navigation_next_life_step(adjustment_step);
    } else if (pressed & J_START) {
        screen_state = SCREEN_ELIMINATION_CONFIRMATION;
        ui_draw_elimination_prompt(&game.players[selected_player]);
        return;
    } else if (pressed & J_A) {
        if (
            (selected_field == DETAIL_FIELD_COMMANDER_DAMAGE)
            && !game.players[selected_player].eliminated
        ) {
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
        toggle_selected_player_status();
    } else if (pressed & J_B) {
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
        } else {
            action_eliminate_player(&game, selected_player);
        }
        screen_state = SCREEN_OVERVIEW;
        ui_show_overview(
            &game,
            selected_player,
            first_visible_player,
            adjustment_step
        );
    } else if (pressed & J_B) {
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
        selected_source = navigation_previous_other_player(
            &game,
            selected_source,
            selected_player
        );
        update_source_window();
    } else if (pressed & J_DOWN) {
        selected_source = navigation_next_other_player(
            &game,
            selected_source,
            selected_player
        );
        update_source_window();
    } else if (pressed & J_LEFT) {
        action_change_commander_damage(
            &game,
            selected_player,
            selected_source,
            0u,
            (int16_t)(-((int16_t)adjustment_step))
        );
    } else if (pressed & J_RIGHT) {
        action_change_commander_damage(
            &game,
            selected_player,
            selected_source,
            0u,
            (int16_t)adjustment_step
        );
    } else if (pressed & J_SELECT) {
        adjustment_step = navigation_next_life_step(adjustment_step);
    } else if (pressed & J_B) {
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
        screen_state = SCREEN_OVERVIEW;
        refresh_overview();
    } else if (pressed & J_SELECT) {
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
    ui_show_setup(
        setup_player_count,
        setup_starting_life,
        setup_field,
        setup_can_cancel
    );

    while (1) {
        uint8_t keys = joypad();
        uint8_t pressed = keys & (uint8_t)(~previous_keys);

        if (screen_state == SCREEN_SETUP) {
            handle_setup_input(pressed);
        } else if (screen_state == SCREEN_RESET_CONFIRMATION) {
            handle_reset_input(pressed);
        } else if (screen_state == SCREEN_ELIMINATION_CONFIRMATION) {
            handle_elimination_input(pressed);
        } else if (screen_state == SCREEN_PLAYER_DETAIL) {
            handle_detail_input(pressed);
        } else if (screen_state == SCREEN_COMMANDER_DAMAGE) {
            handle_commander_damage_input(pressed);
        } else {
            handle_overview_input(pressed);
        }

        previous_keys = keys;
        vsync();
    }
}
