
#!/usr/bin/env bash

# ==================================================
# sunOS Build Script
# Architecture: x86_64
# Firmware: UEFI
# ==================================================

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$ROOT_DIR"

GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
RESET='\033[0m'

log() {
    echo -e "${CYAN}[sunOS]${RESET} $1"
}

success() {
    echo -e "${GREEN}[SUCCESS]${RESET} $1"
}

error() {
    echo -e "${RED}[ERROR]${RESET} $1" >&2
}

required_tools=(
    gcc
    ld
    objcopy
    make
)

log "Checking build tools..."

for tool in "${required_tools[@]}"; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        error "Missing required tool: $tool"
        exit 1
    fi
done

success "Build tools found."

if [[ ! -f "Makefile" ]]; then
    error "Makefile not found."
    exit 1
fi

if [[ "${1:-}" == "--clean" ]]; then
    log "Cleaning previous build..."
    make clean
fi

log "Starting sunOS build..."

if ! make all; then
    error "sunOS compilation failed."
    exit 1
fi

if [[ ! -f "build/kernel.elf" ]]; then
    error "Kernel ELF was not generated."
    exit 1
fi

if [[ ! -f "build/BOOTX64.EFI" ]]; then
    error "UEFI application was not generated."
    exit 1
fi

success "Kernel ELF generated."
success "UEFI application generated."

echo
echo "======================================"
echo "          sunOS BUILD REPORT"
echo "======================================"
echo "Architecture : x86_64"
echo "Firmware     : UEFI"
echo "Kernel       : build/kernel.elf"
echo "Bootloader   : build/BOOTX64.EFI"
echo "ESP          : build/esp/"
echo "======================================"
echo

success "Build process completed."
