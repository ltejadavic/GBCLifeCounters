#include <gb/cgb.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>

#include "rules.h"
#include "ui.h"

static const palette_color_t normal_palette[] = {
    RGB8(245, 239, 224),
    RGB8(174, 205, 100),
    RGB8(67, 117, 69),
    RGB8(19, 43, 35),
};

static const palette_color_t warning_palette[] = {
    RGB8(255, 246, 204),
    RGB8(255, 207, 79),
    RGB8(191, 105, 28),
    RGB8(67, 38, 19),
};

static const palette_color_t loss_palette[] = {
    RGB8(255, 232, 220),
    RGB8(239, 129, 106),
    RGB8(155, 51, 52),
    RGB8(54, 24, 30),
};

static void draw_static_screen(void) {
    gotoxy(3, 1);
    printf("COMMANDER GBC");
    gotoxy(2, 3);
    printf("1 PLAYER COUNTER");

    gotoxy(7, 6);
    printf("LIFE");

    gotoxy(1, 12);
    printf("LEFT/RIGHT  -/+1");
    gotoxy(1, 13);
    printf("DOWN/UP     -/+5");
    gotoxy(1, 14);
    printf("B/A        -/+10");
    gotoxy(1, 15);
    printf("START       RESET");
}

static void draw_status(LifeStatus status) {
    gotoxy(1, 10);
    if (status == LIFE_STATUS_POSSIBLE_LOSS) {
        set_bkg_palette(0, 1, loss_palette);
        printf("STATUS: MAY LOSE  ");
    } else if (status == LIFE_STATUS_WARNING) {
        set_bkg_palette(0, 1, warning_palette);
        printf("STATUS: LOW LIFE  ");
    } else {
        set_bkg_palette(0, 1, normal_palette);
        printf("STATUS: OK        ");
    }
}

void ui_initialize(void) {
    set_bkg_palette(0, 1, normal_palette);
    draw_static_screen();
    SHOW_BKG;
    DISPLAY_ON;
}

void ui_refresh_player(const GameState *game, uint8_t player_id) {
    const Player *player = &game->players[player_id];

    gotoxy(8, 5);
    printf("%-8s", player->name);
    gotoxy(7, 8);
    printf("%6d", player->life);
    draw_status(rules_check_life(player));
}
