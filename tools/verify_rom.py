"""Validate the essential cartridge header fields of a generated GB/GBC ROM."""

from pathlib import Path
import sys


NINTENDO_LOGO = bytes.fromhex(
    "CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D "
    "00 08 11 1F 88 89 00 0E DC CC 6E E6 DD DD D9 99 "
    "BB BB 67 63 6E 0E EC CC DD DC 99 9F BB B9 33 3E"
)


def validate_rom(path: Path) -> list[str]:
    data = path.read_bytes()
    errors: list[str] = []

    if len(data) < 32 * 1024 or len(data) % (16 * 1024) != 0:
        errors.append("ROM size must be at least 32 KiB and aligned to 16 KiB")
        return errors

    if data[0x104:0x134] != NINTENDO_LOGO:
        errors.append("Nintendo logo bytes are invalid")

    if data[0x143] != 0xC0:
        errors.append(f"expected CGB-only flag 0xC0, found 0x{data[0x143]:02X}")

    header_checksum = 0
    for value in data[0x134:0x14D]:
        header_checksum = (header_checksum - value - 1) & 0xFF
    if data[0x14D] != header_checksum:
        errors.append(
            f"header checksum mismatch: stored 0x{data[0x14D]:02X}, "
            f"calculated 0x{header_checksum:02X}"
        )

    stored_global_checksum = (data[0x14E] << 8) | data[0x14F]
    calculated_global_checksum = (
        sum(data) - data[0x14E] - data[0x14F]
    ) & 0xFFFF
    if stored_global_checksum != calculated_global_checksum:
        errors.append(
            f"global checksum mismatch: stored 0x{stored_global_checksum:04X}, "
            f"calculated 0x{calculated_global_checksum:04X}"
        )

    return errors


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: python3 tools/verify_rom.py PATH_TO_ROM", file=sys.stderr)
        return 2

    path = Path(sys.argv[1])
    if not path.is_file():
        print(f"ROM not found: {path}", file=sys.stderr)
        return 2

    errors = validate_rom(path)
    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        return 1

    data = path.read_bytes()
    title = data[0x134:0x143].rstrip(b"\0").decode("ascii", errors="replace")
    print(
        f"ROM verified: {path} | {len(data) // 1024} KiB | "
        f"title={title!r} | CGB flag=0x{data[0x143]:02X}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
