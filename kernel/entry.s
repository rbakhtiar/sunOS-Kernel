
.section .text.entry
.global _start
.type _start, @function

.extern kernel_main

_start:

    cli
    cld

    /*
     * RDI contains SunBootInfo pointer.
     * RSP is provided by the bootloader.
     */

    and $-16, %rsp

    call kernel_main

.halt:
    cli
    hlt
    jmp .halt

.size _start, . - _start

.section .note.GNU-stack,"",@progbits
