#pragma once
#include "common/paging.h"
#include <stdint.h>
#include <efi.h>


typedef enum {
	page_size_4K,
	page_size_2M,
	page_size_1G
} page_size;


EFI_STATUS add_page_mapping(uint64_t *pml4, void* vaddr, void* paddr, page_size size, EFI_BOOT_SERVICES* bs);
