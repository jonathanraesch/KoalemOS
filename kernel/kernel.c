#include "kernel.h"
#include "graphics.h"
#include "acpi.h"


void kmain(gop_framebuffer_info* gop_fb_info, void* acpi_x_r_sdt) {
	init_graphics(gop_fb_info);
	fill_screen(1.0, 1.0, 1.0);
	init_acpi(acpi_x_r_sdt);
	while(1) {
		
	}
}
