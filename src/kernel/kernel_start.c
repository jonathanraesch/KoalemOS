#include <stdint.h>
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


void* __get_kernel_sp() {
	return &kernel_stack[0x2000];
}

void __kernel_init(efi_mmap_data* mmap_data, uint64_t tsc_freq_hz) {
	init_gdt();
	setup_idt();
	init_memory_management(mmap_data);
	kernel_post_init_check();
	init_apic(tsc_freq_hz);
	boot_aps();
}

void __kernel_bsp_init(gop_framebuffer_info* gop_fb_info, void* acpi_x_r_sdt) {
	init_graphics(gop_fb_info, 20);
	fill_screen(0.0, 0.0, 0.0);
	init_acpi(acpi_x_r_sdt);
	if(!init_pci()) {
		kernel_panic(U"failed to initialize PCI");
	}
}
