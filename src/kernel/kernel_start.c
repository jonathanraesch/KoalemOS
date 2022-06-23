#include <stdint.h>
#include "common/boot_info.h"
#include "kernel/gdt.h"
#include "kernel/interrupt.h"
#include "kernel/memory.h"
#include "kernel/runtime_error_check.h"
#include "kernel/apic.h"
#include "kernel/ap_boot.h"
#include "kernel/graphics.h"
#include "kernel/acpi.h"
#include "kernel/pci.h"
#include "kernel/kernel.h"
#include "kernel/tls.h"


#define KERNEL_STACK_SIZE 0x4000


static _Thread_local uint8_t kernel_stack[KERNEL_STACK_SIZE];

static boot_info boot_inf;


void* __get_kernel_sp() {
	return &kernel_stack[KERNEL_STACK_SIZE];
}

void __kernel_bsp_init(boot_info* bi_ptr) {
	boot_inf = *bi_ptr; // do not use bi_ptr after init_memory_management!
	setup_idt();
	init_idt();
	init_apic(boot_inf.tsc_freq_hz);
	boot_aps();
	init_memory_management(&boot_inf.mmap_data);
	if(!kernel_post_init_check()) {
		kernel_panic(U"post init check failed");
	}
	init_graphics(&boot_inf.gop_fb_info, 20);
	fill_screen(0.0, 0.0, 0.0);
	init_acpi(boot_inf.acpi_x_r_sdt);
	if(!init_pci()) {
		kernel_panic(U"failed to initialize PCI");
	}

	if(!tls_reserve_ap_space(ap_count)) {
		kernel_panic(U"failed to reserve space for AP TLS");
	}
	for(int i = 0; i < ap_count; i++) {
		print_str(U"AP booted\n");
	}
}

void __kernel_init(boot_info* bi_ptr) {
	init_gdt();

	if(bi_ptr) {
		__kernel_bsp_init(bi_ptr);
	} else {
		init_idt();
		init_apic(boot_inf.tsc_freq_hz);
	}
}
