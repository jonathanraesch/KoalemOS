#include "kernel/interrupt.h"
#include "kernel/isr/inter_processor_call.h"
#include "kernel/apic.h"
#include <stdatomic.h>


void (*__ip_call_fun)(void);
_Atomic uint16_t __ip_call_count;

__attribute__((interrupt)) void isr_inter_processor_call(interrupt_frame_t* frame) {
	uint8_t fx_buf[512];
	__builtin_ia32_fxsave64((void*)fx_buf);
	__ip_call_fun();
	apic_write_eoi();
	atomic_fetch_add(&__ip_call_count, 1);
	__builtin_ia32_fxrstor64((void*)fx_buf);
}
