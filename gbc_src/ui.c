#include <gb/cgb.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>

#include "rules.h"
#include "text_format.h"
#include "ui.h"

#define OVERVIEW_PLAYER_LIMIT 4u
#define NORMAL_PALETTE 0u
#define WARNING_PALETTE 1u
#define LOSS_PALETTE 2u

static const uint8_t player_rows[OVERVIEW_PLAYER_LIMIT] = {3u, 5u, 7u, 9u};

static const palette_color_t background_palettes[] = {
    RGB8(245, 239, 224),
    RGB8(174, 205, 100),
    RGB8(67, 117, 69),
    RGB8(19, 43, 35),
    RGB8(255, 246, 204),
    RGB8(255, 207, 79),
    RGB8(191, 105, 28),
    RGB8(67, 38, 19),
    RGB8(255, 232, 220),
    RGB8(239, 129, 106),
    RGB8(155, 51, 52),
    RGB8(54, 24, 30),
};

static void set_region_palette(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    uint8_t palette
) {
    if (!DEVICE_SUPPORTS_COLOR) {
        return;
    }
    VBK_REG = VBK_ATTRIBUTES;
    fill_bkg_rect(x, y, width, height, palette);
    VBK_REG = VBK_TILES;
}

static void clear_help_area(void) {
    uint8_t row;

    for (row = 11u; row <= 15u; row++) {
        gotoxy(0u, row);
        printf("                    ");
    }
}

static void draw_static_screen(void) {
    gotoxy(3u, 0u);
    printf("COMMANDER GBC");
    gotoxy(3u, 1u);
    printf("4 PLAYER LIFE");

    gotoxy(1u, 17u);
    if (DEVICE_SUPPORTS_COLOR) {
        printf("CGB COLOR: ACTIVE");
    } else {
        printf("CGB COLOR: OFF   ");
    }
}

static uint8_t palette_for_status(LifeStatus status) {
    if (status == LIFE_STATUS_POSSIBLE_LOSS) {
        return LOSS_PALETTE;
    }
    if (status == LIFE_STATUS_WARNING) {
        return WARNING_PALETTE;
    }
    return NORMAL_PALETTE;
}

static void draw_player_row(
    const Player *player,
    uint8_t row,
    uint8_t is_selected
) {
    LifeStatus status = rules_check_life(player);
    char life_text[LIFE_TEXT_BUFFER_SIZE];

    format_life_total(player->life, life_text);
    gotoxy(0u, row);
    printf("                    ");
    gotoxy(0u, row);
    printf(is_selected ? ">" : " ");
    gotoxy(2u, row);
    printf("%s", player->name);
    gotoxy(5u, row);
    printf("%s", life_text);
    gotoxy(13u, row);
    if (status == LIFE_STATUS_POSSIBLE_LOSS) {
        printf("LOSS");
    } else if (status == LIFE_STATUS_WARNING) {
        printf("LOW ");
    } else {
        printf("OK  ");
    }
    set_region_palette(
        0u,
        row,
        DEVICE_SCREEN_WIDTH,
        1u,
        palette_for_status(status)
    );
}

static void draw_controls(uint8_t life_step) {
    clear_help_area();
    gotoxy(1u, 11u);
    printf("UP/DOWN SELECT");
    gotoxy(1u, 12u);
    printf("LEFT/RIGHT CHANGE");
    gotoxy(1u, 13u);
    if (life_step == 1u) {
        printf("SELECT STEP: 1");
    } else if (life_step == 5u) {
        printf("SELECT STEP: 5");
    } else {
        printf("SELECT STEP: 10");
    }
    gotoxy(1u, 15u);
    printf("START RESET GAME");
}

void ui_initialize(void) {
    set_bkg_palette(BKGF_CGB_PAL0, 3u, background_palettes);
    set_region_palette(
        0u,
        0u,
        DEVICE_SCREEN_WIDTH,
        DEVICE_SCREEN_HEIGHT,
        NORMAL_PALETTE
    );
    draw_static_screen();
    SHOW_BKG;
    DISPLAY_ON;
}

void ui_draw_overview(
    const GameState *game,
    uint8_t selected_player,
    uint8_t life_step
) {
    uint8_t player_id;
    uint8_t visible_players = game->player_count;

    if (visible_players > OVERVIEW_PLAYER_LIMIT) {
        visible_players = OVERVIEW_PLAYER_LIMIT;
    }
    for (player_id = 0u; player_id < visible_players; player_id++) {
        draw_player_row(
            &game->players[player_id],
            player_rows[player_id],
            player_id == selected_player
        );
    }
    draw_controls(life_step);
}

void ui_draw_reset_prompt(void) {
    clear_help_area();
    gotoxy(1u, 12u);
    printf("RESET ALL PLAYERS?");
    gotoxy(3u, 14u);
    printf("A YES    B NO");
}
