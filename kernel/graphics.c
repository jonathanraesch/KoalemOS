#include "graphics.h"
#include "../boot/graphics_common.h"


typedef struct {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t _reserved;
} pixel_bgrx8u;

gop_framebuffer_info fb_info;


void init_graphics(gop_framebuffer_info* info) {
	fb_info = *info;
	fill_screen(1.0, 1.0, 1.0);
}

void fill_screen(float red, float green, float blue) {
	pixel_bgrx8u col = {
		.red = red*255,
		.green = green*255,
		.blue = blue*255,
		._reserved = 0
	};
	pixel_bgrx8u* fb = (pixel_bgrx8u*)fb_info.addr;
	for(uint32_t y = 0; y < fb_info.vres; y++) {
		for(uint32_t x = 0; x < fb_info.width; x++) {
			fb[(uint64_t)y*fb_info.width + x] = col;
		}
	}
}
