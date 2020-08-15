#pragma once
#include "../boot/paging_common.h"
#include <stdint.h>


void* paging_set_up_boot_mapping(uint64_t *old_pml4, uintptr_t kernel_begin_physaddr);
void tst();
