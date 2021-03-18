#include "kernel/kernel.h"
#include "kernel/ap_boot.h"
#include "kernel/graphics.h"


extern void* ap_boot_paddr;


void kmain() {
	volatile uint16_t* count_ptr = (uint16_t*)((uintptr_t)ap_boot_paddr + ap_boot_image_size - 2);
	volatile uint16_t* done_ptr = (uint16_t*)((uintptr_t)ap_boot_paddr + ap_boot_image_size - 4);
	uint16_t cpus_counted = 0;
	uint16_t cpus_done = 0;
	while(1) {
		if(*count_ptr > cpus_counted) {
			print_char('C');
			cpus_counted++;
		}
		if(*done_ptr > cpus_done) {
			print_char('D');
			cpus_done++;
		}
	}
}
