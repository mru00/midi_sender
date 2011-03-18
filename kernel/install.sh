#!/bin/sh
# Installer for the kqemu module
set +e

# Find module name
if [ -f kqemu.ko ] ; then
   module=kqemu.ko
else
   module=kqemu.o
fi

# Find kernel install path
kernel_path="/lib/modules/`uname -r`"

mkdir -p "$kernel_path/misc"
cp "$module" "$kernel_path/misc"

/sbin/depmod -a

# Create the kqemu device. No special priviledge is needed to use kqemu.
device="/dev/kqemu"
rm -f $device
mknod $device c 250 0
chmod 666 $device
