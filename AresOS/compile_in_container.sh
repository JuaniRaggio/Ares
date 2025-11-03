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
    echo "ERROR: Docker no está instalado o no está en el PATH"
    exit 1
fi

# Verificar que el contenedor exista
if ! docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "ERROR: El contenedor '$CONTAINER_NAME' no existe"
    echo ""
    echo "Contenedores disponibles:"
    docker ps -a --format "  - {{.Names}} ({{.Status}})"
    exit 1
fi

# Verificar que el contenedor esté corriendo
if ! docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "ADVERTENCIA: El contenedor '$CONTAINER_NAME' no está corriendo"
    echo "Intentando iniciarlo..."
    docker start "$CONTAINER_NAME" || {
        echo "ERROR: No se pudo iniciar el contenedor"
        exit 1
    }
    echo "Contenedor iniciado correctamente"
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
    echo "ERROR: La compilación falló"
    echo "=========================================="
    exit 1
fi
