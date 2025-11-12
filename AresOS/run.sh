#!/bin/bash

# Check if qemu is available
if ! command -v qemu-system-x86_64 &> /dev/null; then
    echo "ERROR: qemu-system-x86_64 is not installed or not in PATH"
    echo "Please refer to Readme.txt for installation instructions"
    exit 1
fi

# Select image to use (prefer qcow2 if it exists)
if [ -f Image/x64BareBonesImage.qcow2 ]; then
    IMAGE=Image/x64BareBonesImage.qcow2
    FORMAT=qcow2
elif [ -f Image/x64BareBonesImage.img ]; then
    IMAGE=Image/x64BareBonesImage.img
    FORMAT=raw
else
    echo "ERROR: No bootable image found"
    echo "Expected files:"
    echo "  - Image/x64BareBonesImage.qcow2 (preferred)"
    echo "  - Image/x64BareBonesImage.img (fallback)"
    echo ""
    echo "Please compile the project first. Refer to Readme.txt for compilation instructions"
    exit 1
fi

echo "================================================"
echo "Running AresOS"
echo "Image: $IMAGE (format: $FORMAT)"
if [ "$1" == "-d" ]; then
    echo "Mode: Debug (waiting for GDB connection on port 1234)"
fi
echo "================================================"
echo ""

# Determine the best audio backend available
AUDIO_BACKEND="none"
if command -v pulseaudio &> /dev/null || pgrep -x pulseaudio > /dev/null; then
    AUDIO_BACKEND="pa"
elif command -v pactl &> /dev/null; then
    AUDIO_BACKEND="pa"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Try ALSA on Linux
    AUDIO_BACKEND="alsa"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    AUDIO_BACKEND="coreaudio"
fi

echo "Audio backend: $AUDIO_BACKEND"
echo ""

# Flag for debugging
if [ "$1" == "-d" ]; then
    qemu-system-x86_64 \
        -s -S \
        -drive file="$IMAGE",format="$FORMAT" \
        -m 512 \
        -audiodev $AUDIO_BACKEND,id=audio \
        -machine pcspk-audiodev=audio || {
        echo ""
        echo "ERROR: Failed to start QEMU in debug mode"
        echo "Please refer to Readme.txt for troubleshooting instructions"
        exit 1
    }
else
    qemu-system-x86_64 \
        -drive file="$IMAGE",format="$FORMAT" \
        -m 512 \
        -audiodev $AUDIO_BACKEND,id=audio \
        -machine pcspk-audiodev=audio || {
        echo ""
        echo "ERROR: Failed to start QEMU"
        echo "Please refer to Readme.txt for troubleshooting instructions"
        exit 1
    }
fi
