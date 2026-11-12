
# sunOS

### A custom x86_64 operating system kernel

sunOS is an experimental operating system project built from scratch, using a UEFI boot process and a hybrid kernel architecture.

The goal is to develop a modular, extensible operating system with its own memory manager, scheduler, drivers, filesystem, and userspace environment.

---

## Project specifications

| Component | Specification |
|---|---|
| Architecture | x86_64 |
| Firmware | UEFI |
| Boot format | PE32+ EFI application |
| Kernel format | ELF64 |
| Kernel architecture | Hybrid |
| Main language | C |
| Low-level language | x86_64 Assembly |
| Build system | GNU Make |
| Emulator | QEMU |
| Firmware emulator | OVMF |

## Programming languages

The long-term project language plan:

- C — kernel core and drivers
- C++ — higher-level kernel services
- C# — future managed runtime
- Java — future virtual machine
- Ruby — scripting environment
- Python — system scripting
- HolyC — experimental language support
- Rust — memory-safe system components
- Assembly — processor-specific operations
- Lisp — experimental language runtime
- Perl — scripting and utilities

These languages are planned capabilities, not all currently implemented.

## Repository structure

```text
sunOS/
├── bootloader/
│   └── main.c
├── kernel/
│   ├── arch/
│   │   └── x86_64/
│   │       └── serial.c
│   ├── include/
│   │   └── sunos.h
│   ├── entry.S
│   ├── main.c
│   └── linker.ld
├── scripts/
├── build/
├── Makefile
└── README.md
```

## Current development status

- [x] Initial kernel structure
- [x] x86_64 kernel entry point
- [x] Serial output driver
- [x] Kernel linker script
- [x] Initial UEFI ELF loader foundation
- [ ] Complete UEFI memory-map handoff
- [ ] Kernel boot information structure
- [ ] Kernel handoff
- [ ] Physical memory manager
- [ ] Virtual memory manager
- [ ] Interrupt descriptor table
- [ ] Hardware timer
- [ ] Task scheduler
- [ ] Device drivers
- [ ] Filesystem
- [ ] Userspace

## Build requirements

A Linux development environment with:

- GCC
- GNU binutils
- GNU Make
- GNU-EFI
- QEMU
- OVMF

Build:

```bash
make clean
make
```

Run:

```bash
make run
```

The current bootloader is still under development. A successful build does not yet mean the kernel can boot.

## Project philosophy

Build the foundations first.

Every subsystem should be modular, documented, testable, and designed for future expansion.

**sunOS — Built from the ground up.**
