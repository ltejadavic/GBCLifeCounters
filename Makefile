GBDK_HOME ?= $(CURDIR)/.tools/gbdk
LCC := $(GBDK_HOME)/bin/lcc

ROM_NAME := commander_gbc_multiplayer
ROM := build/$(ROM_NAME).gbc
GBC_OBJDIR := build/gbc_obj
SOURCES := \
	gbc_src/main.c \
	gbc_src/archetype_assets.c \
	gbc_src/commander_db.c \
	gbc_src/player.c \
	gbc_src/game_state.c \
	gbc_src/actions.c \
	gbc_src/navigation.c \
	gbc_src/rules.c \
	gbc_src/sound.c \
	gbc_src/splash_assets.c \
	gbc_src/text_format.c \
	gbc_src/ui.c
GBC_OBJECTS := $(patsubst gbc_src/%.c,$(GBC_OBJDIR)/%.o,$(SOURCES))
GBC_HEADERS := $(wildcard gbc_src/*.h)
CORE_SOURCES := \
	gbc_src/player.c \
	gbc_src/game_state.c \
	gbc_src/actions.c \
	gbc_src/commander_db.c \
	gbc_src/navigation.c \
	gbc_src/rules.c \
	gbc_src/text_format.c
CORE_TEST := build/test_gbc_core
LCC_FLAGS := -msm83:gb -Wm-yC -Wm-yn"CMDGBC MVP" \
	-Wl-yt0x19 -Wm-yo4 -autobank -Wb-ext=.rel -Wl-j
HOST_CC ?= cc
HOST_CFLAGS := -std=c99 -Wall -Wextra -Werror -Igbc_src

.PHONY: all rom test-c verify setup-toolchain clean

all: test-c verify

rom: $(ROM)

$(ROM): $(GBC_OBJECTS)
	@test -x "$(LCC)" || { echo "GBDK not found. Run: make setup-toolchain"; exit 1; }
	@mkdir -p build
	$(LCC) $(LCC_FLAGS) -Igbc_src -o $@ $(GBC_OBJECTS)

$(GBC_OBJDIR)/%.o: gbc_src/%.c $(GBC_HEADERS)
	@mkdir -p $(GBC_OBJDIR)
	$(LCC) $(LCC_FLAGS) -Igbc_src -c -o $@ $<

$(CORE_TEST): gbc_tests/test_gbc_core.c $(CORE_SOURCES)
	@mkdir -p build
	$(HOST_CC) $(HOST_CFLAGS) -o $@ gbc_tests/test_gbc_core.c $(CORE_SOURCES)

test-c: $(CORE_TEST)
	./$(CORE_TEST)

verify: test-c $(ROM)
	python3 tools/verify_rom.py $(ROM)

setup-toolchain:
	./tools/setup_gbdk.sh

clean:
	rm -rf build
