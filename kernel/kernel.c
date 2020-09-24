#include "kernel.h"
#include "graphics.h"


void kmain(gop_framebuffer_info* gop_fb_info) {
	init_graphics(gop_fb_info);
	fill_screen(1.0, 1.0, 1.0);
	while(1) {
		
	}
}
