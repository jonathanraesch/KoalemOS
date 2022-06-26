#include "kernel/interrupt.h"
#include "kernel/apic.h"
#include <stdatomic.h>


extern uint64_t __apic_read_tsc();

extern _Thread_local void (*__apic_timer_callback)();
extern _Thread_local _Atomic uint64_t __apic_tsc_end;


__attribute__((interrupt)) void isr_timer(interrupt_frame_t* frame) {
	uint8_t fx_buf[512];
	__builtin_ia32_fxsave64((void*)fx_buf);

	__apic_timer_callback();
	apic_write_eoi();

	__builtin_ia32_fxrstor64((void*)fx_buf);
}

__attribute__((interrupt)) void isr_timer_rdtsc(interrupt_frame_t* frame) {
	uint8_t fx_buf[512];
	__builtin_ia32_fxsave64((void*)fx_buf);

	atomic_store(&__apic_tsc_end, __apic_read_tsc());
	apic_write_eoi();

	__builtin_ia32_fxrstor64((void*)fx_buf);
}
