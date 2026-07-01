#include <gb/gb.h>

#include "archetype_assets.h"

#pragma bank 3

#include "archetype_assets.inc"

void archetype_assets_load(void) BANKED {
    set_bkg_data(
        ARCHETYPE_TILE_FIRST,
        ARCHETYPE_TILE_COUNT,
        archetype_tiles
    );
    set_bkg_data(
        ARCHETYPE_PROFILE_TILE_FIRST,
        ARCHETYPE_PROFILE_TILE_COUNT,
        archetype_profile_tiles
    );
}
