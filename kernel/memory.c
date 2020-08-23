#include "memory.h"
#include "kernel.h"
#include "mmap.h"
#include "../boot/paging_common.h"
#include <stdbool.h>


extern void invalidate_tlbs_for(void* vaddr);


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
				if(mmap_add_range(&phys_mmap, (void*)cur_desc.PhysicalStart, cur_desc.NumberOfPages)) {
					kernel_panic();
				}
				break;
			default:
				break;
		}

		cur_desc_ptr += mmap_data->descriptor_size;
	}
}


void* alloc_phys_pages(uint64_t pages) {
	void* base_addr = mmap_get_pages(&phys_mmap, pages);
	if(base_addr) {
		if(mmap_add_range(&phys_alloc_map, base_addr, pages)) {
			return base_addr;
		}
	}
	return 0;
}

int free_phys_pages(void* base_addr) {
	uint64_t pages = mmap_get_range(&phys_alloc_map, base_addr);
	if(pages) {
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


#define PML4E_ADDR_OF(VADDR) ((uint64_t*)(0xFFFFFFFFFFFFF000 | ((uintptr_t)(VADDR)&0xFF8000000000) >> 39))
#define PDPTE_ADDR_OF(VADDR) ((uint64_t*)(0xFFFFFFFFFFE00000 | ((uintptr_t)(VADDR)&0xFF8000000000) >> 27 | ((uintptr_t)(VADDR)&0x7FC0000000) >> 30))
#define PDE_ADDR_OF(VADDR)   ((uint64_t*)(0xFFFFFFFFC0000000 | ((uintptr_t)(VADDR)&0xFF8000000000) >> 18 | ((uintptr_t)(VADDR)&0x7FC0000000) >> 18 | ((uintptr_t)(VADDR)&0x3FE00000) >> 21))
#define PTE_ADDR_OF(VADDR)   ((uint64_t*)(0xFFFFFF8000000000 | ((uintptr_t)(VADDR)&0xFF8000000000) >>  9 | ((uintptr_t)(VADDR)&0x7FC0000000) >>  9 | ((uintptr_t)(VADDR)&0x3FE00000) >>  9 | ((uintptr_t)(VADDR)&0x1FF000) >> 12))

void map_page(void* vaddr, void* paddr, uint64_t flags) {
	if (!(*PML4E_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT)) {
		uint64_t* pdpt_addr = alloc_phys_pages(1);
		if(pdpt_addr) {
			for(int i = 0; i < 512; i++) {
				pdpt_addr[i] = 0;
			}
			*PML4E_ADDR_OF(vaddr) = (uintptr_t)pdpt_addr | PAGING_FLAG_PRESENT | flags;
		} else {
			kernel_panic();
		}
	}
	if (!(*PDPTE_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT)) {
		uint64_t* pde_addr = alloc_phys_pages(1);
		if(pde_addr) {
			for(int i = 0; i < 512; i++) {
				pde_addr[i] = 0;
			}
			*PDPTE_ADDR_OF(vaddr) = (uintptr_t)pde_addr | PAGING_FLAG_PRESENT | flags;
		} else {
			kernel_panic();
		}
	}
	if (!(*PDE_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT)) {
		uint64_t* pte_addr = alloc_phys_pages(1);
		if(pte_addr) {
			for(int i = 0; i < 512; i++) {
				pte_addr[i] = 0;
			}
			*PDE_ADDR_OF(vaddr) = (uintptr_t)pte_addr | PAGING_FLAG_PRESENT | flags;
		} else {
			kernel_panic();
		}
	}
	*PTE_ADDR_OF(vaddr) = (uintptr_t)paddr | PAGING_FLAG_PRESENT | flags;

	invalidate_tlbs_for(vaddr);
}

void unmap_page(void* vaddr) {
	*PTE_ADDR_OF(vaddr) = 0;

	invalidate_tlbs_for(vaddr);
}
