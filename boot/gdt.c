#include "gdt.h"


const uint64_t boot_gdt32[4] = {
	0,
	GDT_SEGMENT_DESCRIPTOR(
		0x00000000,
		0x000FFFFF,
		GDT_SEGMENT_TYPE_CODE | GDT_SEGMENT_TYPE_EXECUTE_READ,
		0,
		GDT_SEGMENT_FLAG_S_TYPE | GDT_SEGMENT_FLAG_PRESENT | GDT_SEGMENT_FLAG_OPERAND_SIZE | GDT_SEGMENT_FLAG_GRANULARITY
	),
	GDT_SEGMENT_DESCRIPTOR(
		0x00000000,
		0x000FFFFF,
		GDT_SEGMENT_TYPE_DATA | GDT_SEGMENT_TYPE_READ_WRITE,
		0,
		GDT_SEGMENT_FLAG_S_TYPE | GDT_SEGMENT_FLAG_PRESENT | GDT_SEGMENT_FLAG_OPERAND_SIZE | GDT_SEGMENT_FLAG_GRANULARITY
	),
	GDT_SEGMENT_DESCRIPTOR(
		0,
		0,
		GDT_SEGMENT_TYPE_CODE,
		0,
		GDT_SEGMENT_FLAG_S_TYPE | GDT_SEGMENT_FLAG_PRESENT | GDT_SEGMENT_FLAG_64BIT_CODE
	)
};

const uint64_t boot_gdt64[2] = {
	0,
	GDT_SEGMENT_DESCRIPTOR(
		0,
		0,
		GDT_SEGMENT_TYPE_CODE,
		0,
		GDT_SEGMENT_FLAG_S_TYPE | GDT_SEGMENT_FLAG_PRESENT | GDT_SEGMENT_FLAG_64BIT_CODE
	)
};


static gdtr32_t boot_gdtr32;
static gdtr64_t boot_gdtr64;


gdtr32_t* boot_set_up_gdt32() {
	boot_gdtr32.base_address = (uint32_t)boot_gdt32;	
	boot_gdtr32.limit = sizeof(boot_gdt32);
	return &boot_gdtr32;
}

gdtr64_t* boot_set_up_gdt64() {
	boot_gdtr64.base_address = (uint64_t)boot_gdt64;
	boot_gdtr64.limit = sizeof(boot_gdt64);
	return &boot_gdtr64;
}