import json

from tools.generate_archetype_assets import MINIATURES, PROFILES
from tools.generate_commander_db import ARCHETYPES, ARCHETYPE_OVERRIDES, OUTPUT_JSON


def test_all_archetypes_have_distinct_miniatures_and_profiles() -> None:
    miniature_signatures = {"".join(rows) for rows in MINIATURES}
    profile_signatures = {
        tuple(pixel for row in draw_profile() for pixel in row)
        for draw_profile in PROFILES
    }

    assert len(MINIATURES) == 12
    assert len(PROFILES) == 12
    assert len(miniature_signatures) == 12
    assert len(profile_signatures) == 12


def test_profiles_use_four_tone_24_pixel_canvases() -> None:
    for draw_profile in PROFILES:
        image = draw_profile()

        assert len(image) == 24
        assert all(len(row) == 24 for row in image)
        assert {pixel for row in image for pixel in row} == {0, 1, 2, 3}


def test_archetype_tiles_fit_in_background_vram() -> None:
    first_custom_tile = 134
    miniature_tiles = len(MINIATURES)
    profile_tiles = len(PROFILES) * 3 * 3

    assert first_custom_tile + miniature_tiles + profile_tiles <= 256


def test_every_visual_archetype_has_a_clear_database_representative() -> None:
    payload = json.loads(OUTPUT_JSON.read_text(encoding="utf-8"))
    used_archetypes = {item["archetype"] for item in payload["commanders"]}

    assert used_archetypes == set(ARCHETYPES)
    assert set(ARCHETYPE_OVERRIDES.values()) == {"VOLTRON"}
    assert "wyleth, soul of steel" in ARCHETYPE_OVERRIDES
