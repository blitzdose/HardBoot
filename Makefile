# Top-level Makefile for building EFI + RP2040 and collecting outputs

# Subdirectories
EFI_DIR := efi
RP2040_DIR := RP2040
OUT_DIR := build

# Files to collect
EFI_FILE := $(EFI_DIR)/build/BOOTX64.EFI
RP2040_FILES := $(RP2040_DIR)/build/*.uf2

.PHONY: all build efi rp2040 collect clean

# Default target
all: build

# 1️: Run the makefile in the efi directory
efi:
	@echo ">>> Building EFI..."
	$(MAKE) -C $(EFI_DIR)

# 2️: Run the makefile in the RP2040 directory
rp2040:
	@echo ">>> Building RP2040..."
	$(MAKE) -C $(RP2040_DIR)

# 3️: Create the output directory and copy results
collect:
	@echo ">>> Collecting build outputs..."
	mkdir -p $(OUT_DIR)
	cp -v $(EFI_FILE) $(OUT_DIR)/
	cp -v $(RP2040_FILES) $(OUT_DIR)/

mkimage:
	@echo ">>> Making FAT12 image..."
	mkdir $(OUT_DIR)/imagecontent
	printf '\001' > $(OUT_DIR)/imagecontent/DATA.TXT
	mkdir -p $(OUT_DIR)/imagecontent/EFI/BOOT/
	cp $(OUT_DIR)/BOOTX64.EFI $(OUT_DIR)/imagecontent/EFI/BOOT/
	./mkimage.sh HARDBOOT $(OUT_DIR)/imagecontent $(OUT_DIR)
	rm -rf $(OUT_DIR)/imagecontent


# Master build rule — runs both builds and collects outputs
build: efi rp2040 collect mkimage
	@echo ">>> All builds completed. Output is in $(OUT_DIR)/"
	
upload: 
	picotool load -n $(OUT_DIR)/fat12.img -t bin -o 0x10010000
	picotool load -f -x $(OUT_DIR)/*.uf2

# Clean everything
clean:
	@echo ">>> Cleaning EFI..."
	$(MAKE) -C $(EFI_DIR) clean || true
	@echo ">>> Cleaning RP2040..."
	$(MAKE) -C $(RP2040_DIR) clean || true
	@echo ">>> Removing top-level build folder..."
	rm -rf $(OUT_DIR)
