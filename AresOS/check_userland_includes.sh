#!/bin/bash

# Script para verificar que los includes de Userland sean correctos

echo "Verificando includes de Userland..."
echo ""

ERRORS=0

# Verificar includes incorrectos de drivers
INCORRECT_VIDEO=$(grep -r '#include <video_driver.h>' /Users/juaniraggio/workspace/itbaworkspace/Ares/x64BareBones/Userland 2>/dev/null || true)
INCORRECT_KEYBOARD=$(grep -r '#include <keyboard_driver.h>' /Users/juaniraggio/workspace/itbaworkspace/Ares/x64BareBones/Userland 2>/dev/null || true)

if [ -n "$INCORRECT_VIDEO" ]; then
    echo "ERROR: Include incorrecto encontrado:"
    echo "$INCORRECT_VIDEO"
    echo "Debe ser: #include <drivers/video_driver.h>"
    echo ""
    ERRORS=$((ERRORS + 1))
fi

if [ -n "$INCORRECT_KEYBOARD" ]; then
    echo "ERROR: Include incorrecto encontrado:"
    echo "$INCORRECT_KEYBOARD"
    echo "Debe ser: #include <drivers/keyboard_driver.h>"
    echo ""
    ERRORS=$((ERRORS + 1))
fi

if [ $ERRORS -eq 0 ]; then
    echo "OK: Todos los includes son correctos"
    exit 0
else
    echo "ERRORES: $ERRORS problemas encontrados"
    exit 1
fi
