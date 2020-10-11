#include "kernel/graphics.h"
#include "kernel/memory.h"
#include "common/paging.h"
#include "kernel/fonts.h"
#include "kernel/kernel.h"
#include <ft2build.h>
#include FT_FREETYPE_H


typedef struct {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t _reserved;
} pixel_bgrx8u;

gop_framebuffer_info fb_info;


FT_Library ft_library;
FT_Face ft_face;


void init_freetype(int font_size) {
	FT_Error ft_error = FT_Init_FreeType(&ft_library);
	if(ft_error) {
		kernel_panic();
	}

	ft_error = FT_New_Memory_Face(ft_library, roboto_mono_ttf, roboto_mono_ttf_size, 0, &ft_face);
	if(ft_error) {
		kernel_panic();
	}

	ft_error = FT_Set_Pixel_Sizes(ft_face, 0, font_size);
	if(ft_error) {
		kernel_panic();
	}
}

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

	init_freetype(32);
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

void print_char(uint32_t ch) {
	FT_UInt glyph_index = FT_Get_Char_Index(ft_face, ch);

	FT_Error ft_error = FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
	if(ft_error) {
		kernel_panic();
	}

	ft_error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
	if(ft_error) {
		kernel_panic();
	}

	FT_Bitmap* ft_bm = &ft_face->glyph->bitmap;

	pixel_bgrx8u* fb = (pixel_bgrx8u*)fb_info.addr;
	for(uint32_t y = 0; y < ft_bm->rows; y++) {
		for(uint32_t x = 0; x < ft_bm->width; x++) {
			uint8_t val = 255 - ft_bm->buffer[y*ft_bm->pitch + x];
			pixel_bgrx8u col = {
				.red = val,
				.green = val,
				.blue = val,
				._reserved = 0
			};
			fb[(uint64_t)y*fb_info.width + x] = col;
		}
	}
}