#!/bin/bash

# Script para generar todos los formatos de imagen (img, qcow2, vmdk)
# Requiere qemu-img instalado localmente

echo "==========================================="
echo "Generando todos los formatos de imagen"
echo "==========================================="
echo ""

# Verificar que exista la imagen .img
if [ ! -f "Image/x64BareBonesImage.img" ]; then
    echo "ERROR: No existe Image/x64BareBonesImage.img"
    echo "Primero ejecuta: ./compile_in_container.sh"
    exit 1
fi

# Verificar que qemu-img esté instalado
if ! command -v qemu-img &> /dev/null; then
    echo "ERROR: qemu-img no está instalado"
    echo ""
    echo "Para instalarlo:"
    echo "  macOS:   brew install qemu"
    echo "  Ubuntu:  sudo apt-get install qemu-utils"
    exit 1
fi

echo "[*] Generando x64BareBonesImage.qcow2..."
qemu-img convert -f raw -O qcow2 Image/x64BareBonesImage.img Image/x64BareBonesImage.qcow2

echo "[*] Generando x64BareBonesImage.vmdk..."
qemu-img convert -f raw -O vmdk Image/x64BareBonesImage.img Image/x64BareBonesImage.vmdk

echo ""
echo "==========================================="
echo "Imágenes generadas exitosamente:"
echo "==========================================="
ls -lh Image/*.img Image/*.qcow2 Image/*.vmdk 2>/dev/null

echo ""
echo "Ahora podés ejecutar con: ./run.sh"
echo "(usará automáticamente .qcow2 si existe)"
