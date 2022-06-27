#include "kernel/apic.h"
#include "kernel/interrupt.h"
#include "kernel/isr/inter_processor_call.h"
#include "kernel/kernel.h"
#include <stdatomic.h>
#include <threads.h>


#define APIC_OFFS_LOCAL_ID			0x020
#define APIC_OFFS_LOCAL_VER			0x030
#define APIC_OFFS_TPR				0x080
#define APIC_OFFS_APR				0x090
#define APIC_OFFS_PPR				0x0A0
#define APIC_OFFS_EOI				0x0B0
#define APIC_OFFS_RRD				0x0C0
#define APIC_OFFS_LOGIC_DEST		0x0D0
#define APIC_OFFS_DEST_FORMAT		0x0E0
#define APIC_OFFS_SPUR_INT_VEC		0x0F0
#define APIC_OFFS_ISR_0				0x100
#define APIC_OFFS_ISR_1				0x110
#define APIC_OFFS_ISR_2				0x120
#define APIC_OFFS_ISR_3				0x130
#define APIC_OFFS_ISR_4				0x140
#define APIC_OFFS_ISR_5				0x150
#define APIC_OFFS_ISR_6				0x160
#define APIC_OFFS_ISR_7				0x170
#define APIC_OFFS_TMR_0				0x180
#define APIC_OFFS_TMR_1				0x190
#define APIC_OFFS_TMR_2				0x1A0
#define APIC_OFFS_TMR_3				0x1B0
#define APIC_OFFS_TMR_4				0x1C0
#define APIC_OFFS_TMR_5				0x1D0
#define APIC_OFFS_TMR_6				0x1E0
#define APIC_OFFS_TMR_7				0x1F0
#define APIC_OFFS_IRR_0				0x200
#define APIC_OFFS_IRR_1				0x210
#define APIC_OFFS_IRR_2				0x220
#define APIC_OFFS_IRR_3				0x230
#define APIC_OFFS_IRR_4				0x240
#define APIC_OFFS_IRR_5				0x250
#define APIC_OFFS_IRR_6				0x260
#define APIC_OFFS_IRR_7				0x270
#define APIC_OFFS_ERROR_STATUS		0x280
#define APIC_OFFS_LVT_CMCI			0x2F0
#define APIC_OFFS_ICR_LO			0x300
#define APIC_OFFS_ICR_HI			0x310
#define APIC_OFFS_LVT_TIMER			0x320
#define APIC_OFFS_LVT_THERM_SENS	0x330
#define APIC_OFFS_LVT_PERFMON_CNT	0x340
#define APIC_OFFS_LVT_LINT0			0x350
#define APIC_OFFS_LVT_LINT1			0x350
#define APIC_OFFS_LVT_ERROR			0x370
#define APIC_OFFS_INIT_CNT			0x380
#define APIC_OFFS_CUR_CNT			0x390
#define APIC_OFFS_DIV_CONF			0x3E0


#define APIC_IPI_DELIV_LOWEST		0x100
#define APIC_IPI_DELIV_SMI			0x200
#define APIC_IPI_DELIV_NMI			0x400
#define APIC_IPI_DELIV_INIT			0x500
#define APIC_IPI_DELIV_SIPI			0x600
#define APIC_IPI_LEV_ASS			0x4000
#define APIC_IPI_LEV_DEASS			0
#define APIC_IPI_DEST_SELF			0x40000
#define APIC_IPI_DEST_ALL			0x80000
#define APIC_IPI_DEST_NOTSELF		0xC0000

#define APIC_SPUR_INT_FLAG_SOFTWARE_ENABLE				0x100
#define APIC_SPUR_INT_FLAG_FOCUS_PROCESSOR_CHECKING		0x200
#define APIC_SPUR_INT_FLAG_SURPRESS_EOI_BROADCAST		0x1000


#define APIC_REG(OFFSET) (*(volatile uint32_t*)((uintptr_t)apic_base + (OFFSET)))


extern void* __apic_enable();
extern void isr_timer();
extern void isr_timer_rdtsc();
extern uint64_t __apic_read_tsc();

extern uint16_t ap_count;

_Thread_local void (*__apic_timer_callback)();

_Thread_local _Atomic uint64_t __apic_tsc_end = 0;

static _Thread_local void* apic_base;
static _Thread_local uint8_t timer_int;
static _Thread_local uint64_t tsc_freq;
static _Thread_local double timer_freq;

static mtx_t ipcall_mutex;


void start_apic_timer(uint32_t count, uint8_t div_pow, bool periodic, void (*callback)()) {
	__apic_timer_callback = callback;
	if(periodic) {
		APIC_REG(APIC_OFFS_LVT_TIMER) = timer_int | (1u << 17);
	} else {
		APIC_REG(APIC_OFFS_LVT_TIMER) = timer_int;
	}

	div_pow -= 1u;
	APIC_REG(APIC_OFFS_DIV_CONF) = (div_pow&4 << 1) | (div_pow&3);

	APIC_REG(APIC_OFFS_INIT_CNT) = count;
}

void start_apic_timer_rt(double seconds, bool periodic, void (*callback)()) {
	if(seconds*timer_freq < 0xFFFFFFFF) {
		start_apic_timer(seconds*timer_freq, 0, periodic, callback);
	} else if(seconds*timer_freq/2.0 < 0xFFFFFFFF) {
		start_apic_timer(seconds*timer_freq/2.0, 1, periodic, callback);
	} else if(seconds*timer_freq/4.0 < 0xFFFFFFFF) {
		start_apic_timer(seconds*timer_freq/4.0, 2, periodic, callback);
	} else if(seconds*timer_freq/8.0 < 0xFFFFFFFF) {
		start_apic_timer(seconds*timer_freq/8.0, 3, periodic, callback);
	} else if(seconds*timer_freq/16.0 < 0xFFFFFFFF) {
		start_apic_timer(seconds*timer_freq/16.0, 4, periodic, callback);
	} else if(seconds*timer_freq/32.0 < 0xFFFFFFFF) {
		start_apic_timer(seconds*timer_freq/32.0, 5, periodic, callback);
	} else if(seconds*timer_freq/64.0 < 0xFFFFFFFF) {
		start_apic_timer(seconds*timer_freq/64.0, 6, periodic, callback);
	} else {
		start_apic_timer(seconds*timer_freq/128.0, 7, periodic, callback);
	}
}

void stop_apic_timer() {
	APIC_REG(APIC_OFFS_INIT_CNT) = 0;
}


static void set_timer_freq(uint64_t tsc_freq_hz) {
	const uint32_t wait_cnt = 100000000;

	uint8_t vec = alloc_interrupt_vector(isr_timer_rdtsc);
	APIC_REG(APIC_OFFS_LVT_TIMER) = vec;
	APIC_REG(APIC_OFFS_DIV_CONF) = 11;

	APIC_REG(APIC_OFFS_INIT_CNT) = wait_cnt;
	uint64_t tsc_start = __apic_read_tsc();
	while(!atomic_load(&__apic_tsc_end)) {}

	APIC_REG(APIC_OFFS_LVT_TIMER) = 0x10000;
	free_interrupt_vector(vec);
	double t_diff = atomic_load(&__apic_tsc_end)-tsc_start;
	timer_freq = (double)tsc_freq_hz * (double)wait_cnt / t_diff;
}


void init_apic(uint64_t tsc_freq_hz) {
	apic_base = __apic_enable();
	APIC_REG(APIC_OFFS_SPUR_INT_VEC) = INT_VEC_SPURIOUS | APIC_SPUR_INT_FLAG_SOFTWARE_ENABLE;
	APIC_REG(APIC_OFFS_ERROR_STATUS) = 0;

	timer_int = alloc_interrupt_vector(isr_timer);
	tsc_freq = tsc_freq_hz;
	set_timer_freq(tsc_freq_hz);
}


void broadcast_ipi(uint8_t vec) {
	if(get_working_processor_count() > 1) {
		APIC_REG(APIC_OFFS_ICR_LO) = APIC_IPI_DEST_NOTSELF | APIC_IPI_LEV_ASS | vec;
	}
}

void send_init_sipi_sipi(uint8_t vec) {
	if(mtx_init(&ipcall_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}

	APIC_REG(APIC_OFFS_ICR_LO) = APIC_IPI_DEST_NOTSELF | APIC_IPI_LEV_ASS | APIC_IPI_DELIV_INIT;
	uint64_t tsc_target = __apic_read_tsc() + tsc_freq/100;
	while(__apic_read_tsc() < tsc_target) {}
	APIC_REG(APIC_OFFS_ICR_LO) = APIC_IPI_DEST_NOTSELF | APIC_IPI_LEV_ASS | APIC_IPI_DELIV_SIPI | vec;
	tsc_target = __apic_read_tsc() + tsc_freq/5000;
	while(__apic_read_tsc() < tsc_target) {}
	APIC_REG(APIC_OFFS_ICR_LO) = APIC_IPI_DEST_NOTSELF | APIC_IPI_LEV_ASS | APIC_IPI_DELIV_SIPI | vec;
	tsc_target = __apic_read_tsc() + tsc_freq/10;
	while(__apic_read_tsc() < tsc_target) {}
}


void inter_processor_call(void (*f)(void)) {
	if(mtx_lock(&ipcall_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	__ip_call_fun = f;
	atomic_store(&__ip_call_count, 0);
	broadcast_ipi(INT_VEC_INTER_PROCESSOR_CALL);
	while(atomic_load(&__ip_call_count) < get_working_processor_count() - 1) {
	}
	if(mtx_unlock(&ipcall_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}


void apic_write_eoi() {
	APIC_REG(APIC_OFFS_EOI) = 0;
}
