
#include "sunos.h"

static const SunKernelInfo kernel_info =
{
    .major = SUNOS_VERSION_MAJOR,
    .minor = SUNOS_VERSION_MINOR,
    .patch = SUNOS_VERSION_PATCH,

    .name = SUNOS_NAME,
    .architecture = "x86_64"
};

void cpu_disable_interrupts(void)
{
    __asm__ volatile("cli");
}

void cpu_enable_interrupts(void)
{
    __asm__ volatile("sti");
}

void cpu_halt(void)
{
    cpu_disable_interrupts();

    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

void kernel_initialize(SunBootInfo *boot_info)
{
    serial_init();

    serial_write("\n");
    serial_write("====================================\n");
    serial_write("          sunOS OPERATING SYSTEM\n");
    serial_write("====================================\n");

    serial_write("[KERNEL] Initializing sunOS...\n");

    serial_write("[VERSION] sunOS ");

    serial_write_hex(kernel_info.major);
    serial_write(".");
    serial_write_hex(kernel_info.minor);
    serial_write(".");
    serial_write_hex(kernel_info.patch);

    serial_write("\n");

    serial_write("[ARCH] ");
    serial_write(kernel_info.architecture);
    serial_write("\n");

    if (boot_info == 0 ||
        boot_info->magic != SUNOS_BOOT_MAGIC)
    {
        serial_write("[ERROR] Invalid boot information.\n");
        cpu_halt();
    }

    serial_write("[BOOT] UEFI boot information received.\n");

    serial_write("[VIDEO] Framebuffer address: ");
    serial_write_hex(boot_info->framebuffer);
    serial_write("\n");

    serial_write("[VIDEO] Resolution: ");

    serial_write_hex(boot_info->width);
    serial_write(" x ");
    serial_write_hex(boot_info->height);

    serial_write("\n");

    serial_write("[KERNEL] Initialization complete.\n");

    serial_write("====================================\n");

    cpu_halt();
}

void kernel_main(SunBootInfo *boot_info)
{
    kernel_initialize(boot_info);
}
