#!/usr/bin/env python3
"""Generate the compact GBC commander database from current public metadata."""

from __future__ import annotations

import json
import re
import ssl
import time
import unicodedata
import urllib.parse
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT_JSON = ROOT / "data" / "commanders.json"
OUTPUT_INC = ROOT / "gbc_src" / "commander_data.inc"
DECK_LIST_URL = "https://mtgjson.com/api/v5/DeckList.json"
DECK_URL = "https://mtgjson.com/api/v5/decks/{file_name}.json"
SCRYFALL_URL = "https://api.scryfall.com/cards/search"
USER_AGENT = "CommanderGbcLifeCounter/1.0 (local database generator)"
MAX_ROM_NAME = 28
POPULAR_LIMIT = 100

ARCHETYPES = (
    "CONTROL",
    "ARTIFACTS",
    "COUNTERS",
    "ENCHANTMENTS",
    "GRAVEYARD",
    "LANDS",
    "LIFEGAIN",
    "SACRIFICE",
    "SPELLS",
    "TOKENS",
    "TYPAL",
    "VOLTRON",
)

SSL_CONTEXT = ssl.create_default_context(
    cafile="/etc/ssl/cert.pem" if Path("/etc/ssl/cert.pem").exists() else None
)


def fetch_json(url: str) -> dict:
    request = urllib.request.Request(
        url,
        headers={"User-Agent": USER_AGENT, "Accept": "application/json"},
    )
    with urllib.request.urlopen(
        request,
        timeout=45,
        context=SSL_CONTEXT,
    ) as response:
        return json.load(response)


def ascii_text(value: str) -> str:
    normalized = unicodedata.normalize("NFKD", value)
    value = normalized.encode("ascii", "ignore").decode("ascii")
    value = re.sub(r"[^A-Za-z0-9 ',.!?&/+-]", "", value)
    return re.sub(r"\s+", " ", value).strip()


def classify(text: str, type_line: str) -> str:
    value = f"{text} {type_line}".lower()
    rules = (
        ("ARTIFACTS", ("artifact", "treasure token", "clue token")),
        ("ENCHANTMENTS", ("enchantment", "aura ", "constellation")),
        ("COUNTERS", ("+1/+1 counter", "proliferate", "kind of counter")),
        ("GRAVEYARD", ("graveyard", "reanimate", "mill ", "from your grave")),
        ("SACRIFICE", ("sacrifice", "whenever another creature dies")),
        ("LANDS", ("landfall", "land card", "lands you control")),
        ("LIFEGAIN", ("gain life", "lifelink", "life total")),
        ("SPELLS", ("instant or sorcery", "noncreature spell", "cast a spell")),
        ("TOKENS", ("create a ", "creature token", "tokens you control")),
        ("TYPAL", ("other creatures you control", "choose a creature type")),
        ("VOLTRON", ("commander damage", "equipped creature", "whenever this creature attacks")),
    )
    for archetype, needles in rules:
        if any(needle in value for needle in needles):
            return archetype
    return "CONTROL"


def add_card(entries: dict[str, dict], card: dict, source: str) -> None:
    name = ascii_text(card.get("asciiName") or card.get("name") or "")
    if not name:
        return
    key = name.casefold()
    item = entries.setdefault(
        key,
        {
            "name": name[:MAX_ROM_NAME],
            "full_name": name,
            "archetype": classify(
                card.get("text") or card.get("oracle_text") or "",
                card.get("type") or card.get("type_line") or "",
            ),
            "precon": False,
            "popular": False,
            "edhrec_rank": card.get("edhrecRank") or card.get("edhrec_rank"),
        },
    )
    item[source] = True
    rank = card.get("edhrecRank") or card.get("edhrec_rank")
    if rank and (not item["edhrec_rank"] or rank < item["edhrec_rank"]):
        item["edhrec_rank"] = rank


def load_precon_commanders(entries: dict[str, dict]) -> dict:
    deck_list = fetch_json(DECK_LIST_URL)
    decks = [
        deck
        for deck in deck_list["data"]
        if deck["type"] == "Commander Deck"
        and "Collector's Edition" not in deck["name"]
    ]
    for index, deck in enumerate(decks, 1):
        payload = fetch_json(DECK_URL.format(file_name=deck["fileName"]))
        for card in payload["data"].get("commander", []):
            add_card(entries, card, "precon")
        if index % 20 == 0:
            print(f"precons: {index}/{len(decks)}")
        time.sleep(0.055)
    return deck_list["meta"]


def load_popular_commanders(entries: dict[str, dict]) -> None:
    url = SCRYFALL_URL + "?" + urllib.parse.urlencode(
        {
            "q": "is:commander game:paper",
            "order": "edhrec",
            "dir": "asc",
            "unique": "cards",
        }
    )
    collected = 0
    while url and collected < POPULAR_LIMIT:
        payload = fetch_json(url)
        for card in payload["data"]:
            add_card(entries, card, "popular")
            collected += 1
            if collected >= POPULAR_LIMIT:
                break
        url = payload.get("next_page") if payload.get("has_more") else None
        time.sleep(0.1)


def write_outputs(entries: dict[str, dict], meta: dict) -> None:
    ordered = sorted(entries.values(), key=lambda item: item["name"].casefold())
    for index, item in enumerate(ordered):
        item["id"] = index
    OUTPUT_JSON.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_JSON.write_text(
        json.dumps(
            {
                "generated": meta,
                "popular_limit": POPULAR_LIMIT,
                "max_rom_name": MAX_ROM_NAME,
                "archetypes": ARCHETYPES,
                "commanders": ordered,
            },
            indent=2,
            ensure_ascii=True,
        )
        + "\n",
        encoding="utf-8",
    )
    lines = ["/* Generated by tools/generate_commander_db.py. */"]
    for item in ordered:
        escaped = item["name"].replace("\\", "\\\\").replace('"', '\\"')
        lines.append(
            f'    {{"{escaped}", ARCHETYPE_{item["archetype"]}}},'
        )
    OUTPUT_INC.write_text("\n".join(lines) + "\n", encoding="ascii")
    print(f"generated {len(ordered)} unique commanders")


def main() -> None:
    entries: dict[str, dict] = {}
    meta = load_precon_commanders(entries)
    load_popular_commanders(entries)
    write_outputs(entries, meta)


if __name__ == "__main__":
    main()
