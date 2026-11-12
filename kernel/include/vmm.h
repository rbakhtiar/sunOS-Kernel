
#ifndef SUNOS_VMM_H
#define SUNOS_VMM_H

#include <stdint.h>
#include <stddef.h>

#define VMM_PAGE_SIZE 4096ULL

#define VMM_PRESENT       (1ULL << 0)
#define VMM_WRITABLE      (1ULL << 1)
#define VMM_USER          (1ULL << 2)
#define VMM_WRITE_THROUGH (1ULL << 3)
#define VMM_CACHE_DISABLE (1ULL << 4)
#define VMM_HUGE_PAGE     (1ULL << 7)
#define VMM_NO_EXECUTE   (1ULL << 63)

#define VMM_ADDRESS_MASK 0x000FFFFFFFFFF000ULL

typedef struct __attribute__((aligned(4096))) {
    uint64_t entries[512];
} VmmPageTable;

int vmm_map_page(
    VmmPageTable *pml4,
    uintptr_t virtual_address,
    uintptr_t physical_address,
    uint64_t flags
);

int vmm_unmap_page(
    VmmPageTable *pml4,
    uintptr_t virtual_address
);

void vmm_init(VmmPageTable *pml4);

#endif
