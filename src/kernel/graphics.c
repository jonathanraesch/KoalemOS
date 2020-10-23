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


static gop_framebuffer_info fb_info;

static pixel_bgrx8u bg_col = {.red=0, .green=0, .blue=0, ._reserved=0};
static pixel_bgrx8u fg_col = {.red=255, .green=255, .blue=255, ._reserved=0};
static pixel_bgrx8u bg_fg_lerp[256];

static FT_Library ft_library;
static FT_Face ft_face;

static uint32_t adv_x;
static uint32_t adv_y;
static uint32_t next_x = 0;
static uint32_t next_y = 0;

static uint32_t font_height;


static pixel_bgrx8u col_lerp(pixel_bgrx8u a, pixel_bgrx8u b, float t) {
	pixel_bgrx8u ret;
	ret.blue = ((float)b.blue - (float)a.blue)*t + a.blue;
	ret.green = ((float)b.green - (float)a.green)*t + a.green;
	ret.red = ((float)b.red - (float)a.red)*t + a.red;
	ret._reserved = 0;
	return ret;
}


static void init_freetype(int font_size) {
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


	FT_UInt glyph_index = FT_Get_Char_Index(ft_face, ' ');
	ft_error = FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
	if(ft_error) {
		kernel_panic();
	}

	ft_error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
	if(ft_error) {
		kernel_panic();
	}

	adv_x = ft_face->glyph->linearHoriAdvance/65535.0;
	adv_y = ft_face->glyph->linearVertAdvance/65535.0;

	font_height = font_size;
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

	for(int i = 0; i < 256; i++) {
		bg_fg_lerp[i] = col_lerp(bg_col, fg_col, i/255.0);
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
			pixel_bgrx8u col = bg_fg_lerp[ft_bm->buffer[y*ft_bm->pitch + x]];
			uint32_t y_val = next_y+font_height+y-ft_face->glyph->bitmap_top;
			uint32_t x_val = next_x+x+ft_face->glyph->bitmap_left;
			fb[y_val*fb_info.width + x_val] = col;
		}
	}

	next_x += adv_x;
	if(next_x + adv_x > fb_info.hres) {
		next_y += adv_y;
		next_x = 0;
	}
}