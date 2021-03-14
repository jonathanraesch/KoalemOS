#include "kernel/kernel.h"
#include "kernel/graphics.h"
#include "kernel/acpi.h"
#include "kernel/pci.h"
#include "kernel/apic.h"
#include "kernel/ap_boot.h"


void kmain(gop_framebuffer_info* gop_fb_info, void* acpi_x_r_sdt, uint64_t tsc_freq_hz) {
	init_graphics(gop_fb_info, 20);
	fill_screen(0.0, 0.0, 0.0);
	init_acpi(acpi_x_r_sdt);
	if(!init_pci()) {
		kernel_panic(U"failed to initialize PCI");
	}
	init_apic(tsc_freq_hz);

	boot_aps();

	volatile uint8_t* v = (uint8_t*)(ap_boot_image_size-1);
	uint8_t cpus_counted = 0;
	while(1) {
		if(*v > cpus_counted) {
			print_char('C');
			cpus_counted++;
		}
	}
}
