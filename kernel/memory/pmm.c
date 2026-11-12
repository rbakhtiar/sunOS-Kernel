
#include "../include/pmm.h"

#define BITMAP_SIZE (SUNOS_PMM_MAX_PAGES / 8)

#define MEMORY_USABLE SUNOS_PMM_USABLE_MEMORY

static uint8_t bitmap[BITMAP_SIZE];

static uint64_t free_count = 0;
static uint64_t total_count = SUNOS_PMM_MAX_PAGES;

/* --------------------------------------------------
   Bitmap operations
   -------------------------------------------------- */

static int page_used(uint64_t page) {
    return (bitmap[page / 8] >> (page % 8)) & 1;
}

static void set_page(uint64_t page) {
    bitmap[page / 8] |= (uint8_t)(1U << (page % 8));
}

static void clear_page(uint64_t page) {
    bitmap[page / 8] &= (uint8_t)~(1U << (page % 8));
}

/* --------------------------------------------------
   Physical Memory Manager Initialization
   -------------------------------------------------- */

void pmm_init(const SunMemoryMap *map) {
    if (map == NULL || map->regions == NULL) {
        return;
    }

    /* Reserve all physical pages initially. */
    for (uint64_t i = 0; i < total_count; i++) {
        set_page(i);
    }

    free_count = 0;

    for (size_t i = 0; i < map->region_count; i++) {
        const SunMemoryRegion *region = &map->regions[i];

        if (region->type != MEMORY_USABLE) {
            continue;
        }

        uint64_t start = region->physical_start;
        uint64_t pages = region->number_of_pages;

        if (start >= SUNOS_PMM_MAX_MEMORY || pages == 0) {
            continue;
        }

        uint64_t max_pages =
            (SUNOS_PMM_MAX_MEMORY - start) / SUNOS_PAGE_SIZE;

        if (pages > max_pages) {
            pages = max_pages;
        }

        uint64_t end = start + pages * SUNOS_PAGE_SIZE;

        /* Only release fully contained aligned pages. */
        uint64_t first_page =
            (start + SUNOS_PAGE_SIZE - 1) / SUNOS_PAGE_SIZE;

        uint64_t last_page = end / SUNOS_PAGE_SIZE;

        for (uint64_t page = first_page;
             page < last_page;
             page++) {

            if (page >= total_count) {
                break;
            }

            if (page_used(page)) {
                clear_page(page);
                free_count++;
            }
        }
    }

    /* Never allocate physical address zero. */
    if (!page_used(0)) {
        set_page(0);
        free_count--;
    }
}

/* --------------------------------------------------
   Allocate one physical page
   -------------------------------------------------- */

uint64_t pmm_alloc_page(void) {
    for (uint64_t page = 1; page < total_count; page++) {
        if (!page_used(page)) {
            set_page(page);

            if (free_count > 0) {
                free_count--;
            }

            return page * SUNOS_PAGE_SIZE;
        }
    }

    return 0;
}

/* --------------------------------------------------
   Release one physical page
   -------------------------------------------------- */

int pmm_free_page(uint64_t address) {
    if (address == 0 ||
        address >= SUNOS_PMM_MAX_MEMORY ||
        address % SUNOS_PAGE_SIZE != 0) {
        return -1;
    }

    uint64_t page = address / SUNOS_PAGE_SIZE;

    if (page >= total_count || !page_used(page)) {
        return -1;
    }

    clear_page(page);
    free_count++;

    return 0;
}

/* --------------------------------------------------
   Get available physical pages
   -------------------------------------------------- */

uint64_t pmm_free_pages(void) {
    return free_count;
}
