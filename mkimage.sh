#!/bin/bash
#
# Create FAT12 filesystem image on Linux
#
volume_name="$1"
contents="$2"
outputdir="$3"

size_kbytes=$((2048 - 64))
#set -x

echo "Disk size = $size_kbytes kbytes"

# Create filesystem image
dd if=/dev/zero of=$outputdir/fat12.img bs=1024 count=$size_kbytes

# Attach the image as disk
disk=$(sudo losetup -f)
echo "$disk"
sudo losetup $disk $outputdir/fat12.img

# Format entire disk as one DOS partition
sudo sfdisk "$disk" << EOF
"$disk"p1 : type=4, bootable
EOF
sudo partx -a $disk

# Create FAT12 filesystem on the first partition
sudo mkfs.vfat -F 12 -n "$volume_name" "$disk"p1

# Mount the filesystem
rm -rf $outputdir/tmpdir
mkdir $outputdir/tmpdir
sudo mount -o uid=$(id -u) "$disk"p1 $outputdir/tmpdir
if [ $? != 0 ]; then
    echo "Cannot mount $disk"p1
    exit 1
fi

# Copy contents
tar --create --file $outputdir/filesys.tar -C "$contents" .
tar --extract --file $outputdir/filesys.tar -C $outputdir/tmpdir
sync

# Detach the image
sudo umount "$disk"p1
sudo losetup -d $disk

rm -rf $outputdir/tmpdir
rm -rf $outputdir/filesys.tar
