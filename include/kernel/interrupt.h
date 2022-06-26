#pragma once
#include <stdint.h>


uint8_t alloc_interrupt_vector(void isr());
void free_interrupt_vector(uint8_t vec);

void init_idt();
