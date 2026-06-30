#include <gb/cgb.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>

#include "navigation.h"
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

static uint8_t palette_for_status(RuleStatus status) {
    if (status == RULE_STATUS_POSSIBLE_LOSS) {
        return LOSS_PALETTE;
    }
    if (status == RULE_STATUS_WARNING) {
        return WARNING_PALETTE;
    }
    return NORMAL_PALETTE;
}

static void prepare_screen(void) {
    cls();
    set_region_palette(
        0u,
        0u,
        DEVICE_SCREEN_WIDTH,
        DEVICE_SCREEN_HEIGHT,
        NORMAL_PALETTE
    );
}

static void draw_color_diagnostic(void) {
    gotoxy(1u, 17u);
    if (DEVICE_SUPPORTS_COLOR) {
        printf("CGB COLOR: ACTIVE");
    } else {
        printf("CGB COLOR: OFF   ");
    }
}

static void clear_help_area(void) {
    uint8_t row;

    for (row = 11u; row <= 15u; row++) {
        gotoxy(0u, row);
        printf("                    ");
    }
}

static void print_status_word(RuleStatus status) {
    if (status == RULE_STATUS_POSSIBLE_LOSS) {
        printf("LOSS");
    } else if (status == RULE_STATUS_WARNING) {
        printf("WARN");
    } else {
        printf("OK  ");
    }
}

static void draw_overview_header(void) {
    gotoxy(3u, 0u);
    printf("COMMANDER GBC");
    gotoxy(2u, 1u);
    printf("4P LIFE / POISON");
    draw_color_diagnostic();
}

static void draw_player_row(
    const GameState *game,
    const Player *player,
    uint8_t row,
    uint8_t is_selected
) {
    RuleStatus life_status = rules_check_life(player);
    RuleStatus poison_status = rules_check_poison(game, player);
    RuleStatus player_status = rules_check_player(game, player);
    char life_text[LIFE_TEXT_BUFFER_SIZE];
    char poison_text[COUNTER_TEXT_BUFFER_SIZE];

    format_life_total(player->life, life_text);
    format_counter_value(player->poison, poison_text);
    gotoxy(0u, row);
    printf("                    ");
    gotoxy(0u, row);
    printf(is_selected ? ">" : " ");
    gotoxy(1u, row);
    printf("%s", player->name);
    gotoxy(4u, row);
    printf("%s", life_text);
    gotoxy(11u, row);
    printf("P");
    gotoxy(12u, row);
    printf("%s", poison_text);
    gotoxy(16u, row);
    if (player_status == RULE_STATUS_POSSIBLE_LOSS) {
        printf("LOSS");
    } else if (
        (life_status == RULE_STATUS_WARNING)
        && (poison_status == RULE_STATUS_WARNING)
    ) {
        printf("WARN");
    } else if (poison_status == RULE_STATUS_WARNING) {
        printf("PSN ");
    } else if (life_status == RULE_STATUS_WARNING) {
        printf("LOW ");
    } else {
        printf("OK  ");
    }
    set_region_palette(
        0u,
        row,
        DEVICE_SCREEN_WIDTH,
        1u,
        palette_for_status(player_status)
    );
}

static void draw_overview_controls(uint8_t life_step) {
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
    gotoxy(1u, 14u);
    printf("A PLAYER DETAILS");
    gotoxy(1u, 15u);
    printf("START RESET GAME");
}

static void draw_detail_header(const Player *player) {
    gotoxy(3u, 0u);
    printf("COMMANDER GBC");
    gotoxy(2u, 1u);
    printf("PLAYER ");
    printf("%s", player->name);
    printf(" DETAILS");
    draw_color_diagnostic();
}

static void draw_detail_fields(
    const GameState *game,
    const Player *player,
    uint8_t selected_field
) {
    RuleStatus life_status = rules_check_life(player);
    RuleStatus poison_status = rules_check_poison(game, player);
    char life_text[LIFE_TEXT_BUFFER_SIZE];
    char poison_text[COUNTER_TEXT_BUFFER_SIZE];

    format_life_total(player->life, life_text);
    format_counter_value(player->poison, poison_text);

    gotoxy(0u, 3u);
    printf("                    ");
    gotoxy(0u, 3u);
    printf(selected_field == DETAIL_FIELD_LIFE ? ">" : " ");
    gotoxy(2u, 3u);
    printf("LIFE");
    gotoxy(9u, 3u);
    printf("%s", life_text);
    set_region_palette(
        0u,
        3u,
        DEVICE_SCREEN_WIDTH,
        1u,
        palette_for_status(life_status)
    );

    gotoxy(0u, 5u);
    printf("                    ");
    gotoxy(0u, 5u);
    printf(selected_field == DETAIL_FIELD_POISON ? ">" : " ");
    gotoxy(2u, 5u);
    printf("POISON");
    gotoxy(12u, 5u);
    printf("%s", poison_text);
    set_region_palette(
        0u,
        5u,
        DEVICE_SCREEN_WIDTH,
        1u,
        palette_for_status(poison_status)
    );

    gotoxy(1u, 7u);
    printf("LIFE STATUS: ");
    print_status_word(life_status);
    gotoxy(1u, 8u);
    printf("PSN STATUS : ");
    print_status_word(poison_status);
}

static void draw_detail_controls(uint8_t adjustment_step) {
    clear_help_area();
    gotoxy(1u, 11u);
    printf("UP/DOWN FIELD");
    gotoxy(1u, 12u);
    printf("LEFT/RIGHT CHANGE");
    gotoxy(1u, 13u);
    if (adjustment_step == 1u) {
        printf("SELECT STEP: 1");
    } else if (adjustment_step == 5u) {
        printf("SELECT STEP: 5");
    } else {
        printf("SELECT STEP: 10");
    }
    gotoxy(1u, 15u);
    printf("B BACK");
}

void ui_initialize(void) {
    set_bkg_palette(BKGF_CGB_PAL0, 3u, background_palettes);
    SHOW_BKG;
    DISPLAY_ON;
}

void ui_show_overview(
    const GameState *game,
    uint8_t selected_player,
    uint8_t life_step
) {
    prepare_screen();
    draw_overview_header();
    ui_refresh_overview(game, selected_player, life_step);
}

void ui_refresh_overview(
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
            game,
            &game->players[player_id],
            player_rows[player_id],
            player_id == selected_player
        );
    }
    draw_overview_controls(life_step);
}

void ui_show_player_detail(
    const GameState *game,
    uint8_t player_id,
    uint8_t selected_field,
    uint8_t adjustment_step
) {
    prepare_screen();
    draw_detail_header(&game->players[player_id]);
    ui_refresh_player_detail(
        game,
        player_id,
        selected_field,
        adjustment_step
    );
}

void ui_refresh_player_detail(
    const GameState *game,
    uint8_t player_id,
    uint8_t selected_field,
    uint8_t adjustment_step
) {
    draw_detail_fields(game, &game->players[player_id], selected_field);
    draw_detail_controls(adjustment_step);
}

void ui_draw_reset_prompt(void) {
    clear_help_area();
    gotoxy(1u, 12u);
    printf("RESET ALL PLAYERS?");
    gotoxy(3u, 14u);
    printf("A YES    B NO");
}
