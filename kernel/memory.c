#include "memory.h"
#include "kernel.h"
#include "mmap.h"
#include <stdbool.h>


typedef struct __attribute__((__packed__)) {
	uint32_t Type;
	uint32_t _padding;
	uint64_t PhysicalStart;
	uint64_t VirtualStart;
	uint64_t NumberOfPages;
	uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef enum {
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiPersistentMemory,
	EfiMaxMemoryType
} EFI_MEMORY_TYPE;


#define PHYS_MMAP_MAX_RANGE_COUNT 1000
memory_range _phys_mmap_range_buf[PHYS_MMAP_MAX_RANGE_COUNT];
memory_map phys_mmap = {.memory_ranges=_phys_mmap_range_buf, .range_count=0, .max_range_count=PHYS_MMAP_MAX_RANGE_COUNT};

#define PHYS_ALLOC_MAP_MAX_RANGE_COUNT 1000
memory_range _phys_alloc_map_range_buf[PHYS_ALLOC_MAP_MAX_RANGE_COUNT];
memory_map phys_alloc_map = {.memory_ranges=_phys_alloc_map_range_buf, .range_count=0, .max_range_count=PHYS_ALLOC_MAP_MAX_RANGE_COUNT};


void init_mmap(efi_mmap_data* mmap_data) {
	void *cur_desc_ptr =  mmap_data->descriptors;
	const void *efi_mmap_end = mmap_data->descriptors + mmap_data->mmap_size;

	while(cur_desc_ptr < efi_mmap_end) {

		EFI_MEMORY_DESCRIPTOR cur_desc = *((EFI_MEMORY_DESCRIPTOR*)cur_desc_ptr);
		switch(cur_desc.Type) {
			case EfiLoaderCode:
			case EfiLoaderData:
			case EfiBootServicesCode:
			case EfiBootServicesData:
			case EfiConventionalMemory:
			case EfiPersistentMemory:
				if(mmap_add_range(&phys_mmap, cur_desc.PhysicalStart, cur_desc.NumberOfPages)) {
					kernel_panic();
				}
				break;
			default:
				break;
		}

		cur_desc_ptr += mmap_data->descriptor_size;
	}
}


int alloc_phys_pages(uint64_t pages) {
	if(void* base_addr = mmap_get_pages(&phys_mmap, pages)) {
		if(mmap_add_range(&phys_alloc_map, base_addr, pages)) {
			return base_addr;
		}
	}
	return false;
}

int free_phys_pages(void* base_addr) {
	if(uint64_t pages = mmap_get_range(&phys_alloc_map, base_addr)) {
		if(mmap_add_range_merge(&phys_mmap, base_addr, pages)) {
			return true;
		}
		if(mmap_add_range(&phys_alloc_map, base_addr, pages)) {
			return false;
		}
		kernel_panic();
	}
	return false;
}
