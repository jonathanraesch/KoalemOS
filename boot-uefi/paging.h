#pragma once
#include "../boot/paging_common.h"
#include <stdint.h>


void* paging_set_up_boot_mapping(uintptr_t kernel_begin_physaddr);
void tst();
