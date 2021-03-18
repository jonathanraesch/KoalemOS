#pragma once
#include "common/boot_info.h"


void* get_pml4();
void __attribute__((noreturn)) boot_end(void* pml4, void* kernel_addr, boot_info* bi_ptr);
