
# ==========================================================
# sunOS - x86_64 Hybrid Operating System
# Build system
# ==========================================================

CC      := gcc
LD      := ld
AS      := gcc
OBJCOPY := objcopy
QEMU    := qemu-system-x86_64

BUILD_DIR := build
KERNEL_DIR := kernel
BOOT_DIR := bootloader
ESP_DIR := $(BUILD_DIR)/esp

EFI_INCLUDE := /usr/include/efi
EFI_ARCH := /usr/include/efi/x86_64

EFI_CRT := $(shell find /usr/lib /usr/lib64 -name crt0-efi-x86_64.o 2>/dev/null | head -n 1)
EFI_LDS := $(shell find /usr/lib /usr/lib64 -name elf_x86_64_efi.lds 2>/dev/null | head -n 1)
EFI_LIBDIR := $(dir $(EFI_CRT))

OVMF_CODE ?= /usr/share/OVMF/OVMF_CODE.fd

# ----------------------------------------------------------
# Compiler flags
# ----------------------------------------------------------

KERNEL_CFLAGS := \
	-m64 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-pic \
	-fno-pie \
	-fno-builtin \
	-fno-asynchronous-unwind-tables \
	-mno-red-zone \
	-mcmodel=small \
	-O2 \
	-Wall \
	-Wextra \
	-Werror \
	-I$(KERNEL_DIR)/include

KERNEL_ASFLAGS := -m64 -ffreestanding -mno-red-zone

EFI_CFLAGS := \
	-I$(EFI_INCLUDE) \
	-I$(EFI_ARCH) \
	-fpic \
	-ffreestanding \
	-fno-stack-protector \
	-fshort-wchar \
	-mno-red-zone \
	-maccumulate-outgoing-args \
	-DEFI_FUNCTION_WRAPPER \
	-Wall \
	-Wextra

EFI_LDFLAGS := \
	-shared \
	-Bsymbolic \
	-L$(EFI_LIBDIR) \
	-T$(EFI_LDS)

# ----------------------------------------------------------
# Kernel objects
# ----------------------------------------------------------

KERNEL_OBJECTS := \
	$(BUILD_DIR)/kernel_entry.o \
	$(BUILD_DIR)/kernel_main.o \
	$(BUILD_DIR)/serial.o \
	$(BUILD_DIR)/pmm.o \
	$(BUILD_DIR)/paging.o \
	$(BUILD_DIR)/idt.o

.PHONY: all kernel bootloader esp run clean check-tools

all: kernel bootloader esp

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ----------------------------------------------------------
# Kernel assembly
# ----------------------------------------------------------

$(BUILD_DIR)/kernel_entry.o: $(KERNEL_DIR)/entry.S | $(BUILD_DIR)
	$(AS) $(KERNEL_ASFLAGS) -c $< -o $@

# ----------------------------------------------------------
# Kernel C files
# ----------------------------------------------------------

$(BUILD_DIR)/kernel_main.o: $(KERNEL_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/serial.o: $(KERNEL_DIR)/arch/x86_64/serial.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/pmm.o: $(KERNEL_DIR)/memory/pmm.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/paging.o: $(KERNEL_DIR)/arch/x86_64/paging.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/idt.o: $(KERNEL_DIR)/arch/x86_64/idt.c | $(BUILD_DIR)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

# ----------------------------------------------------------
# Kernel linking
# ----------------------------------------------------------

kernel: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(KERNEL_OBJECTS) $(KERNEL_DIR)/linker.ld
	$(LD) -nostdlib \
		-z max-page-size=0x1000 \
		-T $(KERNEL_DIR)/linker.ld \
		-o $@ \
		$(KERNEL_OBJECTS)

# ----------------------------------------------------------
# UEFI bootloader
# ----------------------------------------------------------

bootloader: $(BUILD_DIR)/BOOTX64.EFI

$(BUILD_DIR)/bootloader.o: $(BOOT_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(EFI_CFLAGS) -c $< -o $@

$(BUILD_DIR)/bootloader.so: $(BUILD_DIR)/bootloader.o
	$(LD) $(EFI_LDFLAGS) \
		-o $@ \
		$(EFI_CRT) \
		$(BUILD_DIR)/bootloader.o \
		-lefi -lgnuefi

$(BUILD_DIR)/BOOTX64.EFI: $(BUILD_DIR)/bootloader.so
	$(OBJCOPY) \
		-j .text \
		-j .sdata \
		-j .data \
		-j .dynamic \
		-j .dynsym \
		-j .rel \
		-j .rela \
		-j .reloc \
		--target=efi-app-x86_64 \
		$< $@

# ----------------------------------------------------------
# EFI system partition
# ----------------------------------------------------------

esp: kernel bootloader
	mkdir -p $(ESP_DIR)/EFI/BOOT
	cp $(BUILD_DIR)/BOOTX64.EFI $(ESP_DIR)/EFI/BOOT/BOOTX64.EFI
	cp $(BUILD_DIR)/kernel.elf $(ESP_DIR)/kernel.elf

# ----------------------------------------------------------
# Build dependency checks
# ----------------------------------------------------------

check-tools:
	@command -v gcc >/dev/null || (echo "Missing GCC"; exit 1)
	@command -v ld >/dev/null || (echo "Missing GNU ld"; exit 1)
	@command -v objcopy >/dev/null || (echo "Missing objcopy"; exit 1)
	@command -v make >/dev/null || (echo "Missing make"; exit 1)
	@test -f "$(EFI_CRT)" || (echo "Missing GNU-EFI CRT"; exit 1)
	@test -f "$(EFI_LDS)" || (echo "Missing GNU-EFI linker script"; exit 1)
	@test -d "$(EFI_INCLUDE)" || (echo "Missing GNU-EFI headers"; exit 1)
	@echo "All required build dependencies found."

# ----------------------------------------------------------
# QEMU
# ----------------------------------------------------------

run: esp
	@test -f "$(OVMF_CODE)" || (echo "OVMF firmware not found"; exit 1)
	$(QEMU) \
		-machine q35 \
		-m 512M \
		-bios $(OVMF_CODE) \
		-drive format=raw,file=fat:rw:$(ESP_DIR) \
		-serial stdio \
		-net none

# ----------------------------------------------------------
# Cleanup
# ----------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)
	@echo "sunOS build directory cleaned."
