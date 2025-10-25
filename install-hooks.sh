#!/bin/bash

# Script para instalar git hooks del proyecto
# Ejecutar desde la raíz del repositorio: ./install-hooks.sh

set -e

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}=== Instalando Git Hooks ===${NC}\n"

# Verificar que estamos en la raíz del repositorio
if [ ! -d ".git" ]; then
    echo -e "${RED}Error: Este script debe ejecutarse desde la raíz del repositorio Git${NC}"
    exit 1
fi

# Verificar que clang-format esté instalado
if ! command -v clang-format &> /dev/null; then
    echo -e "${YELLOW}Warning: clang-format no está instalado${NC}"
    echo "Para instalarlo:"
    echo "  - macOS:  brew install clang-format"
    echo "  - Linux:  apt install clang-format"
    echo "  - Arch:   pacman -S clang"
    echo ""
    read -p "¿Continuar de todas formas? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Crear directorio de hooks si no existe
mkdir -p .git/hooks

# Pre-commit hook para clang-format
echo -e "${YELLOW}Instalando pre-commit hook...${NC}"

cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash

# Pre-commit hook para formatear código automáticamente con clang-format
# Este hook formatea archivos .c y .h antes de commitear

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Buscar clang-format
CLANG_FORMAT=$(which clang-format 2>/dev/null)

if [ -z "$CLANG_FORMAT" ]; then
    echo -e "${RED}Error: clang-format no está instalado${NC}"
    echo "Instálalo con: brew install clang-format (macOS) o apt install clang-format (Linux)"
    exit 1
fi

# Buscar archivo .clang-format
CLANG_FORMAT_FILE=""
if [ -f "x64BareBones/.clang-format" ]; then
    CLANG_FORMAT_FILE="x64BareBones/.clang-format"
elif [ -f ".clang-format" ]; then
    CLANG_FORMAT_FILE=".clang-format"
fi

if [ -z "$CLANG_FORMAT_FILE" ]; then
    echo -e "${YELLOW}Warning: No se encontró .clang-format, usando estilo por defecto${NC}"
fi

# Obtener archivos staged .c y .h
FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')

if [ -z "$FILES" ]; then
    # No hay archivos C/H para formatear
    exit 0
fi

echo -e "${YELLOW}Formateando archivos con clang-format...${NC}"

FORMATTED=0
for FILE in $FILES; do
    if [ -f "$FILE" ]; then
        echo -e "  ${GREEN}✓${NC} $FILE"

        # Aplicar clang-format
        if [ -n "$CLANG_FORMAT_FILE" ]; then
            $CLANG_FORMAT -i -style=file:"$CLANG_FORMAT_FILE" "$FILE"
        else
            $CLANG_FORMAT -i "$FILE"
        fi

        # Re-stagear el archivo formateado
        git add "$FILE"
        FORMATTED=1
    fi
done

if [ $FORMATTED -eq 1 ]; then
    echo -e "${GREEN}Archivos formateados y re-staged exitosamente${NC}"
fi

exit 0
EOF

chmod +x .git/hooks/pre-commit

echo -e "${GREEN}✓ Pre-commit hook instalado${NC}"
echo ""
echo -e "${BLUE}=== Hooks instalados exitosamente ===${NC}"
echo ""
echo "Los siguientes hooks están activos:"
echo "  • pre-commit: Formatea código C/H con clang-format"
echo ""
echo -e "${YELLOW}Nota:${NC} Los hooks solo se ejecutan en tu copia local."
echo "Cada desarrollador debe ejecutar este script en su máquina."
