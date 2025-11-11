#!/bin/bash

# Script to clean the project inside a Docker container
# Usage: ./clean_in_container.sh [CONTAINER_NAME]

CONTAINER_NAME="${1:-ARES}"

echo "Cleaning project in container: $CONTAINER_NAME"
exec "$(dirname "$0")/compile_in_container.sh" "$CONTAINER_NAME" clean
