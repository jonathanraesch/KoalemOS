#include "kernel/kernel.h"
#include "kernel/graphics.h"
#include "kernel/acpi.h"
#include "kernel/pci.h"
#include "kernel/apic.h"


void timer_tick() {
	print_char('T');
}


void kmain(gop_framebuffer_info* gop_fb_info, void* acpi_x_r_sdt, uint64_t tsc_freq_hz) {
	init_graphics(gop_fb_info, 20);
	fill_screen(0.0, 0.0, 0.0);
	init_acpi(acpi_x_r_sdt);
	if(!init_pci()) {
		kernel_panic();
	}
	init_apic(tsc_freq_hz);

	start_apic_timer_rt(1.0, true, timer_tick);

	while(1) {

	}
}
