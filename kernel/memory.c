#include "memory.h"
#include "kernel.h"
#include "mmap.h"
#include "../boot/paging_common.h"
#include "../boot/mmap_common.h"
#include <stdbool.h>


extern void invalidate_tlbs_for(void* vaddr);
void map_page(void* vaddr, void* paddr, uint64_t flags);


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


#define PHYS_MMAP_INIT_MAX_RANGE_COUNT (0x1000 / sizeof(memory_range))
_Static_assert (!((PHYS_MMAP_INIT_MAX_RANGE_COUNT * sizeof(memory_range)) & 0xfff), "physcial memory map not page-aligned");
memory_range _phys_mmap_range_buf[PHYS_MMAP_INIT_MAX_RANGE_COUNT] __attribute__ ((section ("PHYS_MMAP"))) = {0};
memory_map phys_mmap = {.memory_ranges=_phys_mmap_range_buf, .range_count=0, .max_range_count=PHYS_MMAP_INIT_MAX_RANGE_COUNT};

void* alloc_phys_pages(uint64_t pages) {
	void* base_addr = mmap_get_pages(&phys_mmap, pages);
	if(base_addr) {
		return base_addr;
	}
	return 0;
}

// TODO: if feasible, resize memory map after merge
int free_phys_pages(void* base_addr, uint64_t count) {
	if(mmap_add_range_merge(&phys_mmap, base_addr, count)) {
		return true;
	}
	if(base_addr) {
		map_page(phys_mmap.memory_ranges + phys_mmap.max_range_count, base_addr, PAGING_FLAG_READ_WRITE);
		phys_mmap.max_range_count += 0x1000 / sizeof(memory_range);
		return true;
	}
	return false;
}


#define PML4E_ADDR_OF(VADDR) ((uint64_t*)(0xFFFFFFFFFFFFF000 | ((uintptr_t)(VADDR)&0xFF8000000000) >> 36))
#define PDPTE_ADDR_OF(VADDR) ((uint64_t*)(0xFFFFFFFFFFE00000 | ((uintptr_t)(VADDR)&0xFFFFC0000000) >> 27))
#define PDE_ADDR_OF(VADDR)   ((uint64_t*)(0xFFFFFFFFC0000000 | ((uintptr_t)(VADDR)&0xFFFFFFE00000) >> 18))
#define PTE_ADDR_OF(VADDR)   ((uint64_t*)(0xFFFFFF8000000000 | ((uintptr_t)(VADDR)&0xFFFFFFFFF000) >>  9))

// TODO: make address calculation for zeroing structures more readable and/or performant
void map_page(void* vaddr, void* paddr, uint64_t flags) {
	if (!(*PML4E_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT)) {
		uint64_t* pdpt_addr = alloc_phys_pages(1);
		if(pdpt_addr) {
			*PML4E_ADDR_OF(vaddr) = (uintptr_t)pdpt_addr | PAGING_FLAG_PRESENT | flags;
			invalidate_tlbs_for(vaddr);
			invalidate_tlbs_for(PTE_ADDR_OF(vaddr));
			invalidate_tlbs_for(PDE_ADDR_OF(vaddr));
			invalidate_tlbs_for(PDPTE_ADDR_OF(vaddr));
			for(uint64_t i = 0; i < 512; i++) {
				uint64_t* base_pdpte_addr = (uint64_t*)(((uintptr_t)vaddr&0xFFFFFF8000000000) + 0x40000000*i);
				*PDPTE_ADDR_OF(base_pdpte_addr) = 0;
			}
		} else {
			kernel_panic();
		}
	}
	if (!(*PDPTE_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT)) {
		uint64_t* pde_addr = alloc_phys_pages(1);
		if(pde_addr) {
			*PDPTE_ADDR_OF(vaddr) = (uintptr_t)pde_addr | PAGING_FLAG_PRESENT | flags;
			invalidate_tlbs_for(vaddr);
			invalidate_tlbs_for(PTE_ADDR_OF(vaddr));
			invalidate_tlbs_for(PDE_ADDR_OF(vaddr));
			for(uint64_t i = 0; i < 512; i++) {
				uint64_t* base_pde_addr = (uint64_t*)(((uintptr_t)vaddr&0xFFFFFFFFC0000000) + 0x200000*i);
				*PDE_ADDR_OF(base_pde_addr) = 0;
			}
		} else {
			kernel_panic();
		}
	}
	if (!(*PDE_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT)) {
		uint64_t* pte_addr = alloc_phys_pages(1);
		if(pte_addr) {
			*PDE_ADDR_OF(vaddr) = (uintptr_t)pte_addr | PAGING_FLAG_PRESENT | flags;
			invalidate_tlbs_for(vaddr);
			invalidate_tlbs_for(PTE_ADDR_OF(vaddr));
			for(uint64_t i = 0; i < 512; i++) {
				uint64_t* base_pte_addr = (uint64_t*)(((uintptr_t)vaddr&0xFFFFFFFFFFE00000) + 0x1000*i);
				*PTE_ADDR_OF(base_pte_addr) = 0;
			}
		} else {
			kernel_panic();
		}
	}
	*PTE_ADDR_OF(vaddr) = (uintptr_t)paddr | PAGING_FLAG_PRESENT | flags;

	invalidate_tlbs_for(vaddr);
}

// TODO: fix memory leak
void unmap_page(void* vaddr) {
	*PTE_ADDR_OF(vaddr) = 0;

	invalidate_tlbs_for(vaddr);
}


void init_mmap(efi_mmap_data* mmap_data) {
	void *cur_desc_ptr =  mmap_data->descriptors;
	const void *efi_mmap_end = (void*)((uintptr_t)mmap_data->descriptors + mmap_data->mmap_size);

	while(cur_desc_ptr < efi_mmap_end) {

		EFI_MEMORY_DESCRIPTOR cur_desc = *((EFI_MEMORY_DESCRIPTOR*)cur_desc_ptr);
		switch(cur_desc.Type) {
			case EfiLoaderCode:
			case EfiLoaderData:
			case EfiBootServicesCode:
			case EfiBootServicesData:
			case EfiConventionalMemory:
			case EfiPersistentMemory:
				if(cur_desc.PhysicalStart == 0) {
					if(cur_desc.NumberOfPages < 2u) {
						break;
					}
					cur_desc.NumberOfPages -= 1;
					cur_desc.PhysicalStart += 0x1000;
				}
				if(!mmap_add_range(&phys_mmap, (void*)cur_desc.PhysicalStart, cur_desc.NumberOfPages)) {
					kernel_panic();
				}
				break;
			default:
				break;
		}

		cur_desc_ptr = (void*)((uintptr_t)cur_desc_ptr + mmap_data->descriptor_size);
	}

	for(uintptr_t addr = 0; addr<256*0x8000000000; addr+=0x8000000000) {
		if (addr == (KERNEL_LINADDR&0xFFFFFF8000000000)) {
			continue;
		}
		*PML4E_ADDR_OF(addr) = 0;
		invalidate_tlbs_for((void*)addr);
	}
	for(uintptr_t addr = 256*0x8000000000; addr<511*0x8000000000; addr+=0x8000000000) {
		if (addr == (KERNEL_LINADDR&0xFFFFFF8000000000)) {
			continue;
		}
		*PML4E_ADDR_OF(addr) = 0;
		invalidate_tlbs_for((void*)addr);
	}
}
