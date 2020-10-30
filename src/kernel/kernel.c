#include "kernel/kernel.h"
#include "kernel/graphics.h"
#include "kernel/acpi.h"
#include "kernel/pci.h"


void kmain(gop_framebuffer_info* gop_fb_info, void* acpi_x_r_sdt) {
	init_graphics(gop_fb_info, 20);
	fill_screen(0.0, 0.0, 0.0);
	init_acpi(acpi_x_r_sdt);
	init_pci();

	print_char('X');

	while(1) {
		
	}
}
