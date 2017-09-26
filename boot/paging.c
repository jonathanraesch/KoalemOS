#include "paging.h"
#include <stdint.h>


/* linker symbols */
extern uint8_t _boot_begin_addr;
extern uint8_t _boot_end_addr;
extern uint8_t _kernel_begin_physaddr;
extern uint8_t _kernel_end_physaddr;


/* neither kernel nor boot code can exceed their 1GB aligned memory region
   if their base address is 1GB aligned, the maximum size is 1GB */
static uint64_t __attribute__((aligned(0x1000))) pml4[512];
static uint64_t __attribute__((aligned(0x1000))) boot_pdpt[512];
static uint64_t __attribute__((aligned(0x1000))) boot_pd[512];
static uint64_t __attribute__((aligned(0x1000))) boot_pts[512][512];
static uint64_t __attribute__((aligned(0x1000))) kernel_pdpt[512];
static uint64_t __attribute__((aligned(0x1000))) kernel_pd[512];
static uint64_t __attribute__((aligned(0x1000))) kernel_pts[512][512];


void* paging_set_up_boot_mapping() {
	pml4[0] = ((uintptr_t)boot_pdpt) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	boot_pdpt[PAGING_PDPT_OFFSET((uintptr_t)(&_boot_begin_addr))] = ((uintptr_t)boot_pd) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	for(int i = 0; i < 512; i++) {
		boot_pd[i] = ((uintptr_t)(boot_pts[i])) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	}
	for(int i = 0; i < 512; i++) {
		for(int j = 0; j < 512; j++) {
			boot_pts[i][j] = (((uintptr_t)(&_boot_begin_addr) & -((uintptr_t)1<<30)) + 0x200000*i + 0x1000*j) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
		}
	}

	pml4[PAGING_PML4_OFFSET(KERNEL_LINADDR)] = ((uintptr_t)kernel_pdpt) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	kernel_pdpt[PAGING_PDPT_OFFSET(KERNEL_LINADDR)] = ((uintptr_t)kernel_pd) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	for(int i = 0; i < 512; i++) {
		kernel_pd[i] = ((uintptr_t)(kernel_pts[i])) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
	}
	for(int i = 0; i < 512; i++) {
		for(int j = 0; j < 512; j++) {
			kernel_pts[i][j] = ((uintptr_t)(&_kernel_begin_physaddr) + 0x200000*i + 0x1000*j) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
		}
	}

	pml4[511] = ((uintptr_t)pml4) | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;

	return pml4;
}