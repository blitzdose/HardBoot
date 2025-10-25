# Makefile

# Tools
CC := gcc
LD := ld
OBJCOPY := objcopy
SUBDIR := gnu-efi
BUILD := build

# Targets
TARGET := $(BUILD)/BOOTX64.EFI

all: $(TARGET)

# Ensure build directory exists
$(BUILD):
	mkdir -p $(BUILD)

# Step 1: force build of gnu-efi before everything else
.PHONY: gnu-efi-build
gnu-efi-build:
	@echo ">>> Building gnu-efi..."
	$(MAKE) -C $(SUBDIR)

# Step 2: compile main.c
$(BUILD)/main.o: main.c | $(BUILD) gnu-efi-build
	$(CC) -I$(SUBDIR)/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check \
		-fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c main.c -o $@

# Step 3: compile gnu-efi/lib/data.c
$(BUILD)/data.o: $(SUBDIR)/lib/data.c | $(BUILD) gnu-efi-build
	$(CC) -I$(SUBDIR)/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check \
		-fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c $(SUBDIR)/lib/data.c -o $@

# Step 4: link into shared EFI object
$(BUILD)/main.so: $(BUILD)/main.o $(BUILD)/data.o | $(BUILD) gnu-efi-build
	$(LD) -shared -Bsymbolic -L$(SUBDIR)/x86_64/lib -L$(SUBDIR)/x86_64/gnuefi \
		-T$(SUBDIR)/gnuefi/elf_x86_64_efi.lds \
		$(SUBDIR)/x86_64/gnuefi/crt0-efi-x86_64.o $(BUILD)/main.o $(BUILD)/data.o \
		-o $@ -lgnuefi -lefi

# Step 5: convert to EFI
$(BUILD)/BOOTX64.EFI: $(BUILD)/main.so | $(BUILD)
	$(OBJCOPY) -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym \
		-j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
		--target efi-app-x86_64 --subsystem=10 $< $@
	@echo ">>> Cleaning intermediate files..."
	rm -f $(BUILD)/main.o $(BUILD)/data.o $(BUILD)/main.so
	@echo ">>> Build complete: $@"

# Cleanup
clean:
	$(MAKE) -C $(SUBDIR) clean || true
	rm -rf $(BUILD)

.PHONY: all clean gnu-efi-build
