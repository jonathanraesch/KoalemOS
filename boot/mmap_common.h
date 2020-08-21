#pragma once
#include <stdint.h>

typedef struct {
	void* descriptors;
	uint64_t mmap_size;
	uint64_t descriptor_size;
} efi_mmap_data; 

#define EFI_MEM_TYPE_KERNEL 0x80000000
