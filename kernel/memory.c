#include "memory.h"
#include "kernel.h"


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


typedef struct {
	uint64_t base_addr;
	uint64_t pages;
} memory_range;

typedef struct {
	memory_range* memory_ranges;
	uint64_t range_count;
} memory_map;


#define MMAP_MAX_RANGE_COUNT 1000

memory_range _mem_range_buf[MMAP_MAX_RANGE_COUNT];
memory_map mmap = {.memory_ranges=_mem_range_buf, .range_count=0};


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
				if(mmap.range_count >= MMAP_MAX_RANGE_COUNT) {
					kernel_panic();
				}
				memory_range range = {.base_addr=cur_desc.PhysicalStart, .pages=cur_desc.NumberOfPages};
				mmap.memory_ranges[mmap.range_count] = range;
				mmap.range_count++;
				break;
			default:
				break;
		}

		cur_desc_ptr += mmap_data->descriptor_size;
	}
}
