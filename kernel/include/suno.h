
#ifndef SUNOS_H
#define SUNOS_H

#include <stdint.h>
#include <stddef.h>

/* sunOS version */
#define SUNOS_VERSION_MAJOR 0
#define SUNOS_VERSION_MINOR 1
#define SUNOS_VERSION_PATCH 0

#define SUNOS_NAME "sunOS"

/* Bootloader identification */
#define SUNOS_BOOT_MAGIC 0x53554E4F535F424FULL

/* Architecture */
#define SUNOS_ARCH_X86_64 1

/* Memory */
#define SUNOS_PAGE_SIZE 4096
#define SUNOS_KERNEL_BASE 0x01000000ULL

/* Port I/O */
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile(
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile(
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

/* UEFI boot information */
typedef struct
{
    uint64_t magic;

    uint64_t framebuffer;
    uint64_t framebuffer_size;

    uint64_t width;
    uint64_t height;
    uint64_t pixels_per_scanline;

} SunBootInfo;

/* Kernel information */
typedef struct
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;

    const char *name;
    const char *architecture;

} SunKernelInfo;

/* Kernel entry */
void kernel_main(SunBootInfo *boot_info);

/* Serial subsystem */
void serial_init(void);
void serial_write(const char *text);
void serial_write_char(char character);
void serial_write_hex(uint64_t value);

/* CPU subsystem */
void cpu_halt(void);
void cpu_disable_interrupts(void);
void cpu_enable_interrupts(void);

/* Kernel initialization */
void kernel_initialize(SunBootInfo *boot_info);

#endif
