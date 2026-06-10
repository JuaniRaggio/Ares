#!/bin/bash
# Generate all image formats (qcow2, vmdk) from raw img
# Requires qemu-img installed locally

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Check that .img exists
if [ ! -f "Image/x64BareBonesImage.img" ]; then
    echo -e "${RED}Error: Image/x64BareBonesImage.img not found${NC}"
    echo "Run ./compile_in_container.sh first"
    exit 1
fi

# Check that qemu-img is installed
if ! command -v qemu-img &> /dev/null; then
    echo -e "${RED}Error: qemu-img not installed${NC}"
    echo ""
    echo "Install with:"
    echo "  macOS:   brew install qemu"
    echo "  Ubuntu:  sudo apt-get install qemu-utils"
    exit 1
fi

echo "Generating image formats..."

qemu-img convert -f raw -O qcow2 Image/x64BareBonesImage.img Image/x64BareBonesImage.qcow2 2>/dev/null
qemu-img convert -f raw -O vmdk Image/x64BareBonesImage.img Image/x64BareBonesImage.vmdk 2>/dev/null

echo -e "${GREEN}Done${NC}"
ls -lh Image/*.img Image/*.qcow2 Image/*.vmdk 2>/dev/null
echo ""
echo "Run with: ./run.sh (will use .qcow2 if available)"
