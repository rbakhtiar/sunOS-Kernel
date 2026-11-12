
#!/usr/bin/env bash

set -Eeuo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

echo "================================"
echo "       sunOS BUILD SYSTEM"
echo "================================"

if [[ "${1:-}" == "--clean" ]]; then
    make clean
fi

echo "[1/3] Checking dependencies..."
make check-tools

echo "[2/3] Compiling sunOS..."
make all

echo "[3/3] Checking outputs..."

test -f build/kernel.elf
test -f build/BOOTX64.EFI
test -f build/esp/EFI/BOOT/BOOTX64.EFI

echo "================================"
echo "       BUILD SUCCESSFUL"
echo "================================"
