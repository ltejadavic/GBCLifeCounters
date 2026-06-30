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
#define ELIMINATED_PALETTE 3u

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
    RGB8(232, 235, 239),
    RGB8(158, 171, 183),
    RGB8(75, 88, 103),
    RGB8(25, 31, 40),
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

static void draw_overview_header(
    const GameState *game,
    uint8_t first_visible_player
) {
    uint8_t last_visible_player = (uint8_t)(
        first_visible_player + OVERVIEW_PLAYER_LIMIT
    );
    char range_text[] = "PLAYERS 1-4 OF 4";

    if (last_visible_player > game->player_count) {
        last_visible_player = game->player_count;
    }
    range_text[8] = (char)('1' + first_visible_player);
    range_text[10] = (char)('0' + last_visible_player);
    range_text[15] = (char)('0' + game->player_count);
    gotoxy(3u, 0u);
    printf("COMMANDER GBC");
    gotoxy(1u, 1u);
    printf("%s", range_text);
    gotoxy(0u, 2u);
    printf("MON");
    gotoxy(4u, 2u);
    if (game->monarch_player == NO_PLAYER) {
        printf("--");
    } else {
        printf("%s", game->players[(uint8_t)game->monarch_player].name);
    }
    gotoxy(8u, 2u);
    printf("INI");
    gotoxy(12u, 2u);
    if (game->initiative_player == NO_PLAYER) {
        printf("--");
    } else {
        printf("%s", game->players[(uint8_t)game->initiative_player].name);
    }
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
    RuleStatus commander_status = rules_check_commander_damage_for_player(
        game,
        player->player_id
    );
    RuleStatus player_status = rules_check_player(game, player->player_id);
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
    if (player->eliminated) {
        printf("OUT ");
    } else if (player_status == RULE_STATUS_POSSIBLE_LOSS) {
        printf("LOSS");
    } else if (
        ((life_status == RULE_STATUS_WARNING) ? 1u : 0u)
        + ((poison_status == RULE_STATUS_WARNING) ? 1u : 0u)
        + ((commander_status == RULE_STATUS_WARNING) ? 1u : 0u)
        > 1u
    ) {
        printf("WARN");
    } else if (commander_status == RULE_STATUS_WARNING) {
        printf("CMD ");
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
        player->eliminated
            ? ELIMINATED_PALETTE
            : palette_for_status(player_status)
    );
}

static void draw_winner(const GameState *game) {
    int8_t winner = rules_check_winner(game);
    char turn_text[UINT16_TEXT_BUFFER_SIZE];

    gotoxy(0u, 16u);
    printf("                    ");
    if (winner != NO_PLAYER) {
        gotoxy(3u, 16u);
        printf("WINNER: ");
        printf("%s", game->players[(uint8_t)winner].name);
    } else {
        format_uint16_value(game->turn_number, turn_text);
        gotoxy(1u, 16u);
        printf("ACT");
        gotoxy(5u, 16u);
        if (game->active_player == NO_PLAYER) {
            printf("--");
        } else {
            printf("%s", game->players[(uint8_t)game->active_player].name);
        }
        gotoxy(9u, 16u);
        printf("TURN");
        gotoxy(14u, 16u);
        printf("%s", turn_text);
    }
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
    printf("A PLAYER  B GLOBAL");
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
    RuleStatus commander_status = rules_check_commander_damage_for_player(
        game,
        player->player_id
    );
    char life_text[LIFE_TEXT_BUFFER_SIZE];
    char poison_text[COUNTER_TEXT_BUFFER_SIZE];
    char commander_text[COUNTER_TEXT_BUFFER_SIZE];

    format_life_total(player->life, life_text);
    format_counter_value(player->poison, poison_text);
    format_counter_value(
        rules_get_highest_commander_damage(game, player->player_id),
        commander_text
    );

    gotoxy(0u, 3u);
    printf("                    ");
    gotoxy(0u, 3u);
    printf(selected_field == DETAIL_FIELD_LIFE ? ">" : " ");
    gotoxy(2u, 3u);
    printf("LIFE");
    gotoxy(9u, 3u);
    printf("%s", life_text);
    gotoxy(16u, 3u);
    if (player->eliminated) {
        printf("OUT ");
    } else {
        print_status_word(life_status);
    }
    set_region_palette(
        0u,
        3u,
        DEVICE_SCREEN_WIDTH,
        1u,
        player->eliminated
            ? ELIMINATED_PALETTE
            : palette_for_status(life_status)
    );

    gotoxy(0u, 5u);
    printf("                    ");
    gotoxy(0u, 5u);
    printf(selected_field == DETAIL_FIELD_POISON ? ">" : " ");
    gotoxy(2u, 5u);
    printf("POISON");
    gotoxy(12u, 5u);
    printf("%s", poison_text);
    gotoxy(16u, 5u);
    if (player->eliminated) {
        printf("OUT ");
    } else {
        print_status_word(poison_status);
    }
    set_region_palette(
        0u,
        5u,
        DEVICE_SCREEN_WIDTH,
        1u,
        player->eliminated
            ? ELIMINATED_PALETTE
            : palette_for_status(poison_status)
    );

    gotoxy(0u, 7u);
    printf("                    ");
    gotoxy(0u, 7u);
    printf(selected_field == DETAIL_FIELD_COMMANDER_DAMAGE ? ">" : " ");
    gotoxy(2u, 7u);
    printf("CMD MAX");
    gotoxy(12u, 7u);
    printf("%s", commander_text);
    gotoxy(16u, 7u);
    if (player->eliminated) {
        printf("OUT ");
    } else {
        print_status_word(commander_status);
    }
    set_region_palette(
        0u,
        7u,
        DEVICE_SCREEN_WIDTH,
        1u,
        player->eliminated
            ? ELIMINATED_PALETTE
            : palette_for_status(commander_status)
    );
}

static void draw_detail_controls(
    uint8_t adjustment_step,
    uint8_t player_is_eliminated
) {
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
    gotoxy(1u, 14u);
    if (player_is_eliminated) {
        printf("START RESTORE");
    } else {
        printf("A CMD  START OUT");
    }
    gotoxy(1u, 15u);
    printf("B BACK");
}

static void draw_commander_header(const Player *target) {
    gotoxy(3u, 0u);
    printf("COMMANDER GBC");
    gotoxy(1u, 1u);
    printf("CMD DAMAGE TO ");
    printf("%s", target->name);
    draw_color_diagnostic();
}

static void draw_commander_source_row(
    const GameState *game,
    uint8_t target_player,
    uint8_t source_player,
    uint8_t row,
    uint8_t is_selected
) {
    uint8_t is_self_source = target_player == source_player;
    RuleStatus status = rules_check_commander_damage(
        game,
        target_player,
        source_player,
        0u
    );
    char damage_text[COUNTER_TEXT_BUFFER_SIZE];

    format_counter_value(
        game->commander_damage[target_player][source_player][0u],
        damage_text
    );
    if (is_self_source) {
        damage_text[0] = '-';
        damage_text[1] = '-';
        damage_text[2] = '-';
    }
    gotoxy(0u, row);
    printf("                    ");
    gotoxy(0u, row);
    printf((is_selected && !is_self_source) ? ">" : " ");
    gotoxy(2u, row);
    printf("FROM");
    gotoxy(7u, row);
    printf("%s", game->players[source_player].name);
    gotoxy(11u, row);
    printf("%s", damage_text);
    gotoxy(15u, row);
    if (is_self_source) {
        printf("SELF");
    } else {
        print_status_word(status);
    }
    set_region_palette(
        0u,
        row,
        DEVICE_SCREEN_WIDTH,
        1u,
        is_self_source ? ELIMINATED_PALETTE : palette_for_status(status)
    );
}

static void draw_commander_controls(uint8_t adjustment_step) {
    clear_help_area();
    gotoxy(1u, 11u);
    printf("UP/DOWN SOURCE");
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

static void print_player_or_none(const GameState *game, int8_t player_id) {
    if (player_id == NO_PLAYER) {
        printf("--");
    } else {
        printf("%s", game->players[(uint8_t)player_id].name);
    }
}

static void draw_global_field_marker(uint8_t row, uint8_t is_selected) {
    gotoxy(0u, row);
    printf(is_selected ? ">" : " ");
}

static void draw_global_controls(uint8_t adjustment_step) {
    gotoxy(1u, 13u);
    printf("UP/DOWN FIELD");
    gotoxy(1u, 14u);
    printf("LEFT/RIGHT CHANGE");
    gotoxy(1u, 15u);
    printf("A ACTION  B BACK");
    gotoxy(1u, 16u);
    if (adjustment_step == 1u) {
        printf("SELECT STEP: 1");
    } else if (adjustment_step == 5u) {
        printf("SELECT STEP: 5");
    } else {
        printf("SELECT STEP: 10");
    }
}

void ui_initialize(void) {
    set_bkg_palette(BKGF_CGB_PAL0, 4u, background_palettes);
    SHOW_BKG;
    DISPLAY_ON;
}

void ui_show_setup(
    uint8_t player_count,
    int16_t starting_life,
    uint8_t selected_field,
    uint8_t can_cancel
) {
    char player_count_text[COUNTER_TEXT_BUFFER_SIZE];
    char life_text[LIFE_TEXT_BUFFER_SIZE];

    prepare_screen();
    format_counter_value(player_count, player_count_text);
    format_life_total(starting_life, life_text);

    gotoxy(3u, 0u);
    printf("COMMANDER GBC");
    gotoxy(5u, 2u);
    printf("GAME SETUP");

    gotoxy(0u, 5u);
    printf(selected_field == SETUP_FIELD_PLAYER_COUNT ? ">" : " ");
    gotoxy(2u, 5u);
    printf("PLAYERS");
    gotoxy(13u, 5u);
    printf("%s", player_count_text);

    gotoxy(0u, 7u);
    printf(selected_field == SETUP_FIELD_STARTING_LIFE ? ">" : " ");
    gotoxy(2u, 7u);
    printf("START LIFE");
    gotoxy(13u, 7u);
    printf("%s", life_text);

    gotoxy(3u, 9u);
    printf("2-8 PLAYERS");
    gotoxy(1u, 11u);
    printf("UP/DOWN FIELD");
    gotoxy(1u, 12u);
    printf("LEFT/RIGHT CHANGE");
    gotoxy(1u, 14u);
    printf("A START GAME");
    gotoxy(1u, 15u);
    if (can_cancel) {
        printf("B CANCEL");
    } else {
        printf("B DEFAULTS");
    }
    draw_color_diagnostic();
}

void ui_show_overview(
    const GameState *game,
    uint8_t selected_player,
    uint8_t first_visible_player,
    uint8_t life_step
) {
    prepare_screen();
    ui_refresh_overview(
        game,
        selected_player,
        first_visible_player,
        life_step
    );
}

void ui_refresh_overview(
    const GameState *game,
    uint8_t selected_player,
    uint8_t first_visible_player,
    uint8_t life_step
) {
    uint8_t visible_index;
    uint8_t visible_players = (uint8_t)(
        game->player_count - first_visible_player
    );

    draw_overview_header(game, first_visible_player);
    if (visible_players > OVERVIEW_PLAYER_LIMIT) {
        visible_players = OVERVIEW_PLAYER_LIMIT;
    }
    for (visible_index = 0u; visible_index < visible_players; visible_index++) {
        uint8_t player_id = (uint8_t)(first_visible_player + visible_index);
        draw_player_row(
            game,
            &game->players[player_id],
            player_rows[visible_index],
            player_id == selected_player
        );
    }
    draw_overview_controls(life_step);
    draw_winner(game);
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
    draw_detail_controls(
        adjustment_step,
        game->players[player_id].eliminated
    );
}

void ui_show_commander_damage(
    const GameState *game,
    uint8_t target_player,
    uint8_t selected_source,
    uint8_t first_visible_source,
    uint8_t adjustment_step
) {
    prepare_screen();
    draw_commander_header(&game->players[target_player]);
    ui_refresh_commander_damage(
        game,
        target_player,
        selected_source,
        first_visible_source,
        adjustment_step
    );
}

void ui_refresh_commander_damage(
    const GameState *game,
    uint8_t target_player,
    uint8_t selected_source,
    uint8_t first_visible_source,
    uint8_t adjustment_step
) {
    uint8_t visible_index;
    uint8_t visible_sources = (uint8_t)(
        game->player_count - first_visible_source
    );

    if (visible_sources > OVERVIEW_PLAYER_LIMIT) {
        visible_sources = OVERVIEW_PLAYER_LIMIT;
    }
    for (visible_index = 0u; visible_index < visible_sources; visible_index++) {
        uint8_t source_player = (uint8_t)(first_visible_source + visible_index);
        draw_commander_source_row(
            game,
            target_player,
            source_player,
            player_rows[visible_index],
            source_player == selected_source
        );
    }
    draw_commander_controls(adjustment_step);
}

void ui_show_global_state(
    const GameState *game,
    uint8_t selected_field,
    uint8_t adjustment_step
) {
    prepare_screen();
    gotoxy(3u, 0u);
    printf("COMMANDER GBC");
    gotoxy(1u, 1u);
    printf("GLOBAL GAME STATE");
    draw_color_diagnostic();
    ui_refresh_global_state(game, selected_field, adjustment_step);
}

void ui_refresh_global_state(
    const GameState *game,
    uint8_t selected_field,
    uint8_t adjustment_step
) {
    char turn_text[UINT16_TEXT_BUFFER_SIZE];
    char storm_text[COUNTER_TEXT_BUFFER_SIZE];

    format_uint16_value(game->turn_number, turn_text);
    format_counter_value(game->storm_count, storm_text);

    draw_global_field_marker(
        3u,
        selected_field == GLOBAL_FIELD_ACTIVE_PLAYER
    );
    gotoxy(2u, 3u);
    printf("ACTIVE");
    gotoxy(14u, 3u);
    print_player_or_none(game, game->active_player);

    draw_global_field_marker(5u, selected_field == GLOBAL_FIELD_TURN_NUMBER);
    gotoxy(2u, 5u);
    printf("TURN");
    gotoxy(13u, 5u);
    printf("%s", turn_text);

    draw_global_field_marker(7u, selected_field == GLOBAL_FIELD_STORM_COUNT);
    gotoxy(2u, 7u);
    printf("STORM");
    gotoxy(15u, 7u);
    printf("%s", storm_text);

    draw_global_field_marker(9u, selected_field == GLOBAL_FIELD_MONARCH);
    gotoxy(2u, 9u);
    printf("MONARCH");
    gotoxy(14u, 9u);
    print_player_or_none(game, game->monarch_player);

    draw_global_field_marker(11u, selected_field == GLOBAL_FIELD_INITIATIVE);
    gotoxy(2u, 11u);
    printf("INITIATIVE");
    gotoxy(14u, 11u);
    print_player_or_none(game, game->initiative_player);

    draw_global_controls(adjustment_step);
}

void ui_draw_reset_prompt(void) {
    clear_help_area();
    gotoxy(1u, 12u);
    printf("RESET ALL PLAYERS?");
    gotoxy(3u, 14u);
    printf("A YES    B NO");
    gotoxy(1u, 15u);
    printf("SELECT NEW SETUP");
}

void ui_draw_elimination_prompt(const Player *player) {
    clear_help_area();
    gotoxy(2u, 12u);
    if (player->eliminated) {
        printf("RESTORE ");
    } else {
        printf("ELIMINATE ");
    }
    printf("%s", player->name);
    printf("?");
    gotoxy(3u, 14u);
    printf("A YES    B NO");
}
