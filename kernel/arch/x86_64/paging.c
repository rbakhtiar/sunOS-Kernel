
#include "../../include/vmm.h"

static inline void invalidate_page(uintptr_t address) {
    __asm__ volatile(
        "invlpg (%0)"
        :
        : "r"(address)
        : "memory"
    );
}

static inline uint64_t read_cr3(void) {
    uint64_t value;

    __asm__ volatile(
        "mov %%cr3, %0"
        : "=r"(value)
    );

    return value;
}

static inline void write_cr3(uint64_t value) {
    __asm__ volatile(
        "mov %0, %%cr3"
        :
        : "r"(value)
        : "memory"
    );
}

void vmm_init(VmmPageTable *pml4) {
    if (!pml4) {
        return;
    }

    uint64_t address = (uint64_t)(uintptr_t)pml4;

    if ((address & 0xFFF) != 0 ||
        (address & ~VMM_ADDRESS_MASK) != 0) {
        return;
    }

    write_cr3(address);
}

int vmm_map_page(
    VmmPageTable *pml4,
    uintptr_t virtual_address,
    uintptr_t physical_address,
    uint64_t flags
) {
    (void)pml4;
    (void)virtual_address;
    (void)physical_address;
    (void)flags;

    /*
     * Page-table allocation and intermediate-level
     * table creation are not implemented yet.
     */
    return -1;
}

int vmm_unmap_page(
    VmmPageTable *pml4,
    uintptr_t virtual_address
) {
    (void)pml4;
    (void)virtual_address;

    /*
     * Mapping removal is not implemented yet.
     */
    return -1;
}
