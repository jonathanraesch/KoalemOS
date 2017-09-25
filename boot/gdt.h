#pragma once
#include <stdint.h>


typedef struct __attribute__((__packed__)) gdtr_t {
	uint16_t limit;
	uint32_t base_address;
} gdtr_t;


#define GDT_SEGMENT_DESCRIPTOR(BASE, LIMIT, TYPE, DPL, FLAGS) (		\
	  ((LIMIT)&0xFFFF)					\
	| ((uint64_t)((LIMIT)&0xF0000)<<32)	\
	| ((uint64_t)((BASE)&0xFFFFFF)<<16)	\
	| ((BASE)&0xFF000000)				\
	| ((uint64_t)(TYPE)<<40)			\
	| ((uint64_t)(DPL)<<45)				\
	| (FLAGS)							\
)


#define GDT_SEGMENT_TYPE_DATA			0
#define GDT_SEGMENT_TYPE_CODE			(1 << 3)
#define GDT_SEGMENT_TYPE_EXPAND_DOWN	(1 << 2)
#define GDT_SEGMENT_TYPE_CONFORMING		(1 << 2)
#define GDT_SEGMENT_TYPE_READ_WRITE		(1 << 1)
#define GDT_SEGMENT_TYPE_EXECUTE_READ	(1 << 1)
#define GDT_SEGMENT_TYPE_ACCESSED		1

#define GDT_SEGMENT_FLAG_S_TYPE			((uint64_t)1 << 44)
#define GDT_SEGMENT_FLAG_PRESENT		((uint64_t)1 << 47)
#define GDT_SEGMENT_FLAG_64BIT_CODE		((uint64_t)1 << 53)
#define GDT_SEGMENT_FLAG_OPERAND_SIZE	((uint64_t)1 << 54)
#define GDT_SEGMENT_FLAG_EXPAND_DOWN	((uint64_t)1 << 54)
#define GDT_SEGMENT_FLAG_GRANULARITY	((uint64_t)1 << 55)


gdtr_t* boot_set_up_gdt32();