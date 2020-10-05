#include "boot/paging.h"
#include "common/mmap.h"


#define PML4_INDEX_OF(VADDR) ((uintptr_t)(VADDR)>>39 & 511)
#define PDPT_INDEX_OF(VADDR) ((uintptr_t)(VADDR)>>30 & 511)
#define PD_INDEX_OF(VADDR) ((uintptr_t)(VADDR)>>21 & 511)
#define PT_INDEX_OF(VADDR) ((uintptr_t)(VADDR)>>12 & 511)

#define NEXT_STRUCT(ADDR) ((uint64_t*)((uintptr_t)(ADDR) & 0xFFFFFFFFFFFFF000))

EFI_STATUS add_page_mapping(uint64_t *pml4, void* vaddr, void* paddr, EFI_BOOT_SERVICES* bs) {
	EFI_STATUS status;
	if(!(pml4[PML4_INDEX_OF(vaddr)] & PAGING_FLAG_PRESENT)) {
		EFI_PHYSICAL_ADDRESS addr;
		status = uefi_call_wrapper(bs->AllocatePages, 4, AllocateAnyPages, EFI_MEM_TYPE_KERNEL, 1, &addr);
		if (status != EFI_SUCCESS) {
			return status;
		}
		pml4[PML4_INDEX_OF(vaddr)] = addr | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	}
	pml4[PML4_INDEX_OF(vaddr)] |= PAGING_FLAG_READ_WRITE;

	uint64_t* pdpt = NEXT_STRUCT(pml4[PML4_INDEX_OF(vaddr)]);
	if(!(pdpt[PDPT_INDEX_OF(vaddr)] & PAGING_FLAG_PRESENT)) {
		EFI_PHYSICAL_ADDRESS addr;
		status = uefi_call_wrapper(bs->AllocatePages, 4, AllocateAnyPages, EFI_MEM_TYPE_KERNEL, 1, &addr);
		if (status != EFI_SUCCESS) {
			return status;
		}
		pdpt[PDPT_INDEX_OF(vaddr)] = addr | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	}
	pdpt[PDPT_INDEX_OF(vaddr)] |= PAGING_FLAG_READ_WRITE;

	uint64_t* pd = NEXT_STRUCT(pdpt[PDPT_INDEX_OF(vaddr)]);
	if(!(pd[PD_INDEX_OF(vaddr)] & PAGING_FLAG_PRESENT)) {
		EFI_PHYSICAL_ADDRESS addr;
		status = uefi_call_wrapper(bs->AllocatePages, 4, AllocateAnyPages, EFI_MEM_TYPE_KERNEL, 1, &addr);
		if (status != EFI_SUCCESS) {
			return status;
		}
		pd[PD_INDEX_OF(vaddr)] = addr | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	}
	pd[PD_INDEX_OF(vaddr)] |= PAGING_FLAG_READ_WRITE;

	uint64_t* pt = NEXT_STRUCT(pd[PD_INDEX_OF(vaddr)]);
	pt[PT_INDEX_OF(vaddr)] = (uintptr_t)paddr | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;

	return EFI_SUCCESS;
}
