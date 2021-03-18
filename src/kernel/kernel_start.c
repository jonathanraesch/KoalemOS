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


static _Thread_local uint8_t kernel_stack[0x2000];

static boot_info boot_inf;


void* __get_kernel_sp() {
	return &kernel_stack[0x2000];
}

void __kernel_bsp_init() {
	init_graphics(&boot_inf.gop_fb_info, 20);
	fill_screen(0.0, 0.0, 0.0);
	init_acpi(boot_inf.acpi_x_r_sdt);
	if(!init_pci()) {
		kernel_panic(U"failed to initialize PCI");
	}
}

void __kernel_init(boot_info* bi_ptr) {
	boot_inf = *bi_ptr;	// do not use bi_ptr after init_memory_management!
	init_gdt();
	setup_idt();
	init_memory_management(&boot_inf.mmap_data);
	kernel_post_init_check();
	init_apic(boot_inf.tsc_freq_hz);
	uint16_t ap_count = boot_aps();

	__kernel_bsp_init();

	for(int i = 0; i < ap_count; i++) {
		print_str(U"AP booted\n");
	}
}
