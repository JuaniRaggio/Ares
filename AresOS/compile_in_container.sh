#!/bin/bash
# Compile the project inside a Docker container
# Usage: ./compile_in_container.sh [CONTAINER_NAME] [TARGET]

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

CONTAINER_NAME="${1:-ARES}"
MAKE_TARGET="${2:-all}"
PROJECT_PATH="/root"

# Check if docker is available
if ! command -v docker &> /dev/null; then
    echo -e "${RED}Error: Docker not installed${NC}"
    exit 1
fi

# Check if the container exists
if ! docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo -e "${RED}Error: Container '$CONTAINER_NAME' not found${NC}"
    echo ""
    echo "Available containers:"
    docker ps -a --format "  - {{.Names}} ({{.Status}})"
    exit 1
fi

# Check if the container is running
if ! docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "Starting container $CONTAINER_NAME..."
    docker start "$CONTAINER_NAME" > /dev/null || {
        echo -e "${RED}Error: Could not start container${NC}"
        exit 1
    }
    sleep 1
fi

echo "Building in container $CONTAINER_NAME (target: $MAKE_TARGET)"

# Execute make inside the container
if docker exec -it "$CONTAINER_NAME" bash -c "cd $PROJECT_PATH && make $MAKE_TARGET"; then
    echo -e "${GREEN}Build successful${NC}"

    # Fix permissions of generated files
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Kernel/build" 2>/dev/null || true
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Kernel/bin" 2>/dev/null || true
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Image" 2>/dev/null || true
    sudo chown -R "$(id -u):$(id -g)" "$(pwd)/Userland" 2>/dev/null || true
    exit 0
else
    echo -e "${RED}Build failed${NC}"
    exit 1
fi
