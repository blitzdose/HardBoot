#!/bin/bash
#
# Create FAT12 filesystem image on MacOS
#
if [ $# != 2 ]; then
    echo "Usage: $0 volname dir"
    exit 0
fi

volume_name="$1"
contents="$2"
oem="MSWIN4.1"

size_kbytes=$((2048 - 64))
#set -x

echo "Disk size = $size_kbytes kbytes"

# Create filesystem image
dd if=/dev/zero of=filesys.img bs=1024 count=$size_kbytes

# Create dummy boot code
dd if=/dev/zero of=bootcode.bin bs=512 count=1

# Format entire disk as one DOS partition
fdisk -i -y -a dos -f bootcode.bin filesys.img

# Attach the image as disk
set $(hdiutil attach -nomount filesys.img)
disk=$1
echo "$disk"

# Create FAT12 filesystem on the first partition
newfs_msdos -F 12 -v "$volume_name" -O "$oem" "$disk"s1

# Mount the filesystem
set $(hdiutil mountvol "$disk"s1)
dir="$3"

# Copy contents
cp -a "$contents"/ "$dir"
sync

# Detach the image
hdiutil detach $disk

echo "Use the following command to write image to RP2040 Flash memory:"
echo
echo "    picotool load -n filesys.img -t bin -o 0x10010000"
