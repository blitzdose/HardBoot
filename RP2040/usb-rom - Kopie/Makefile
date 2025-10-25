#
# make
# make all      -- build everything
#
# make upload   -- load the program onto the device
#
# make clean    -- remove build files
#

#
# Select board from the pico-sdk list
#
#PICO_BOARD  = waveshare_rp2040_zero
PICO_BOARD  = vccgnd_yd_rp2040

all:    build
	$(MAKE) -C build $@

upload: build
	$(MAKE) -C build $@

clean:
	rm -rf build

.PHONY: all clean upload

# Configure for build
build:
	cmake -B $@ -D PICO_BOARD=$(PICO_BOARD) .
