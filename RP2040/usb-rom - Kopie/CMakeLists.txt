
cmake_minimum_required(VERSION 3.15)

# Include build functions from Pico SDK
set(PICO_PLATFORM rp2040)
include(../pico-sdk/pico_sdk_init.cmake)

project(usb_rom C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)

# Fix waveshare_rp2040_zero USB unrecognize after unplug and replug
# https://github.com/raspberrypi/pico-sdk/pull/1421
add_compile_definitions(PICO_FLASH_SPI_CLKDIV=4)

# Initialize the SDK
set(PICO_BOARD_HEADER_DIRS ${CMAKE_SOURCE_DIR}/boards)
if (DEFINED ENV{PICO_BOARD})
    set(PICO_BOARD $ENV{PICO_BOARD})
    message("Using PICO_BOARD from environment ('${PICO_BOARD}')")
else()
    if (NOT PICO_BOARD)
        set(PICO_BOARD "waveshare_rp2040_zero")
        pico_message("Defaulting PICO target board to ${PICO_BOARD} since not specified.")
    else()
        message("PICO target board is ${PICO_BOARD}.")
    endif()
endif()
pico_sdk_init()

add_executable(${PICO_BOARD}
    main.c
    msc_disk.c
    usb_descriptors.c
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(${PICO_BOARD} PUBLIC
    .
)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(${PICO_BOARD} PUBLIC
    pico_stdlib
    pico_unique_id
    tinyusb_device
    tinyusb_board
    hardware_pio
)

pico_add_extra_outputs(${PICO_BOARD})

# 'make upload' to program the binary
add_custom_target(upload
    picotool load -f -x ${PICO_BOARD}.uf2
    COMMENT "picotool load -f -x ${PICO_BOARD}.uf2"
    DEPENDS ${PICO_BOARD}
)
