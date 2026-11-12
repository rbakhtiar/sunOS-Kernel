
#!/usr/bin/env bash

# ==================================================
# sunOS QEMU Runner
# Architecture: x86_64
# Firmware: UEFI / OVMF
# ==================================================

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
ESP_DIR="$BUILD_DIR/esp"

QEMU="${QEMU:-qemu-system-x86_64}"
OVMF_CODE="${OVMF_CODE:-}"

log() {
    echo "[sunOS] $1"
}

fail() {
    echo "[ERROR] $1" >&2
    exit 1
}

# Check QEMU
command -v "$QEMU" >/dev/null 2>&1 ||
    fail "QEMU was not found."

# Check build output
[[ -f "$BUILD_DIR/BOOTX64.EFI" ]] ||
    fail "BOOTX64.EFI missing. Run scripts/build.sh first."

[[ -f "$BUILD_DIR/kernel.elf" ]] ||
    fail "kernel.elf missing. Run scripts/build.sh first."

[[ -d "$ESP_DIR/EFI/BOOT" ]] ||
    fail "EFI directory missing. Rebuild sunOS."

# Locate OVMF
if [[ -z "$OVMF_CODE" ]]; then
    for candidate in \
        /usr/share/OVMF/OVMF_CODE.fd \
        /usr/share/ovmf/OVMF_CODE.fd \
        /usr/share/edk2/x64/OVMF_CODE.fd
    do
        if [[ -f "$candidate" ]]; then
            OVMF_CODE="$candidate"
            break
        fi
    done
fi

[[ -n "$OVMF_CODE" && -f "$OVMF_CODE" ]] ||
    fail "OVMF firmware not found. Set OVMF_CODE to its firmware file."

log "Starting sunOS virtual machine..."
log "Firmware: $OVMF_CODE"
log "Memory: 512 MB"

exec "$QEMU" \
    -machine q35 \
    -m 512M \
    -bios "$OVMF_CODE" \
    -drive "format=raw,file=fat:rw:$ESP_DIR" \
    -serial stdio \
    -net none \
    -no-reboot
