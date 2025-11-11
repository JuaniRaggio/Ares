#!/bin/bash

# Script para compilar el proyecto dentro de un contenedor Docker ya existente
# Uso: ./compile_in_container.sh [NOMBRE_CONTENEDOR] [TARGET]
#   NOMBRE_CONTENEDOR: nombre del contenedor (default: ARES)
#   TARGET: target de make (default: all)

set -e

CONTAINER_NAME="${1:-ARES}"
MAKE_TARGET="${2:-all}"
PROJECT_PATH="/root"

echo "=========================================="
echo "Compilando en contenedor: $CONTAINER_NAME"
echo "Target: $MAKE_TARGET"
echo "=========================================="
echo ""

# Verificar que docker esté disponible
if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker is not installed or not in PATH"
    echo "Please refer to Readme.txt for compilation prerequisites"
    exit 1
fi

# Verificar que el contenedor exista
if ! docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "ERROR: Container '$CONTAINER_NAME' does not exist"
    echo ""
    echo "Available containers:"
    docker ps -a --format "  - {{.Names}} ({{.Status}})"
    echo ""
    echo "Please refer to Readme.txt for instructions on how to set up the build environment"
    exit 1
fi

# Verificar que el contenedor esté corriendo
if ! docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "WARNING: Container '$CONTAINER_NAME' is not running"
    echo "Attempting to start it..."
    docker start "$CONTAINER_NAME" || {
        echo "ERROR: Could not start the container"
        echo "Please refer to Readme.txt for troubleshooting instructions"
        exit 1
    }
    echo "Container started successfully"
    sleep 1
fi

echo "[*] Ejecutando make $MAKE_TARGET dentro del contenedor..."
echo ""

# Ejecutar make dentro del contenedor
if docker exec -it "$CONTAINER_NAME" bash -c "cd $PROJECT_PATH && make $MAKE_TARGET"; then
    echo ""
    echo "=========================================="
    echo "Compilación exitosa"
    echo "=========================================="

    # Corregir permisos de los archivos generados
    echo ""
    echo "[*] Corrigiendo permisos de archivos generados..."
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Kernel/build" 2>/dev/null || true
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Kernel/bin" 2>/dev/null || true
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Image" 2>/dev/null || true
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Userland" 2>/dev/null || true

    exit 0
else
    echo ""
    echo "=========================================="
    echo "ERROR: Compilation failed"
    echo "=========================================="
    echo ""
    echo "Please refer to Readme.txt for detailed compilation instructions"
    echo "and make sure all prerequisites are properly installed."
    echo ""
    exit 1
fi
