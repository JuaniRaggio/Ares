#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Check if qemu is available
if ! command -v qemu-system-x86_64 &> /dev/null; then
    echo -e "${RED}Error: qemu-system-x86_64 not installed${NC}"
    exit 1
fi

# Always boot the raw image: it is the direct output of the build, so it
# can never be staler than the last compile
IMAGE=Image/x64BareBonesImage.img
FORMAT=raw
if [ ! -f "$IMAGE" ]; then
    echo -e "${RED}Error: No bootable image found${NC}"
    echo "Run ./compile_in_container.sh first"
    exit 1
fi

# Determine audio backend
AUDIO_BACKEND="none"
if command -v pulseaudio &> /dev/null || pgrep -x pulseaudio > /dev/null; then
    AUDIO_BACKEND="pa"
elif command -v pactl &> /dev/null; then
    AUDIO_BACKEND="pa"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    AUDIO_BACKEND="alsa"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    AUDIO_BACKEND="coreaudio"
fi

echo "Running AresOS ($FORMAT)"
[ "$1" == "-d" ] && echo "Debug mode: waiting for GDB on port 1234"

# Run QEMU
if [ "$1" == "-d" ]; then
    qemu-system-x86_64 \
        -s -S \
        -drive file="$IMAGE",format="$FORMAT" \
        -m 512 \
        -audiodev $AUDIO_BACKEND,id=audio \
        -machine pcspk-audiodev=audio 2>/dev/null || {
        echo -e "${RED}Error: Failed to start QEMU${NC}"
        exit 1
    }
else
    qemu-system-x86_64 \
        -drive file="$IMAGE",format="$FORMAT" \
        -m 512 \
        -audiodev $AUDIO_BACKEND,id=audio \
        -machine pcspk-audiodev=audio 2>/dev/null || {
        echo -e "${RED}Error: Failed to start QEMU${NC}"
        exit 1
    }
fi
