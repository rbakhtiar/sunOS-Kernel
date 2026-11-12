
#include "../../include/idt.h"

static IdtEntry idt[IDT_ENTRIES];

static void idt_load(const IdtPointer *pointer) {
    __asm__ volatile(
        "lidt (%0)"
        :
        : "r"(pointer)
        : "memory"
    );
}

void idt_set_gate(
    uint8_t vector,
    void (*handler)(void),
    uint8_t attributes
) {
    uint64_t address = (uint64_t)(uintptr_t)handler;

    idt[vector].offset_low = address & 0xFFFF;
    idt[vector].selector = 0x08;
    idt[vector].ist = 0;
    idt[vector].attributes = attributes;
    idt[vector].offset_middle = (address >> 16) & 0xFFFF;
    idt[vector].offset_high = (address >> 32) & 0xFFFFFFFF;
    idt[vector].reserved = 0;
}

void idt_init(void) {
    for (unsigned int i = 0; i < IDT_ENTRIES; i++) {
        idt[i] = (IdtEntry){0};
    }

    IdtPointer pointer = {
        .limit = sizeof(idt) - 1,
        .base = (uint64_t)(uintptr_t)idt
    };

    idt_load(&pointer);
}
