#!/bin/bash

# fflush
clear
# Clean the build environment
make clean

# Build the project
make

# Run QEMU in the background
(make qemu &)

# Wait for 10 seconds
sleep 10

# Send Ctrl-A X to QEMU to exit
# Ctrl-A is ASCII code 1, X is ASCII code 24
echo -e '\001x' >/dev/tty

echo "QEMU exited after 10 seconds"
pkill qemu
