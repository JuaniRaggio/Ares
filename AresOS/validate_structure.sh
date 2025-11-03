#!/bin/bash

echo "==================================="
echo "Validando estructura del proyecto"
echo "==================================="

PROJECT_ROOT="/Users/juaniraggio/workspace/itbaworkspace/Ares/x64BareBones"
ERRORS=0

echo ""
echo "[1] Verificando que no existan headers duplicados..."
if [ -f "$PROJECT_ROOT/Kernel/drivers/video/fonts/font.h" ]; then
    echo "  ERROR: Header duplicado encontrado: Kernel/drivers/video/fonts/font.h"
    ERRORS=$((ERRORS + 1))
fi
if [ -f "$PROJECT_ROOT/Kernel/drivers/video/fonts/fontManager.h" ]; then
    echo "  ERROR: Header duplicado encontrado: Kernel/drivers/video/fonts/fontManager.h"
    ERRORS=$((ERRORS + 1))
fi
if [ -f "$PROJECT_ROOT/Kernel/drivers/video/fonts/font_ubuntu_mono.h" ]; then
    echo "  ERROR: Header duplicado encontrado: Kernel/drivers/video/fonts/font_ubuntu_mono.h"
    ERRORS=$((ERRORS + 1))
fi
if [ $ERRORS -eq 0 ]; then
    echo "  OK: No se encontraron headers duplicados"
fi

echo ""
echo "[2] Verificando archivos duplicados de drivers..."
if [ -f "$PROJECT_ROOT/Kernel/drivers/video/video.c" ]; then
    echo "  ERROR: Archivo duplicado encontrado: Kernel/drivers/video/video.c"
    echo "         (usar video_driver.c en su lugar)"
    ERRORS=$((ERRORS + 1))
fi
if [ -f "$PROJECT_ROOT/Kernel/drivers/video/video.h" ]; then
    echo "  ERROR: Header duplicado encontrado: Kernel/drivers/video/video.h"
    echo "         (usar include/drivers/video_driver.h en su lugar)"
    ERRORS=$((ERRORS + 1))
fi
if [ $ERRORS -eq 0 ]; then
    echo "  OK: No se encontraron archivos duplicados"
fi

echo ""
echo "[3] Verificando includes con paths relativos..."
RELATIVE_INCLUDES=$(grep -r '#include "\.\.\/' "$PROJECT_ROOT/Kernel" "$PROJECT_ROOT/Userland" 2>/dev/null || true)
if [ -n "$RELATIVE_INCLUDES" ]; then
    echo "  ERROR: Se encontraron includes con paths relativos:"
    echo "$RELATIVE_INCLUDES"
    ERRORS=$((ERRORS + 1))
else
    echo "  OK: No se encontraron includes con paths relativos"
fi

echo ""
echo "[4] Verificando que compile_flags.txt existan..."
if [ ! -f "$PROJECT_ROOT/Kernel/compile_flags.txt" ]; then
    echo "  ERROR: No existe Kernel/compile_flags.txt"
    ERRORS=$((ERRORS + 1))
else
    echo "  OK: Kernel/compile_flags.txt existe"
fi

if [ ! -f "$PROJECT_ROOT/Userland/UserCodeModule/compile_flags.txt" ]; then
    echo "  ERROR: No existe Userland/UserCodeModule/compile_flags.txt"
    ERRORS=$((ERRORS + 1))
else
    echo "  OK: Userland/UserCodeModule/compile_flags.txt existe"
fi

echo ""
echo "[5] Verificando que compile_commands.json exista..."
if [ ! -f "$PROJECT_ROOT/Kernel/compile_commands.json" ]; then
    echo "  ADVERTENCIA: No existe Kernel/compile_commands.json"
    echo "  Ejecuta: python3 Kernel/generate_compile_commands.py"
else
    echo "  OK: Kernel/compile_commands.json existe"
fi

echo ""
echo "[6] Verificando estructura de directorios..."
REQUIRED_DIRS=(
    "Kernel/include"
    "Kernel/include/arch/x86_64"
    "Kernel/include/core"
    "Kernel/include/drivers"
    "Kernel/arch/x86_64"
    "Kernel/drivers/video"
    "Kernel/drivers/keyboard"
    "Kernel/drivers/timer"
    "Kernel/core"
    "Kernel/lib"
    "Userland/UserCodeModule/include"
    "Userland/UserCodeModule/src"
)

for dir in "${REQUIRED_DIRS[@]}"; do
    if [ ! -d "$PROJECT_ROOT/$dir" ]; then
        echo "  ERROR: Directorio faltante: $dir"
        ERRORS=$((ERRORS + 1))
    fi
done

if [ $ERRORS -eq 0 ]; then
    echo "  OK: Todos los directorios requeridos existen"
fi

echo ""
echo "==================================="
if [ $ERRORS -eq 0 ]; then
    echo "VALIDACION EXITOSA"
    echo "==================================="
    exit 0
else
    echo "VALIDACION FALLIDA: $ERRORS errores encontrados"
    echo "==================================="
    exit 1
fi
