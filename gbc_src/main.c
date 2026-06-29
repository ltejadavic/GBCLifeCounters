#include <gb/gb.h>
#include <stdint.h>

#include "actions.h"
#include "constants.h"
#include "game_state.h"
#include "ui.h"

static GameState game;

static uint8_t apply_input(uint8_t pressed) {
    if (pressed & J_LEFT) {
        return action_change_life(&game, 0u, -1);
    }
    if (pressed & J_RIGHT) {
        return action_change_life(&game, 0u, 1);
    }
    if (pressed & J_DOWN) {
        return action_change_life(&game, 0u, -5);
    }
    if (pressed & J_UP) {
        return action_change_life(&game, 0u, 5);
    }
    if (pressed & J_B) {
        return action_change_life(&game, 0u, -10);
    }
    if (pressed & J_A) {
        return action_change_life(&game, 0u, 10);
    }
    if (pressed & J_START) {
        return action_reset_player(&game, 0u);
    }
    return 0u;
}

void main(void) {
    uint8_t previous_keys = 0u;

    game_state_init(&game, 1u, DEFAULT_STARTING_LIFE);
    ui_initialize();
    ui_refresh_player(&game, 0u);

    while (1) {
        uint8_t keys = joypad();
        uint8_t pressed = keys & (uint8_t)(~previous_keys);

        if (apply_input(pressed)) {
            ui_refresh_player(&game, 0u);
        }

        previous_keys = keys;
        vsync();
    }
}
