#include "kernel/kernel.h"
#include "kernel/graphics.h"
#include "kernel/acpi.h"
#include "kernel/pci.h"


#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library ftlibrary;


void kmain(gop_framebuffer_info* gop_fb_info, void* acpi_x_r_sdt) {
	init_graphics(gop_fb_info);
	fill_screen(1.0, 1.0, 1.0);
	init_acpi(acpi_x_r_sdt);
	init_pci();
	FT_Error fterror = FT_Init_FreeType(&ftlibrary);
	if(fterror) {
		kernel_panic();
	}
	while(1) {
		
	}
}
