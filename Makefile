GBDK_HOME ?= $(CURDIR)/.tools/gbdk
LCC := $(GBDK_HOME)/bin/lcc

ROM_NAME := commander_gbc_one_player
ROM := build/$(ROM_NAME).gbc
SOURCES := \
	gbc_src/main.c \
	gbc_src/player.c \
	gbc_src/game_state.c \
	gbc_src/actions.c \
	gbc_src/rules.c \
	gbc_src/text_format.c \
	gbc_src/ui.c
CORE_SOURCES := \
	gbc_src/player.c \
	gbc_src/game_state.c \
	gbc_src/actions.c \
	gbc_src/rules.c \
	gbc_src/text_format.c
CORE_TEST := build/test_gbc_core
LCC_FLAGS := -msm83:gb -Wm-yC -Wm-yn"CMDGBC 1P"
HOST_CC ?= cc
HOST_CFLAGS := -std=c99 -Wall -Wextra -Werror -Igbc_src

.PHONY: all rom test-c verify setup-toolchain clean

all: test-c verify

rom: $(ROM)

$(ROM): $(SOURCES)
	@test -x "$(LCC)" || { echo "GBDK not found. Run: make setup-toolchain"; exit 1; }
	@mkdir -p build
	$(LCC) $(LCC_FLAGS) -o $@ $(SOURCES)

$(CORE_TEST): gbc_tests/test_one_player.c $(CORE_SOURCES)
	@mkdir -p build
	$(HOST_CC) $(HOST_CFLAGS) -o $@ gbc_tests/test_one_player.c $(CORE_SOURCES)

test-c: $(CORE_TEST)
	./$(CORE_TEST)

verify: test-c $(ROM)
	python3 tools/verify_rom.py $(ROM)

setup-toolchain:
	./tools/setup_gbdk.sh

clean:
	rm -rf build
