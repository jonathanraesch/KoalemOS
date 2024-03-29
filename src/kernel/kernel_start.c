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
#include <stdatomic.h>


#define KERNEL_STACK_SIZE 0x4000
#define AP_INIT_STACK_SIZE 128


static _Thread_local uint8_t kernel_stack[KERNEL_STACK_SIZE];

static boot_info boot_inf;

static _Atomic uint16_t working_processor_count = 0;

void set_ap_init_stack(void* ptr, void* limit, uint64_t size_each);

void* __get_kernel_sp() {
	return &kernel_stack[KERNEL_STACK_SIZE];
}

static bool reserve_ap_init_stack(uint16_t ap_count) {
	void* ptr = kmalloc(AP_INIT_STACK_SIZE * ap_count);
	if(!ptr) {
		return false;
	}
	set_ap_init_stack(ptr, (void*)((uintptr_t)ptr+ap_count*AP_INIT_STACK_SIZE), AP_INIT_STACK_SIZE);
	return true;
}

void __kernel_bsp_init(boot_info* bi_ptr) {
	boot_inf = *bi_ptr; // do not use bi_ptr after init_memory_management!
	init_apic(boot_inf.tsc_freq_hz);
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

	boot_aps();
	if(!tls_reserve_ap_space(ap_count)) {
		kernel_panic(U"failed to reserve space for AP TLS");
	}
	if(!reserve_ap_init_stack(ap_count)) {
		kernel_panic(U"failed to reserve space for initial AP stack");
	}
	for(int i = 0; i < ap_count; i++) {
		print_str(U"AP booted\n");
	}
}

void __kernel_init(boot_info* bi_ptr) {
	init_gdt();
	init_idt();

	if(bi_ptr) {
		__kernel_bsp_init(bi_ptr);
	} else {
		init_apic(boot_inf.tsc_freq_hz);
	}
	atomic_fetch_add(&working_processor_count, 1);
	while(atomic_load(&working_processor_count) < ap_count+1) {
	}
}


uint16_t get_working_processor_count() {
	return atomic_load(&working_processor_count);
}
