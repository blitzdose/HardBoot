#!/bin/bash
#
# Create ISO filesystem image on Linux
#
# Pre-requisites:
#   sudo apt install genisoimage xorriso
#
if [ $# != 2 ]; then
    echo "Usage: $0 volname dir"
    exit 0
fi

volume_name=$(echo "$1" | dd conv=ucase)
contents="$2"

#set -x
rm -f filesys.iso

#genisoimage -J -R -l -V "$volume_name" -hfs-volid "$volume_name" -o filesys.iso "$contents"

mkisofs -l -J -r -hfs -V "$volume_name" -hfs-volid "$volume_name" -o filesys.iso "$contents"

#xorrisofs -J -r -V "$volume_name" -o filesys.iso "$contents"

echo "Create filesystem image from the provided data"
mkisofs -o filesys.iso "$contents"
if [ $? != 0 ]; then
    echo "Failed to create filesystem"
    exit 1
fi

echo "Use the following command to write image to RP2040 Flash memory:"
echo
echo "    picotool load -n filesys.iso -t bin -o 0x10010000"
