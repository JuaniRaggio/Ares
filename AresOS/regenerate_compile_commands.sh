#!/bin/bash
# Regenerate compile_commands.json with correct paths for local IDE

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

CONTAINER_NAME="${1:-ARES}"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Regenerating compile_commands.json (container: $CONTAINER_NAME)"

# 1. Generate in Docker
docker exec "$CONTAINER_NAME" bash -c "cd /root && make compile_commands" 2>/dev/null || {
    echo -e "${RED}Error: Could not regenerate in Docker${NC}"
    echo "Check that container '$CONTAINER_NAME' is running"
    exit 1
}

# 2. Copy from container
docker cp "$CONTAINER_NAME:/root/compile_commands.json" "$PROJECT_ROOT/compile_commands.json" 2>/dev/null

# 3. Convert Docker paths to local paths and make include paths absolute
TMPFILE=$(mktemp)
sed \
  -e "s|/root/Kernel|${PROJECT_ROOT}/Kernel|g" \
  -e "s|/root/Userland|${PROJECT_ROOT}/Userland|g" \
  -e "s|/root/Common|${PROJECT_ROOT}/Common|g" \
  -e "s|\"directory\": \"/root\"|\"directory\": \"${PROJECT_ROOT}\"|g" \
  -e "s| -Iinclude| -I${PROJECT_ROOT}/Kernel/include|g" \
  -e "s| -Iinclude/lib| -I${PROJECT_ROOT}/Kernel/include/lib|g" \
  -e "s| -Iinclude/arch/x86_64| -I${PROJECT_ROOT}/Kernel/include/arch/x86_64|g" \
  -e "s| -Iinclude/core| -I${PROJECT_ROOT}/Kernel/include/core|g" \
  -e "s| -Iinclude/drivers| -I${PROJECT_ROOT}/Kernel/include/drivers|g" \
  -e "s| -Idrivers/video| -I${PROJECT_ROOT}/Kernel/drivers/video|g" \
  -e "s| -I\.\./Common/include| -I${PROJECT_ROOT}/Common/include|g" \
  -e "s| -Ilibc/include| -I${PROJECT_ROOT}/Userland/UserCodeModule/libc/include|g" \
  -e "s| -I\.\./\.\./Common/include| -I${PROJECT_ROOT}/Common/include|g" \
  -e "s| -Itests| -I${PROJECT_ROOT}/Userland/UserCodeModule/tests|g" \
  "${PROJECT_ROOT}/compile_commands.json" > "$TMPFILE"
mv "$TMPFILE" "${PROJECT_ROOT}/compile_commands.json"

# 4. Validate JSON
if python3 -m json.tool "${PROJECT_ROOT}/compile_commands.json" > /dev/null 2>&1; then
    NUM_FILES=$(python3 -c "import json; print(len(json.load(open('${PROJECT_ROOT}/compile_commands.json'))))")
    echo -e "${GREEN}Done:${NC} $NUM_FILES files processed"
    echo "Restart your IDE/LSP to apply changes"
else
    echo -e "${RED}Error: Invalid JSON${NC}"
    exit 1
fi
