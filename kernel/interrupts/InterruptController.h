#pragma once

#include "../Base.h"

#define IDT_MAX_DESCRIPTORS 256
#define IRQ_BASE 0x50

typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;


typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;


class InterruptController {
	public:
    enum EVENT_TYPE {
        test
    };
    idtr_t idtr;
		__attribute__((aligned(0x10)))
		idt_entry_t idt[256];
		bool vectors[IDT_MAX_DESCRIPTORS];
		InterruptController();
		void initialize();
		void register_vector_handler(uint8_t, EVENT_TYPE, void*, uint8_t, uint16_t);

};
