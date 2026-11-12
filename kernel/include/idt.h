
#ifndef SUNOS_IDT_H
#define SUNOS_IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} IdtEntry;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} IdtPointer;

void idt_init(void);

void idt_set_gate(
    uint8_t vector,
    void (*handler)(void),
    uint8_t attributes
);

#endif
