#pragma once
#include <stdint.h>


typedef struct {
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} interrupt_frame_t;


#define INT_VEC_INTER_PROCESSOR_CALL	32
#define INT_VEC_SPURIOUS				255


uint8_t alloc_interrupt_vector(void isr());
void free_interrupt_vector(uint8_t vec);

void init_idt();
