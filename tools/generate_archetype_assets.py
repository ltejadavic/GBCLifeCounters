#!/usr/bin/env python3
"""Generate 2bpp GBC miniature and 24x24 archetype portrait tiles."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "gbc_src" / "archetype_assets.inc"
SIZE = 24


def canvas() -> list[list[int]]:
    return [[0 for _ in range(SIZE)] for _ in range(SIZE)]


def pixel(image: list[list[int]], x: int, y: int, color: int) -> None:
    if 0 <= x < SIZE and 0 <= y < SIZE:
        image[y][x] = color


def line(
    image: list[list[int]],
    x0: int,
    y0: int,
    x1: int,
    y1: int,
    color: int,
) -> None:
    dx = abs(x1 - x0)
    sx = 1 if x0 < x1 else -1
    dy = -abs(y1 - y0)
    sy = 1 if y0 < y1 else -1
    error = dx + dy
    while True:
        pixel(image, x0, y0, color)
        if x0 == x1 and y0 == y1:
            return
        twice_error = 2 * error
        if twice_error >= dy:
            error += dy
            x0 += sx
        if twice_error <= dx:
            error += dx
            y0 += sy


def rectangle(
    image: list[list[int]],
    x0: int,
    y0: int,
    x1: int,
    y1: int,
    color: int,
    filled: bool = False,
) -> None:
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            if filled or x in (x0, x1) or y in (y0, y1):
                pixel(image, x, y, color)


def circle(
    image: list[list[int]],
    center_x: int,
    center_y: int,
    radius: int,
    color: int,
    filled: bool = False,
) -> None:
    for y in range(center_y - radius, center_y + radius + 1):
        for x in range(center_x - radius, center_x + radius + 1):
            distance = (x - center_x) ** 2 + (y - center_y) ** 2
            if (filled and distance <= radius * radius) or (
                not filled and (radius - 1) ** 2 <= distance <= radius * radius
            ):
                pixel(image, x, y, color)


def diamond(
    image: list[list[int]],
    center_x: int,
    center_y: int,
    radius: int,
    color: int,
    filled: bool = False,
) -> None:
    for y in range(center_y - radius, center_y + radius + 1):
        width = radius - abs(y - center_y)
        for x in range(center_x - width, center_x + width + 1):
            if filled or x in (center_x - width, center_x + width):
                pixel(image, x, y, color)


def control() -> list[list[int]]:
    image = canvas()
    circle(image, 11, 8, 7, 2, True)
    for y in range(4, 16):
        span = min(8, y - 1)
        line(image, 11 - span, y, 11 + span, y, 3)
    diamond(image, 11, 9, 4, 1, True)
    line(image, 8, 9, 14, 9, 3)
    pixel(image, 11, 9, 2)
    line(image, 5, 18, 11, 14, 2)
    line(image, 17, 18, 11, 14, 2)
    line(image, 4, 20, 18, 20, 3)
    line(image, 20, 5, 20, 21, 3)
    circle(image, 20, 4, 2, 1, True)
    return image


def artifacts() -> list[list[int]]:
    image = canvas()
    circle(image, 12, 12, 8, 3, True)
    circle(image, 12, 12, 6, 1, True)
    circle(image, 12, 12, 4, 2, True)
    diamond(image, 12, 12, 2, 3, True)
    for x, y in ((11, 1), (11, 20), (1, 11), (20, 11)):
        rectangle(image, x, y, x + 2, y + 2, 3, True)
    for x, y in ((4, 4), (17, 4), (4, 17), (17, 17)):
        rectangle(image, x, y, x + 2, y + 2, 2, True)
    return image


def counters() -> list[list[int]]:
    image = canvas()
    for center_x, center_y, color in ((7, 17, 2), (16, 15, 1), (11, 7, 2)):
        diamond(image, center_x, center_y, 5, 3, True)
        diamond(image, center_x, center_y, 3, color, True)
        line(image, center_x - 2, center_y, center_x + 2, center_y, 3)
        line(image, center_x, center_y - 2, center_x, center_y + 2, 3)
    line(image, 19, 20, 19, 4, 3)
    line(image, 16, 7, 19, 4, 3)
    line(image, 22, 7, 19, 4, 3)
    return image


def enchantments() -> list[list[int]]:
    image = canvas()
    diamond(image, 12, 12, 10, 3, True)
    diamond(image, 12, 12, 8, 1, True)
    line(image, 12, 4, 7, 12, 2)
    line(image, 12, 4, 17, 12, 2)
    line(image, 7, 12, 12, 20, 2)
    line(image, 17, 12, 12, 20, 3)
    line(image, 12, 4, 12, 20, 3)
    for x, y in ((3, 4), (20, 5), (3, 18), (20, 18)):
        pixel(image, x, y, 2)
        pixel(image, x - 1, y, 1)
        pixel(image, x + 1, y, 1)
        pixel(image, x, y - 1, 1)
        pixel(image, x, y + 1, 1)
    return image


def graveyard() -> list[list[int]]:
    image = canvas()
    circle(image, 5, 5, 4, 1, True)
    circle(image, 7, 4, 4, 0, True)
    rectangle(image, 7, 8, 17, 21, 3, True)
    circle(image, 12, 8, 5, 3, True)
    rectangle(image, 9, 10, 15, 18, 2, True)
    circle(image, 12, 13, 3, 1, True)
    pixel(image, 11, 13, 3)
    pixel(image, 13, 13, 3)
    line(image, 10, 17, 14, 17, 3)
    line(image, 2, 21, 21, 21, 2)
    for x in (3, 19):
        line(image, x, 21, x + 1, 17, 1)
        line(image, x + 1, 18, x + 3, 16, 1)
    return image


def lands() -> list[list[int]]:
    image = canvas()
    circle(image, 19, 5, 3, 1, True)
    for offset in (-5, 0, 5):
        line(image, 19 + offset // 2, 1, 19 + offset, 0, 2)
    line(image, 1, 17, 8, 7, 3)
    line(image, 8, 7, 14, 15, 3)
    line(image, 10, 17, 16, 9, 2)
    line(image, 16, 9, 23, 18, 2)
    line(image, 6, 11, 8, 7, 1)
    line(image, 8, 7, 10, 12, 1)
    for y in range(17, 24):
        line(image, 0, y, 23, y, 2 if y % 2 else 1)
    line(image, 11, 16, 7, 23, 3)
    line(image, 12, 16, 15, 23, 3)
    return image


def lifegain() -> list[list[int]]:
    image = canvas()
    circle(image, 12, 3, 5, 2)
    circle(image, 8, 10, 5, 3, True)
    circle(image, 16, 10, 5, 3, True)
    for y in range(9, 20):
        width = max(0, 10 - abs(y - 10))
        line(image, 12 - width, y, 12 + width, y, 3)
    for y in range(10, 18):
        width = max(0, 7 - abs(y - 11))
        line(image, 12 - width, y, 12 + width, y, 1)
    line(image, 4, 9, 0, 6, 2)
    line(image, 4, 12, 0, 12, 2)
    line(image, 20, 9, 23, 6, 2)
    line(image, 20, 12, 23, 12, 2)
    return image


def sacrifice() -> list[list[int]]:
    image = canvas()
    diamond(image, 12, 8, 7, 3, True)
    diamond(image, 12, 9, 5, 2, True)
    diamond(image, 12, 10, 3, 1, True)
    line(image, 12, 3, 9, 11, 3)
    rectangle(image, 5, 16, 19, 19, 3, True)
    line(image, 3, 22, 21, 22, 3)
    line(image, 6, 19, 3, 22, 2)
    line(image, 18, 19, 21, 22, 2)
    for x in (4, 20):
        rectangle(image, x, 10, x + 1, 17, 2, True)
        pixel(image, x, 9, 1)
    return image


def spells() -> list[list[int]]:
    image = canvas()
    line(image, 2, 15, 10, 17, 3)
    line(image, 10, 17, 12, 21, 3)
    line(image, 22, 15, 14, 17, 3)
    line(image, 14, 17, 12, 21, 3)
    rectangle(image, 3, 14, 10, 20, 1, True)
    rectangle(image, 14, 14, 21, 20, 2, True)
    line(image, 12, 15, 12, 22, 3)
    for x0, y0, x1, y1 in ((12, 1, 12, 11), (7, 6, 17, 6), (8, 2, 16, 10), (16, 2, 8, 10)):
        line(image, x0, y0, x1, y1, 2)
    diamond(image, 12, 6, 2, 1, True)
    return image


def tokens() -> list[list[int]]:
    image = canvas()
    for center_x, center_y, color in ((5, 11, 1), (12, 8, 2), (19, 11, 1)):
        circle(image, center_x, center_y, 3, 3, True)
        circle(image, center_x, center_y, 1, color, True)
        line(image, center_x - 4, center_y + 9, center_x, center_y + 3, 3)
        line(image, center_x + 4, center_y + 9, center_x, center_y + 3, 3)
        line(image, center_x - 4, center_y + 9, center_x + 4, center_y + 9, 3)
        line(image, center_x, center_y + 3, center_x, center_y + 9, color)
    return image


def typal() -> list[list[int]]:
    image = canvas()
    line(image, 4, 3, 9, 7, 3)
    line(image, 20, 3, 15, 7, 3)
    line(image, 4, 3, 6, 14, 3)
    line(image, 20, 3, 18, 14, 3)
    diamond(image, 12, 12, 9, 3, True)
    diamond(image, 12, 13, 7, 2, True)
    diamond(image, 8, 11, 2, 1, True)
    diamond(image, 16, 11, 2, 1, True)
    pixel(image, 8, 11, 3)
    pixel(image, 16, 11, 3)
    diamond(image, 12, 17, 3, 1, True)
    line(image, 9, 21, 12, 18, 3)
    line(image, 15, 21, 12, 18, 3)
    return image


def voltron() -> list[list[int]]:
    image = canvas()
    line(image, 3, 21, 20, 4, 3)
    line(image, 5, 22, 22, 5, 2)
    diamond(image, 21, 4, 2, 1, True)
    circle(image, 10, 11, 8, 3, True)
    rectangle(image, 4, 10, 16, 19, 2, True)
    line(image, 4, 10, 10, 4, 1)
    line(image, 16, 10, 10, 4, 1)
    line(image, 10, 4, 10, 19, 3)
    rectangle(image, 6, 11, 9, 13, 1, True)
    rectangle(image, 11, 11, 14, 13, 1, True)
    line(image, 4, 19, 16, 19, 3)
    return image


MINIATURES = (
    ("..333...", ".32223..", "3211123.", "3313133.", ".32123..", "..333...", "...3....", "........"),
    ("..333...", ".32123..", "3311133.", "3213123.", "3311133.", ".32123..", "..333...", "........"),
    (".33.33..", "..313...", "3333333.", "..313...", "3333333.", "..313...", ".33.33..", "........"),
    ("...3....", "..323...", ".32123..", "3211123.", ".32123..", "..323...", "...3....", "........"),
    ("..333...", ".32223..", "3211123.", "3213133.", ".32123..", "..3.3...", ".3...3..", "........"),
    (".....1..", "...1.11.", "..2.1.1.", ".23222..", "3232233.", "2221222.", "3311133.", "........"),
    (".33.33..", "3212123.", "3211123.", ".32123..", "..323...", "...3....", "........", "........"),
    ("...1....", "..121...", "..232...", ".32323..", "...3....", ".33333..", "3333333.", "........"),
    ("...2....", ".2.2.2..", "..232...", "2231322.", "..232...", ".2.2.2..", "...2....", ".33.33.."),
    (".3...3..", "321.123.", "333.333.", ".3.3.3..", "3232323.", "3333333.", "3.3.3.3.", "........"),
    (".3...3..", "33...33.", "3211123.", "3213123.", ".32123..", "..323...", ".3...3..", "........"),
    ("......33", ".....32.", "..3332..", ".32123..", "3213123.", ".33333..", "...33...", "........"),
)

PROFILES = (
    control,
    artifacts,
    counters,
    enchantments,
    graveyard,
    lands,
    lifegain,
    sacrifice,
    spells,
    tokens,
    typal,
    voltron,
)


def tile_bytes(image: list[list[int]], tile_x: int, tile_y: int) -> list[int]:
    output: list[int] = []
    for row in range(8):
        low = 0
        high = 0
        for column in range(8):
            value = image[tile_y * 8 + row][tile_x * 8 + column]
            bit = 7 - column
            if value & 1:
                low |= 1 << bit
            if value & 2:
                high |= 1 << bit
        output.extend((low, high))
    return output


def miniature_image(rows: tuple[str, ...]) -> list[list[int]]:
    assert len(rows) == 8 and all(len(row) == 8 for row in rows)
    image = canvas()
    for y, row in enumerate(rows):
        for x, value in enumerate(row):
            image[y][x] = int(value) if value != "." else 0
    return image


def emit_array(name: str, values: list[int]) -> list[str]:
    lines = [f"const uint8_t {name}[{len(values)}u] = {{"]
    for offset in range(0, len(values), 16):
        chunk = ", ".join(f"0x{value:02x}u" for value in values[offset : offset + 16])
        lines.append(f"    {chunk},")
    lines.append("};")
    return lines


def main() -> None:
    mini_bytes: list[int] = []
    for rows in MINIATURES:
        mini_bytes.extend(tile_bytes(miniature_image(rows), 0, 0))

    profile_bytes: list[int] = []
    for draw_profile in PROFILES:
        image = draw_profile()
        for tile_y in range(3):
            for tile_x in range(3):
                profile_bytes.extend(tile_bytes(image, tile_x, tile_y))

    lines = ["/* Generated by tools/generate_archetype_assets.py. */"]
    lines.extend(emit_array("archetype_tiles", mini_bytes))
    lines.append("")
    lines.extend(emit_array("archetype_profile_tiles", profile_bytes))
    OUTPUT.write_text("\n".join(lines) + "\n", encoding="ascii")
    print("generated 12 miniatures and 12 24x24 archetype portraits")


if __name__ == "__main__":
    main()
