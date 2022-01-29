#include "InterruptController.h"

struct interrupt_frame {
    int error_code;
};

NAKED void interrupt_handler(struct interrupt_frame* frame) {
    asm volatile ("pushal");
    asm volatile ("xchg %bx, %bx");
    asm volatile ("popal; iret");
}

void InterruptController::register_vector_handler(uint8_t vector, EVENT_TYPE event_type, void* isr, uint8_t flags, uint16_t isr_cs) {
    idt_entry_t* descriptor = &idt[vector];
 
    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs      = isr_cs; // this value can be whatever offset your kernel code selector is in your GDT
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

void InterruptController::initialize() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 0x40; vector++) {
        register_vector_handler(vector, test, (void*)interrupt_handler, 0x8E, 0x8);
        this->vectors[vector] = true;
    }

    __asm__ volatile ("lidt %0" : : "memory"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag


}

InterruptController::InterruptController(){
    initialize();
}
