#include "kernel/gdt.h"
#include <stdint.h>
#include <stdbool.h>


#define GDT_FLAG_CS_CONFORMING	0x40000000000
#define GDT_FLAG_CODE			0x80000000000
#define GDT_FLAG_TSS			0x90000000000
#define GDT_FLAG_NOTSYS			0x100000000000
#define GDT_FLAG_PRESENT		0x800000000000
#define GDT_FLAG_64BIT			0x20000000000000

#define GDT_DPL(DPL)			((uint64_t)(DPL) << 45)

#define GDT_CS(CONF, DPL)		(GDT_FLAG_CODE | GDT_FLAG_NOTSYS | GDT_FLAG_PRESENT | GDT_FLAG_64BIT | GDT_DPL(DPL) | (CONF?GDT_FLAG_CS_CONFORMING:0))
#define GDT_TSS_LO(BASE, DPL)	(GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_DPL(DPL) | ((((uintptr_t)(BASE))&0xFFFFFF)<<16) | ((((uintptr_t)(BASE))&0xFF000000) << 56))
#define GDT_TSS_HI(BASE)		((((uintptr_t)(BASE))&0xFFFFFFFF00000000)>>32)


typedef struct {
	uint32_t _resv1;
	uint64_t rsp_0;
	uint64_t rsp_1;
	uint64_t rsp_2;
	uint32_t _resv2;
	uint64_t ist_1;
	uint64_t ist_2;
	uint64_t ist_3;
	uint64_t ist_4;
	uint64_t ist_5;
	uint64_t ist_6;
	uint64_t ist_7;
	uint64_t _resv3;
	uint16_t _resv4;
	uint16_t iomap_base;
	uint8_t iomap;
} tss_t;


void __load_task_register(uint16_t seg_sel);
void __load_gdt(void* gdt_base, uint16_t gdt_lim, uint16_t cs_sel);


static _Thread_local uint8_t rsp_0[0x1000];
// should a large IOMAP (> 1MiB - 104 bytes) be required, TSS code will have to be rewritten to use 4KiB granularity
static _Thread_local tss_t tss;
static _Thread_local uint64_t gdt[4];


void init_gdt() {
	tss = (tss_t){
		.rsp_0 = (uintptr_t)rsp_0,
		.iomap_base = 104
	};
	gdt[0] = 0;
	gdt[1] = GDT_CS(false, 0);
	gdt[2] = GDT_TSS_LO(&tss, 0);
	gdt[3] = GDT_TSS_HI(&tss);
	__load_gdt(gdt, sizeof(gdt)-1, 1*8);
	__load_task_register(2*8);
}
