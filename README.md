# HardBoot
A hardware boot order switch

This project uses an RP2040 Zero and a switch to select which OS to boot.
It emulates an EFI partition and comes with its own EFI application
that will boot the appropriate OS.

![Screenshot](images/screenshot.png)

## Configuration

Configuration is pretty simple. 
1. Build the project or download the pre-built binaries
2. Flash them to your RP2040 Zero. Connect a switch between `GPIO15` and `GND`.
3. Reboot your system and head into the UEFI.
4. Change the boot order to always boot to the "HardBoot" partition first
5. Set up the other boot order options so that the second boot option is the 
OS you want to boot when the switch is off, and the third boot option is the OS
you want to boot when the switch is on.
6. Save, reboot, and see the magic happen

## Flasing
Put your RP2040 Zero in BOOTSEL mode by holding `BOOT` and pressing `RESET`. Then run the following commands.
After the second command, your Board should be detected as a mass storage device containing a `DATA.TXT` and `EFI/BOOT/BOOTX64.EFI` file.

```
picotool load -n fat12.img -t bin -o 0x10010000
picotool load -f -x waveshare_rp2040_zero.uf2
```

## Known issues
Make sure you are using the boot entries configured by your OS durin install (Usually "Windows Boot Manager", "Debain", etc.).
DO NOT use plain devices (Like "NVME0"), as these might not work. If you run into a situation where HardBoot shows some error, feel free
to open an issue.

## Building

To build the project, do the following steps. Currently, only Linux (Tested on Debian) is supported. Use WSL if you are on Windows.

1. Clone this repo, including its submodules

```
git clone https://github.com/blitzdose/HardBoot.git
cd HardBoot
git submodule update --init --recursive
```

2. Install necessary tools

```
sudo apt install make gcc cmake python3 build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib picotool dosfstools
```

3. Build

```
make
```
There will be an error shown at the end. Something like "Re-reading the partition table failed.: Invalid argument". Don't worry about it. It still works.

4. Upload

Either run `make upload` or the following commands manually to upload:

```
picotool load -n build/fat12.img -t bin -o 0x10010000
picotool load -f -x build/waveshare_rp2040_zero.uf2
```

## Credit
Most of the RP2040 code is from [https://github.com/sergev/usb-rom/tree/main](https://github.com/sergev/usb-rom/tree/main), 
I just added very few modifications to it.
