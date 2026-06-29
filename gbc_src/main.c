#include <gb/cgb.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdint.h>
#include <stdio.h>

#define STARTING_LIFE 40
#define DISPLAY_MIN_LIFE -999
#define DISPLAY_MAX_LIFE 999

static int16_t life = STARTING_LIFE;

static const palette_color_t background_palette[] = {
    RGB8(245, 239, 224),
    RGB8(174, 205, 100),
    RGB8(67, 117, 69),
    RGB8(19, 43, 35),
};

static void draw_static_screen(void) {
    set_bkg_palette(0, 1, background_palette);

    gotoxy(3, 1);
    printf("COMMANDER GBC");
    gotoxy(3, 3);
    printf("TECHNICAL TEST");

    gotoxy(7, 6);
    printf("LIFE");

    gotoxy(1, 10);
    printf("LEFT/RIGHT  -/+1");
    gotoxy(1, 11);
    printf("DOWN/UP     -/+5");
    gotoxy(1, 12);
    printf("B/A        -/+10");
    gotoxy(1, 13);
    printf("START       RESET");

    gotoxy(2, 16);
    printf("GBDK 4.5.0 / CGB");
}

static void draw_life(void) {
    gotoxy(8, 7);
    printf("%4d", life);
}

static void change_life(int16_t amount) {
    int16_t next_life = life + amount;

    if (next_life > DISPLAY_MAX_LIFE) {
        life = DISPLAY_MAX_LIFE;
    } else if (next_life < DISPLAY_MIN_LIFE) {
        life = DISPLAY_MIN_LIFE;
    } else {
        life = next_life;
    }
}

void main(void) {
    uint8_t previous_keys = 0;

    draw_static_screen();
    draw_life();
    SHOW_BKG;
    DISPLAY_ON;

    while (1) {
        uint8_t keys = joypad();
        uint8_t pressed = keys & (uint8_t)(~previous_keys);

        if (pressed & J_LEFT) {
            change_life(-1);
        } else if (pressed & J_RIGHT) {
            change_life(1);
        } else if (pressed & J_DOWN) {
            change_life(-5);
        } else if (pressed & J_UP) {
            change_life(5);
        } else if (pressed & J_B) {
            change_life(-10);
        } else if (pressed & J_A) {
            change_life(10);
        } else if (pressed & J_START) {
            life = STARTING_LIFE;
        } else {
            previous_keys = keys;
            vsync();
            continue;
        }

        draw_life();
        previous_keys = keys;
        vsync();
    }
}
