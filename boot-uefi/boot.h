#pragma once

void* get_pml4();
void __attribute__((noreturn)) boot_end(void* pml4, void* kernel_addr);

extern uint8_t _kernel_start_offset;