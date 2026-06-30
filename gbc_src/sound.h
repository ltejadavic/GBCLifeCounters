#ifndef COMMANDER_GBC_SOUND_H
#define COMMANDER_GBC_SOUND_H

#include <stdint.h>

typedef enum SoundEffect {
    SOUND_EFFECT_NONE = 0u,
    SOUND_EFFECT_NAVIGATION = 1u,
    SOUND_EFFECT_CONFIRM = 2u,
    SOUND_EFFECT_CANCEL = 3u,
    SOUND_EFFECT_START = 4u,
    SOUND_EFFECT_VALUE_UP = 5u,
    SOUND_EFFECT_VALUE_DOWN = 6u,
    SOUND_EFFECT_LOSS = 7u,
    SOUND_EFFECT_ELIMINATION = 8u
} SoundEffect;

void sound_initialize(void);
void sound_start_intro_music(void);
void sound_stop_intro_music(void);
void sound_play_effect(SoundEffect effect);
void sound_update(void);

#endif
