#include "kernel/ap_boot.h"
#include "kernel/memory.h"
#include "kernel/apic.h"
#include <string.h>


extern uint8_t ap_boot_image[];

bool ap_boot_paddr_unset = true;
void* ap_boot_paddr = 0;


void boot_aps() {
	int pages = ap_boot_image_size % 0x1000 ? ap_boot_image_size / 0x1000 + 1 : ap_boot_image_size % 0x1000;
	for(int i = 0; i < pages; i++) {
		map_page((void*)((uintptr_t)ap_boot_paddr + 0x1000*pages), (void*)((uintptr_t)ap_boot_paddr + 0x1000*pages), PAGING_FLAG_READ_WRITE);
	}
	memcpy(ap_boot_paddr, &ap_boot_image, ap_boot_image_size);

	send_init_sipi_sipi((uintptr_t)ap_boot_paddr>>12);
}
