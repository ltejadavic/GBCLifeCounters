GBDK_HOME ?= $(CURDIR)/.tools/gbdk
LCC := $(GBDK_HOME)/bin/lcc

ROM_NAME := commander_gbc_technical_test
ROM := build/$(ROM_NAME).gbc
SOURCES := gbc_src/main.c
LCC_FLAGS := -msm83:gb -Wm-yC -Wm-yn"CMDGBC TEST"

.PHONY: all rom verify setup-toolchain clean

all: verify

rom: $(ROM)

$(ROM): $(SOURCES)
	@test -x "$(LCC)" || { echo "GBDK not found. Run: make setup-toolchain"; exit 1; }
	@mkdir -p build
	$(LCC) $(LCC_FLAGS) -o $@ $(SOURCES)

verify: $(ROM)
	python3 tools/verify_rom.py $(ROM)

setup-toolchain:
	./tools/setup_gbdk.sh

clean:
	rm -rf build
