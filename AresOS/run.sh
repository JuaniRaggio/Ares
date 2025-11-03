#!/bin/bash

# Seleccionar imagen a usar (preferir qcow2 si existe)
if [ -f Image/x64BareBonesImage.qcow2 ]; then
    IMAGE=Image/x64BareBonesImage.qcow2
else
    IMAGE=Image/x64BareBonesImage.img
fi

# Flag para debuggear
if [ "$1" == "-d" ]; then
    qemu-system-x86_64 -s -S -hda "$IMAGE" -m 512
else
    qemu-system-x86_64 -hda "$IMAGE" -m 512
fi
