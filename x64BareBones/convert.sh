#!/bin/bash

# Script para convertir la imagen .img a .qcow2 (ejecutar desde Mac, no desde Docker)

IMG_FILE="Image/x64BareBonesImage.img"
QCOW2_FILE="Image/x64BareBonesImage.qcow2"

if [ ! -f "$IMG_FILE" ]; then
    echo "Error: No se encontró $IMG_FILE"
    echo "Primero ejecuta 'make all' en Docker para generar la imagen."
    exit 1
fi

echo "Convirtiendo $IMG_FILE a $QCOW2_FILE..."
qemu-img convert -f raw -O qcow2 "$IMG_FILE" "$QCOW2_FILE"

if [ $? -eq 0 ]; then
    echo "Conversión exitosa!"
    ls -lh "$QCOW2_FILE"
else
    echo "Error en la conversión"
    exit 1
fi
