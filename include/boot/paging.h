#pragma once
#include "common/paging.h"
#include <stdint.h>
#include <efi.h>


EFI_STATUS add_page_mapping(uint64_t *pml4, void* vaddr, void* paddr, EFI_BOOT_SERVICES* bs);
