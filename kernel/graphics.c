#include "graphics.h"
#include "memory.h"
#include "../boot/paging_common.h"

typedef struct {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t _reserved;
} pixel_bgrx8u;

gop_framebuffer_info fb_info;


void init_graphics(gop_framebuffer_info* info) {
	if(PAGE_BASE(info) != PAGE_BASE(info+1)) {
		map_page(PAGE_BASE(info), PAGE_BASE(info), 0);
		map_page(PAGE_BASE(info+1), PAGE_BASE(info+1), 0);
		fb_info = *info;
		unmap_page(PAGE_BASE(info));
		unmap_page(PAGE_BASE(info+1));
	} else {
		map_page(PAGE_BASE(info), PAGE_BASE(info), 0);
		fb_info = *info;
		unmap_page(PAGE_BASE(info));
	}
	uint8_t* last = PAGE_BASE((uintptr_t)fb_info.addr + fb_info.hres*fb_info.vres*4);
	for(uint8_t* base = (uint8_t*)PAGE_BASE(fb_info.addr); base <= last; base += 0x1000) {
		map_page(base, base, PAGING_FLAG_READ_WRITE | PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE);
	}
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
