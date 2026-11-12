
.section .text

.global _start
.type _start, @function

_start:

    /*
     * UEFI firmware calls efi_main.
     * GNU-EFI provides the EFI entry wrapper.
     * This file is reserved for architecture-specific
     * bootloader support.
     */

    ret

.size _start, . - _start

.section .note.GNU-stack,"",@progbits
