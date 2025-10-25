# Top-level Makefile for usb-rom with patch

# Variables
SUBDIR := usb-rom
PATCH := usb-rom.patch
PICO_BOARD := waveshare_rp2040_zero
#PICO_BOARD := vccgnd_yd_rp2040

.PHONY: all patch build clean

# Default target
all: build

# Step 2: build
build:
	@echo ">>> Building $(SUBDIR)..."
	@mkdir -p build
	cmake -B build -D PICO_BOARD=$(PICO_BOARD) $(SUBDIR)
	$(MAKE) -C build

# Clean
clean:
	@echo ">>> Cleaning build..."
	rm -rf build
