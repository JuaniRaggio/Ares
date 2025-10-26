#!/bin/bash

# Script para configurar los git hooks del proyecto

echo "Configurando git hooks para el proyecto Ares..."

# Verificar que estamos en un repositorio git
if [ ! -d ".git" ]; then
    echo "Error: Este script debe ejecutarse desde la raíz del repositorio git"
    exit 1
fi

# Verificar que el directorio .githooks existe
if [ ! -d ".githooks" ]; then
    echo "Error: El directorio .githooks no existe"
    exit 1
fi

# Configurar git para usar el directorio .githooks
git config core.hooksPath .githooks

# Verificar que la configuración se aplicó correctamente
HOOKS_PATH=$(git config core.hooksPath)

if [ "$HOOKS_PATH" = ".githooks" ]; then
    echo "Configuración exitosa!"
    echo "Git ahora usará los hooks en el directorio .githooks/"
    echo ""
    echo "Hooks disponibles:"
    ls -1 .githooks/
    echo ""
    echo "Los hooks se ejecutarán automáticamente en las operaciones git correspondientes."
else
    echo "Error: No se pudo configurar el directorio de hooks"
    exit 1
fi
