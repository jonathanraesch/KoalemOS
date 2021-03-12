#include "kernel/interrupt.h"
#include <stdbool.h>


extern uint16_t get_cs();
extern void load_idt(void* base, uint16_t limit);
extern void isr_not_implemented();
extern void isr_do_nothing();


#define IDT_GATE_PRESENT_FLAG 0x800000000000
#define IDT_GATE_DPL(X) ((uint64_t)(X)<<45)
#define IDT_GATE_TYPE_TASK 0x50000000000
#define IDT_GATE_TYPE_INTERRUPT 0xE0000000000
#define IDT_GATE_TYPE_TRAP 0xF0000000000
#define IDT_GATE_SEG_SEL(X) ((uint16_t)(X)<<16)
#define IDT_GATE_IST(X) ((uint64_t)((X)&0x7)<<16)
#define IDT_GATE_OFFSET_LO(X) (((uint64_t)(X)&0xFFFF) | ((uint64_t)(X)<<32 & 0xFFFF000000000000))
#define IDT_GATE_OFFSET_HI(X) ((uint64_t)(X)>>32)

typedef struct {
	uint64_t low;
	uint64_t high;
} idt_gate_descr;

#define IDT_INT_GATE(OFFSET, CS, DPL, IST) ((idt_gate_descr){.low=IDT_GATE_PRESENT_FLAG|IDT_GATE_DPL((DPL))|IDT_GATE_TYPE_INTERRUPT|		\
	IDT_GATE_IST(IST)|IDT_GATE_SEG_SEL(CS)|IDT_GATE_OFFSET_LO((OFFSET)), .high=IDT_GATE_OFFSET_HI((OFFSET))})


#define IDT_ENTRY_COUNT 256
static idt_gate_descr idt[IDT_ENTRY_COUNT];
static bool int_used[IDT_ENTRY_COUNT-32];


uint8_t alloc_interrupt_vector(void isr()) {
	for(int i = 0; i < IDT_ENTRY_COUNT-32; i++) {
		if(!int_used[i]) {
			idt[i+32] = IDT_INT_GATE(isr, get_cs(), 0, 0);
			return i+32;
		}
	}
	return 0;
}

void free_interrupt_vector(uint8_t vec) {
	idt[vec] = (idt_gate_descr){.low=0, .high=0};
	int_used[vec-32] = false;
}

void setup_idt() {
	uint16_t cs = get_cs();
	for(int i = 0; i < 22; i++) {
		idt[i] = IDT_INT_GATE(isr_not_implemented, cs, 0, 0);
	}
	for(int i = 22; i < IDT_ENTRY_COUNT; i++) {
		idt[i] = (idt_gate_descr){.low=0, .high=0};
	}
	idt[15] = (idt_gate_descr){.low=0, .high=0};	// interrupt 15

	idt[1] = IDT_INT_GATE(isr_do_nothing, cs, 0, 0);	// DB
	idt[3] = IDT_INT_GATE(isr_do_nothing, cs, 0, 0);	// BP

	load_idt(idt, IDT_ENTRY_COUNT*sizeof(idt_gate_descr)-1);
}
