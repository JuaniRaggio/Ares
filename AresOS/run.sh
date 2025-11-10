#!/bin/bash

# Seleccionar imagen a usar (preferir qcow2 si existe)
if [ -f Image/x64BareBonesImage.qcow2 ]; then
    IMAGE=Image/x64BareBonesImage.qcow2
    FORMAT=qcow2
else
    IMAGE=Image/x64BareBonesImage.img
    FORMAT=raw
fi

# Flag para debuggear
if [ "$1" == "-d" ]; then
    qemu-system-x86_64 -s -S -drive file="$IMAGE",format="$FORMAT" -m 512
else
    qemu-system-x86_64 -drive file="$IMAGE",format="$FORMAT" -m 512
fi
