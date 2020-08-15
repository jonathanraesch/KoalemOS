#include "paging.h"


/* kernel code can not exceed its 1GB aligned memory region
   if its base address is 1GB aligned, the maximum size is 1GB */
static uint64_t __attribute__((aligned(0x1000))) pml4[512];
static uint64_t __attribute__((aligned(0x1000))) kernel_pdpt[512];
static uint64_t __attribute__((aligned(0x1000))) kernel_pd[512];
static uint64_t __attribute__((aligned(0x1000))) kernel_pts[512][512];
static uint64_t __attribute__((aligned(0x1000))) realmode_mem_pdpt[512];
static uint64_t __attribute__((aligned(0x1000))) realmode_mem_pd[512];
static uint64_t __attribute__((aligned(0x1000))) realmode_mem_pt[512];


void* paging_set_up_boot_mapping(uint64_t *old_pml4, uintptr_t kernel_begin_physaddr) {
	for(int i = 0; i < 512; i++) {
		pml4[i] = old_pml4[i];
	}

	pml4[PAGING_PML4_OFFSET(KERNEL_LINADDR & 0xFFFFFFFFFFFF)] = ((uintptr_t)kernel_pdpt) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	kernel_pdpt[PAGING_PDPT_OFFSET(KERNEL_LINADDR & 0xFFFFFFFFFFFF)] = ((uintptr_t)kernel_pd) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	for(int i = 0; i < 512; i++) {
		kernel_pd[i] = ((uintptr_t)(kernel_pts[i])) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	}
	for(int i = 0; i < 512; i++) {
		for(int j = 0; j < 512; j++) {
			kernel_pts[i][j] = (kernel_begin_physaddr + 0x200000*i + 0x1000*j) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
		}
	}

	pml4[PAGING_PML4_OFFSET(REALMODE_MEM_LINADDR & 0xFFFFFFFFFFFF)] = ((uintptr_t)realmode_mem_pdpt) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE | PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE;
	realmode_mem_pdpt[PAGING_PDPT_OFFSET(REALMODE_MEM_LINADDR & 0xFFFFFFFFFFFF)] = ((uintptr_t)realmode_mem_pd) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE | PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE;
	realmode_mem_pd[0] = ((uintptr_t)realmode_mem_pt) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE | PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE;
	for(int i = 0; i < 256; i++) {
		realmode_mem_pt[i] = ((uintptr_t)(0) + 0x1000*i) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE | PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE;
	}

	pml4[511] = ((uintptr_t)pml4) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;

	return pml4;
}


void tst() {

}