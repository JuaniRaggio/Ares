#!/bin/bash

# Script to compile the project inside an existing Docker container
# Usage: ./compile_in_container.sh [CONTAINER_NAME] [TARGET]
#   CONTAINER_NAME: container name (default: ARES)
#   TARGET: make target (default: all)

set -e

CONTAINER_NAME="${1:-ARES}"
MAKE_TARGET="${2:-all}"
PROJECT_PATH="/root"

echo "=========================================="
echo "Compiling in container: $CONTAINER_NAME"
echo "Target: $MAKE_TARGET"
echo "=========================================="
echo ""

# Check if docker is available
if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker is not installed or not in PATH"
    echo "Please refer to Readme.txt for compilation prerequisites"
    exit 1
fi

# Check if the container exists
if ! docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "ERROR: Container '$CONTAINER_NAME' does not exist"
    echo ""
    echo "Available containers:"
    docker ps -a --format "  - {{.Names}} ({{.Status}})"
    echo ""
    echo "Please refer to Readme.txt for instructions on how to set up the build environment"
    exit 1
fi

# Check if the container is running
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

echo "[*] Running make $MAKE_TARGET inside the container..."
echo ""

# Execute make inside the container
if docker exec -it "$CONTAINER_NAME" bash -c "cd $PROJECT_PATH && make $MAKE_TARGET"; then
    echo ""
    echo "=========================================="
    echo "Compilation successful"
    echo "=========================================="

    # Fix permissions of generated files
    echo ""
    echo "[*] Fixing permissions of generated files..."
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
