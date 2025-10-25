# RP2040 as read-only USB Flash drive

This firmware for [RP2040-Zero board](https://www.waveshare.com/wiki/RP2040-Zero)
turns it into a USB mass storage device. Contents of Flash memory is exposed
to the USB host as read-only disk image. Disk size is 1984 kbytes.

Disk contents is prepared by user in one of two formats:

 * FAT12 - similar to traditional Flash disks
 * ISO9660 - much like CD-ROM or DVD-ROM

Other formats are possible by user's discretion. The RP2040 firmware is agnostic
to the filesystem format.

Longevity of the storage depends on a particular Flash chip installed on the board.
For Winbond W25Q16JV-IQ it's "more than 20-year data retention", according to the spec.

## Prerequisites
Before build, please make sure you have the following components installled:

 * CMake and ARM cross compiler
 * [Pico SDK](https://github.com/raspberrypi/pico-sdk)
 * [Picotool utility](https://github.com/raspberrypi/picotool)

## Build RP2040 firmware

    make

The resulting binary is located in file `build/usb_rom.uf2`.

## Program the firmware into RP2040 board

    make upload

Alternatively:

    picotool load -f -x build/usb_rom.uf2

After that, you will see a USB device like this:

        USB-ROM Drive:

          Version: 1.00
          Serial Number: E6625887D3264237
          Speed: Up to 12 Mb/s
          Manufacturer: github.com/sergev/usb-rom
          Location ID: 0x14500000 / 16
          Current Available (mA): 500
          Current Required (mA): 100
          Extra Operating Current (mA): 0
          Media:
            USB-ROM:
              Capacity: 2 MB (2,031,616 bytes)
              Removable Media: Yes
              BSD Name: disk5
              Logical Unit: 0
              Partition Map Type: MBR (Master Boot Record)
              S.M.A.R.T. status: Verified
              USB Interface: 0

To be able to mount it, you need to prepare and write a disk image to the Flash memory of the board.
The disk image is located in Flash memory starting from address `0x10010000` until `0x10200000`.

## Create disk contents

Use one of scripts in the `mkimage` directory:

 * mkfat-linux.sh - Create FAT12 filesystem on Linux
 * mkfat-macos.sh - Create FAT12 filesystem on MacOS
 * mkiso-linux.sh - Create ISO9660 filesystem on Linux
 * mkiso-macos.sh - Create ISO9660 filesystem on MacOS

Any other tools for creating filesystem images with your desired contents also are fine.
When disk image is ready, switch your RP2040 board into the boot mode again,
and write the image into it by command:

    picotool load -n filesys.img -t bin -o 0x10010000

After that, press the Reset button on your RP2040 board, and the disk should appear mounted on your computer.
