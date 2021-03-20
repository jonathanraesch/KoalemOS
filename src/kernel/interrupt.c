#include "kernel/interrupt.h"
#include "kernel/kernel.h"
#include <stdbool.h>
#include <threads.h>


extern uint16_t get_cs();
extern void load_idt(void* base, uint16_t limit);

extern void isr_divide_error();
extern void isr_debug();
extern void isr_nmi();
extern void isr_breakpoint();
extern void isr_overflow();
extern void isr_bound_range();
extern void isr_invalid_opcode();
extern void isr_no_coproc();
extern void isr_double_fault();
extern void isr_invalid_tss();
extern void isr_segment_not_present();
extern void isr_ss_fault();
extern void isr_general_protection();
extern void isr_page_fault();
extern void isr_math_fault();
extern void isr_align_check();
extern void isr_machine_check();
extern void isr_simd_exception();
extern void isr_virtualization_exception();
extern void isr_control_protection();


#define IDT_GATE_PRESENT_FLAG 0x800000000000
#define IDT_GATE_DPL(X) ((uint64_t)(X)<<45)
#define IDT_GATE_TYPE_TASK 0x50000000000
#define IDT_GATE_TYPE_INTERRUPT 0xE0000000000
#define IDT_GATE_TYPE_TRAP 0xF0000000000
#define IDT_GATE_SEG_SEL(X) ((uint16_t)(X)<<16)
#define IDT_GATE_IST(X) ((uint64_t)((X)&0x7)<<32)
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

static mtx_t idt_mutex;


uint8_t alloc_interrupt_vector(void isr()) {
	if(mtx_lock(&idt_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	for(int i = 0; i < IDT_ENTRY_COUNT-32; i++) {
		if(!int_used[i]) {
			idt[i+32] = IDT_INT_GATE(isr, get_cs(), 0, 0);
			int_used[i] = true;
			if(mtx_unlock(&idt_mutex) != thrd_success) {
				kernel_panic(U"mutex failed");
			}
			return i+32;
		}
	}
	if(mtx_unlock(&idt_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	return 0;
}

void free_interrupt_vector(uint8_t vec) {
	if(mtx_lock(&idt_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	idt[vec] = (idt_gate_descr){.low=0, .high=0};
	int_used[vec-32] = false;
	if(mtx_unlock(&idt_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}

void setup_idt() {
	if(mtx_init(&idt_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	uint16_t cs = get_cs();

	idt[0] = IDT_INT_GATE(isr_divide_error, cs, 0, 0);
	idt[1] = IDT_INT_GATE(isr_debug, cs, 0, 0);
	idt[2] = IDT_INT_GATE(isr_nmi, cs, 0, 0);
	idt[3] = IDT_INT_GATE(isr_breakpoint, cs, 0, 0);
	idt[4] = IDT_INT_GATE(isr_overflow, cs, 0, 0);
	idt[5] = IDT_INT_GATE(isr_bound_range, cs, 0, 0);
	idt[6] = IDT_INT_GATE(isr_invalid_opcode, cs, 0, 0);
	idt[7] = IDT_INT_GATE(isr_no_coproc, cs, 0, 0);
	idt[8] = IDT_INT_GATE(isr_double_fault, cs, 0, 0);
	idt[9] = (idt_gate_descr){.low=0, .high=0};
	idt[10] = IDT_INT_GATE(isr_invalid_tss, cs, 0, 0);
	idt[11] = IDT_INT_GATE(isr_segment_not_present, cs, 0, 0);
	idt[12] = IDT_INT_GATE(isr_ss_fault, cs, 0, 0);
	idt[13] = IDT_INT_GATE(isr_general_protection, cs, 0, 0);
	idt[14] = IDT_INT_GATE(isr_page_fault, cs, 0, 0);
	idt[15] = (idt_gate_descr){.low=0, .high=0};
	idt[16] = IDT_INT_GATE(isr_math_fault, cs, 0, 0);
	idt[17] = IDT_INT_GATE(isr_align_check, cs, 0, 0);
	idt[18] = IDT_INT_GATE(isr_machine_check, cs, 0, 0);
	idt[19] = IDT_INT_GATE(isr_simd_exception, cs, 0, 0);
	idt[20] = IDT_INT_GATE(isr_virtualization_exception, cs, 0, 0);
	idt[21] = IDT_INT_GATE(isr_control_protection, cs, 0, 0);

	for(int i = 22; i < IDT_ENTRY_COUNT; i++) {
		idt[i] = (idt_gate_descr){.low=0, .high=0};
	}
}

void init_idt() {
	load_idt(idt, IDT_ENTRY_COUNT*sizeof(idt_gate_descr)-1);
}
