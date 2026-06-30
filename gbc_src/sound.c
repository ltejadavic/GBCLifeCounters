#include <gb/gb.h>
#include <stdint.h>

#include "sound.h"

#define NOTE_C3 1046u
#define NOTE_C4 1547u
#define NOTE_D4 1602u
#define NOTE_E4 1651u
#define NOTE_G4 1714u
#define NOTE_A4 1750u
#define NOTE_C5 1798u
#define NOTE_E5 1849u
#define NOTE_G5 1881u
#define INTRO_NOTE_FRAMES 18u
#define INTRO_REST_FRAMES 10u

static const uint16_t intro_notes[] = {
    NOTE_C5,
    NOTE_E5,
    NOTE_G5,
    NOTE_E5,
    NOTE_A4,
    NOTE_C5,
    NOTE_E5,
    0u,
};

static SoundEffect active_effect = SOUND_EFFECT_NONE;
static uint8_t effect_frame = 0u;
static uint8_t intro_active = 0u;
static uint8_t intro_note_index = 0u;
static uint8_t intro_frames_remaining = 0u;

static void enable_sound_hardware(void) {
    if (!(NR52_REG & AUDENA_ON)) {
        NR52_REG = AUDENA_ON;
    }
    NR50_REG = (uint8_t)(AUDVOL_VOL_LEFT(5u) | AUDVOL_VOL_RIGHT(5u));
    NR51_REG = (uint8_t)(
        AUDTERM_1_LEFT
        | AUDTERM_1_RIGHT
        | AUDTERM_2_LEFT
        | AUDTERM_2_RIGHT
        | AUDTERM_4_LEFT
        | AUDTERM_4_RIGHT
    );
}

static void trigger_pulse_1(
    uint16_t frequency,
    uint8_t initial_volume,
    uint8_t sweep
) {
    enable_sound_hardware();
    NR10_REG = sweep;
    NR11_REG = 0x80u;
    NR12_REG = (uint8_t)((initial_volume << 4u) | 0x02u);
    NR13_REG = (uint8_t)frequency;
    NR14_REG = (uint8_t)(0x80u | ((frequency >> 8u) & 0x07u));
}

static void silence_pulse_1(void) {
    NR12_REG = 0u;
    NR14_REG = 0x80u;
}

static void trigger_pulse_2(uint16_t frequency) {
    enable_sound_hardware();
    NR21_REG = 0x80u;
    NR22_REG = 0x30u;
    NR23_REG = (uint8_t)frequency;
    NR24_REG = (uint8_t)(0x80u | ((frequency >> 8u) & 0x07u));
}

static void silence_pulse_2(void) {
    NR22_REG = 0u;
    NR24_REG = 0x80u;
}

static void trigger_noise(uint8_t volume, uint8_t polynomial) {
    enable_sound_hardware();
    NR41_REG = 0x20u;
    NR42_REG = (uint8_t)((volume << 4u) | 0x03u);
    NR43_REG = polynomial;
    NR44_REG = 0x80u;
}

static void silence_noise(void) {
    NR42_REG = 0u;
    NR44_REG = 0x80u;
}

static void finish_effect(void) {
    active_effect = SOUND_EFFECT_NONE;
    effect_frame = 0u;
    silence_pulse_1();
    silence_noise();
    if (!intro_active) {
        NR52_REG = AUDENA_OFF;
    }
}

static void play_initial_effect_note(SoundEffect effect) {
    if (effect == SOUND_EFFECT_NAVIGATION) {
        trigger_noise(2u, 0x15u);
    } else if (effect == SOUND_EFFECT_CONFIRM) {
        trigger_pulse_1(NOTE_C5, 4u, 0u);
    } else if (effect == SOUND_EFFECT_CANCEL) {
        trigger_pulse_1(NOTE_D4, 4u, 0u);
    } else if (effect == SOUND_EFFECT_START) {
        trigger_pulse_1(NOTE_C5, 7u, 0x12u);
    } else if (effect == SOUND_EFFECT_VALUE_UP) {
        trigger_pulse_1(NOTE_C4, 5u, 0u);
    } else if (effect == SOUND_EFFECT_VALUE_DOWN) {
        trigger_pulse_1(NOTE_G4, 5u, 0u);
    } else if (effect == SOUND_EFFECT_LOSS) {
        trigger_pulse_1(NOTE_G4, 8u, 0x21u);
    } else if (effect == SOUND_EFFECT_ELIMINATION) {
        trigger_pulse_1(NOTE_C3, 7u, 0u);
        trigger_noise(6u, 0x36u);
    }
}

static void update_effect(void) {
    if (active_effect == SOUND_EFFECT_NONE) {
        return;
    }

    effect_frame++;
    if (active_effect == SOUND_EFFECT_NAVIGATION) {
        if (effect_frame >= 3u) {
            finish_effect();
        }
    } else if (
        (active_effect == SOUND_EFFECT_CONFIRM)
        || (active_effect == SOUND_EFFECT_CANCEL)
    ) {
        if (effect_frame >= 8u) {
            finish_effect();
        }
    } else if (active_effect == SOUND_EFFECT_START) {
        if (effect_frame == 5u) {
            trigger_pulse_1(NOTE_E5, 7u, 0u);
        } else if (effect_frame == 10u) {
            trigger_pulse_1(NOTE_G5, 7u, 0u);
        } else if (effect_frame >= 18u) {
            finish_effect();
        }
    } else if (active_effect == SOUND_EFFECT_VALUE_UP) {
        if (effect_frame == 4u) {
            trigger_pulse_1(NOTE_E4, 5u, 0u);
        } else if (effect_frame == 8u) {
            trigger_pulse_1(NOTE_G4, 5u, 0u);
        } else if (effect_frame >= 14u) {
            finish_effect();
        }
    } else if (active_effect == SOUND_EFFECT_VALUE_DOWN) {
        if (effect_frame == 4u) {
            trigger_pulse_1(NOTE_E4, 5u, 0u);
        } else if (effect_frame == 8u) {
            trigger_pulse_1(NOTE_C4, 5u, 0u);
        } else if (effect_frame >= 14u) {
            finish_effect();
        }
    } else if (active_effect == SOUND_EFFECT_LOSS) {
        if (effect_frame == 9u) {
            trigger_pulse_1(NOTE_E4, 8u, 0x21u);
        } else if (effect_frame == 18u) {
            trigger_pulse_1(NOTE_D4, 8u, 0x21u);
        } else if (effect_frame == 27u) {
            trigger_pulse_1(NOTE_C3, 8u, 0u);
        } else if (effect_frame == 36u) {
            trigger_noise(6u, 0x35u);
        } else if (effect_frame >= 50u) {
            finish_effect();
        }
    } else if (active_effect == SOUND_EFFECT_ELIMINATION) {
        if (effect_frame == 12u) {
            trigger_pulse_1(NOTE_C3, 5u, 0x21u);
            trigger_noise(4u, 0x45u);
        } else if (effect_frame >= 30u) {
            finish_effect();
        }
    }
}

static void start_next_intro_note(void) {
    uint16_t frequency = intro_notes[intro_note_index];

    intro_note_index++;
    if (intro_note_index >= (uint8_t)(sizeof(intro_notes) / sizeof(uint16_t))) {
        intro_note_index = 0u;
    }
    if (frequency == 0u) {
        silence_pulse_2();
        intro_frames_remaining = INTRO_REST_FRAMES;
    } else {
        trigger_pulse_2(frequency);
        intro_frames_remaining = INTRO_NOTE_FRAMES;
    }
}

static void update_intro_music(void) {
    if (!intro_active) {
        return;
    }
    if (intro_frames_remaining > 0u) {
        intro_frames_remaining--;
    }
    if (intro_frames_remaining == 0u) {
        start_next_intro_note();
    }
}

void sound_initialize(void) {
    active_effect = SOUND_EFFECT_NONE;
    effect_frame = 0u;
    intro_active = 0u;
    intro_note_index = 0u;
    intro_frames_remaining = 0u;
    NR52_REG = AUDENA_OFF;
}

void sound_start_intro_music(void) {
    intro_active = 1u;
    intro_note_index = 0u;
    intro_frames_remaining = 0u;
    start_next_intro_note();
}

void sound_stop_intro_music(void) {
    intro_active = 0u;
    intro_frames_remaining = 0u;
    silence_pulse_2();
    if (active_effect == SOUND_EFFECT_NONE) {
        NR52_REG = AUDENA_OFF;
    }
}

void sound_play_effect(SoundEffect effect) {
    if (effect == SOUND_EFFECT_NONE) {
        return;
    }
    active_effect = effect;
    effect_frame = 0u;
    play_initial_effect_note(effect);
}

void sound_update(void) {
    update_intro_music();
    update_effect();
}
