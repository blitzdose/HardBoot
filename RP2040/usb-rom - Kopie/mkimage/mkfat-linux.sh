#!/bin/bash
#
# Create FAT12 filesystem image on Linux
#
if [ $# != 2 ]; then
    echo "Usage: $0 volname dir"
    exit 0
fi

volume_name="$1"
contents="$2"

size_kbytes=$((2048 - 64))
#set -x

echo "Disk size = $size_kbytes kbytes"

# Create filesystem image
dd if=/dev/zero of=filesys.img bs=1024 count=$size_kbytes

# Attach the image as disk
disk=$(losetup -f)
echo "$disk"
losetup $disk filesys.img

# Format entire disk as one DOS partition
sfdisk "$disk" << EOF
"$disk"p1 : type=4, bootable
EOF
sudo partx -a $disk

# Create FAT12 filesystem on the first partition
mkfs.vfat -F 12 -n "$volume_name" "$disk"p1

# Mount the filesystem
rm -rf tmpdir
mkdir tmpdir
sudo mount -o uid=$(id -u) "$disk"p1 tmpdir
if [ $? != 0 ]; then
    echo "Cannot mount $disk"p1
    exit 1
fi

# Copy contents
tar --create --file filesys.tar -C "$contents" .
tar --extract --file filesys.tar -C tmpdir
sync

# Detach the image
sudo umount "$disk"p1
losetup -d $disk

echo "Use the following command to write image to RP2040 Flash memory:"
echo
echo "    picotool load -n filesys.img -t bin -o 0x10010000"
