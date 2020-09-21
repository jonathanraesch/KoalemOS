#include "interrupt.h"
#include <stdint.h>


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
#define IDT_GATE_OFFSET_LO(X) (((uint64_t)(X)&0xFFFF) | ((uint64_t)(X)<<32 & 0xFFFF000000000000))
#define IDT_GATE_OFFSET_HI(X) ((uint64_t)(X)>>32)

typedef struct {
	uint64_t low;
	uint64_t high;
} idt_gate_descr;

#define IDT_INT_GATE(OFFSET, CS, DPL) ((idt_gate_descr){.low=IDT_GATE_PRESENT_FLAG|IDT_GATE_DPL((DPL))|IDT_GATE_TYPE_INTERRUPT|		\
	IDT_GATE_SEG_SEL(CS)|IDT_GATE_OFFSET_LO((OFFSET)), .high=IDT_GATE_OFFSET_HI((OFFSET))})


#define IDT_ENTRY_COUNT 22
idt_gate_descr idt[IDT_ENTRY_COUNT];


void setup_idt() {
	uint16_t cs = get_cs();
	idt[0] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// DE
	idt[1] = IDT_INT_GATE(isr_do_nothing, cs, 0);		// DB
	idt[2] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// NMI
	idt[3] = IDT_INT_GATE(isr_do_nothing, cs, 0);		// BP
	idt[4] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// OF
	idt[5] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// BR
	idt[6] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// UD
	idt[7] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// NM
	idt[8] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// DF
	idt[9] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// coprocessor segment overrun
	idt[10] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// TS
	idt[11] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// NP
	idt[12] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// SS
	idt[13] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// GP
	idt[14] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// PF
	idt[15] = (idt_gate_descr){.low=0, .high=0};	// interrupt 15
	idt[16] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// MF
	idt[17] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// AC
	idt[18] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// MC
	idt[19] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// XM
	idt[20] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// VE
	idt[21] = IDT_INT_GATE(isr_not_implemented, cs, 0);	// CP
	load_idt(idt, IDT_ENTRY_COUNT*8-1);
}
