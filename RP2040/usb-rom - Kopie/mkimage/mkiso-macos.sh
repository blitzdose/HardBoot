#!/bin/bash
#
# Create ISO filesystem image on MacOS
#
if [ $# != 2 ]; then
    echo "Usage: $0 volname dir"
    exit 0
fi

volume_name="$1"
contents="$2"

#set -x
rm -f filesys.dmg filesys.cdr filesys.iso

echo "Create filesystem image from the provided data"
hdiutil create filesys.dmg -srcfolder "$contents" -volname "$volume_name"
if [ $? != 0 ]; then
    echo "Failed to create filesystem: probably too much data"
    exit 1
fi

echo "Convert image to CDR type"
hdiutil convert filesys.dmg -format UDTO -o filesys.cdr

echo "Generate ISO image"
hdiutil makehybrid -iso -joliet -o filesys.iso filesys.cdr

echo "Use the following command to write image to RP2040 Flash memory:"
echo
echo "    picotool load -n filesys.iso -t bin -o 0x10010000"
