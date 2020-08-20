#pragma once

#define EFI_MEM_TYPE_KERNEL 0x80000000

void* get_pml4();
void __attribute__((noreturn)) boot_end(void* pml4, void* kernel_addr);
