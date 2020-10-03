#include "boot/paging.h"


/* kernel code can not exceed its 1GB aligned memory region
   if its base address is 1GB aligned, the maximum size is 1GB */
void paging_set_up_boot_mapping(void* paging_buf, uint64_t *old_pml4, uintptr_t kernel_begin_physaddr) {
	uint64_t *pml4 = paging_buf;
	uint64_t *kernel_pdpt = paging_buf+PML4_SIZE;
	uint64_t *kernel_pd = paging_buf+PML4_SIZE+PDPT_SIZE;
	uint64_t *kernel_pts = paging_buf+PML4_SIZE+PDPT_SIZE+PD_SIZE;
	for(int i = 0; i < 512; i++) {
		pml4[i] = old_pml4[i];
	}

	pml4[PAGING_PML4_OFFSET(KERNEL_LINADDR & 0xFFFFFFFFFFFF)] = ((uintptr_t)kernel_pdpt) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	kernel_pdpt[PAGING_PDPT_OFFSET(KERNEL_LINADDR & 0xFFFFFFFFFFFF)] = ((uintptr_t)kernel_pd) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	for(int i = 0; i < 512; i++) {
		kernel_pd[i] = ((uintptr_t)&(kernel_pts[i*512])) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	}
	for(int i = 0; i < 512; i++) {
		for(int j = 0; j < 512; j++) {
			kernel_pts[i*512 + j] = (kernel_begin_physaddr + 0x200000*i + 0x1000*j) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
		}
	}

	pml4[511] = ((uintptr_t)pml4) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
}
