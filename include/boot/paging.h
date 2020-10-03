#pragma once
#include "common/paging.h"
#include <stdint.h>


#define PML4_SIZE 0x1000
#define PDPT_SIZE 0x1000
#define PD_SIZE 0x1000
#define PTS_SIZE 0x200000
#define PAGING_STRUCTS_SIZE (PML4_SIZE + PDPT_SIZE + PD_SIZE + PTS_SIZE)

void paging_set_up_boot_mapping(void* paging_buf, uint64_t *old_pml4, uintptr_t kernel_begin_physaddr);
