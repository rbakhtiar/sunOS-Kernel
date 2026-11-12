
#ifndef SUNOS_PMM_H
#define SUNOS_PMM_H

#include <stdint.h>
#include <stddef.h>

#define SUNOS_PAGE_SIZE 4096ULL

#define SUNOS_PMM_MAX_MEMORY (4ULL * 1024 * 1024 * 1024)
#define SUNOS_PMM_MAX_PAGES \
    (SUNOS_PMM_MAX_MEMORY / SUNOS_PAGE_SIZE)

typedef struct {
    uint64_t physical_start;
    uint64_t number_of_pages;
    uint32_t type;
} SunMemoryRegion;

typedef struct {
    SunMemoryRegion *regions;
    size_t region_count;
} SunMemoryMap;

void pmm_init(const SunMemoryMap *map);

uint64_t pmm_alloc_page(void);

int pmm_free_page(uint64_t address);

uint64_t pmm_free_pages(void);

#endif
