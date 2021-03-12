#include "kernel/apic.h"
#include "kernel/interrupt.h"


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


#define APIC_REG(OFFSET) (*(uint32_t*)((uintptr_t)apic_base + (OFFSET)))


extern void* __apic_enable();
extern void isr_timer();

void* __apic_reg_eoi;
void (*__apic_timer_callback)();


static void* apic_base;
static uint8_t timer_int;


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

void stop_apic_timer() {
	APIC_REG(APIC_OFFS_INIT_CNT) = 0;
}


void init_apic() {
	apic_base = __apic_enable();
	__apic_reg_eoi = (void*)((uintptr_t)apic_base + APIC_OFFS_EOI);
	APIC_REG(APIC_OFFS_ERROR_STATUS) = 0;

	timer_int = alloc_interrupt_vector(isr_timer);
}

