#!/bin/bash

# Script para limpiar el proyecto dentro de un contenedor Docker
# Uso: ./clean_in_container.sh [NOMBRE_CONTENEDOR]

CONTAINER_NAME="${1:-ARES}"

echo "Limpiando proyecto en contenedor: $CONTAINER_NAME"
exec "$(dirname "$0")/compile_in_container.sh" "$CONTAINER_NAME" clean
