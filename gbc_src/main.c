#include <gb/gb.h>
#include <stdint.h>

#include "actions.h"
#include "constants.h"
#include "game_state.h"
#include "navigation.h"
#include "ui.h"

#define PLAYER_COUNT 4u
#define SCREEN_OVERVIEW 0u
#define SCREEN_RESET_CONFIRMATION 1u

static GameState game;
static uint8_t selected_player = 0u;
static uint8_t life_step = LIFE_STEP_SMALL;
static uint8_t screen_state = SCREEN_OVERVIEW;

static void handle_overview_input(uint8_t pressed) {
    if (pressed & J_UP) {
        selected_player = navigation_previous_player(&game, selected_player);
        ui_draw_overview(&game, selected_player, life_step);
    } else if (pressed & J_DOWN) {
        selected_player = navigation_next_player(&game, selected_player);
        ui_draw_overview(&game, selected_player, life_step);
    } else if (pressed & J_LEFT) {
        action_change_life(
            &game,
            selected_player,
            (int16_t)(-((int16_t)life_step))
        );
        ui_draw_overview(&game, selected_player, life_step);
    } else if (pressed & J_RIGHT) {
        action_change_life(&game, selected_player, (int16_t)life_step);
        ui_draw_overview(&game, selected_player, life_step);
    } else if (pressed & J_SELECT) {
        life_step = navigation_next_life_step(life_step);
        ui_draw_overview(&game, selected_player, life_step);
    } else if (pressed & J_START) {
        screen_state = SCREEN_RESET_CONFIRMATION;
        ui_draw_reset_prompt();
    }
}

static void handle_reset_input(uint8_t pressed) {
    if (pressed & J_A) {
        game_state_reset(&game);
        selected_player = 0u;
        life_step = LIFE_STEP_SMALL;
        screen_state = SCREEN_OVERVIEW;
        ui_draw_overview(&game, selected_player, life_step);
    } else if (pressed & J_B) {
        screen_state = SCREEN_OVERVIEW;
        ui_draw_overview(&game, selected_player, life_step);
    }
}

void main(void) {
    uint8_t previous_keys = 0u;

    game_state_init(&game, PLAYER_COUNT, DEFAULT_STARTING_LIFE);
    ui_initialize();
    ui_draw_overview(&game, selected_player, life_step);

    while (1) {
        uint8_t keys = joypad();
        uint8_t pressed = keys & (uint8_t)(~previous_keys);

        if (screen_state == SCREEN_RESET_CONFIRMATION) {
            handle_reset_input(pressed);
        } else {
            handle_overview_input(pressed);
        }

        previous_keys = keys;
        vsync();
    }
}
