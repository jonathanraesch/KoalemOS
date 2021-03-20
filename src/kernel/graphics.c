#include "kernel/graphics.h"
#include "kernel/memory.h"
#include "common/paging.h"
#include "kernel/fonts.h"
#include "kernel/kernel.h"
#include <string.h>
#include <ctype.h>
#include <threads.h>
#include <ft2build.h>
#include FT_FREETYPE_H


#define GLYPH_CACHE_SIZE 100


typedef struct {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t _reserved;
} pixel_bgrx8u;

typedef struct {
	uint32_t ch;
	int priority;
	pixel_bgrx8u bitmap[];
} glyph_cache_entry;


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

static const int tab_size = 4;
static const int ff_size = 1;

static uint32_t font_height;

static void* glyph_cache;
static size_t glyph_cache_entry_size;

static mtx_t graphics_mutex;


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
		kernel_panic(U"FreeType error");
	}

	ft_error = FT_New_Memory_Face(ft_library, roboto_mono_ttf, roboto_mono_ttf_size, 0, &ft_face);
	if(ft_error) {
		kernel_panic(U"FreeType error");
	}

	ft_error = FT_Set_Pixel_Sizes(ft_face, 0, font_size);
	if(ft_error) {
		kernel_panic(U"FreeType error");
	}


	FT_UInt glyph_index = FT_Get_Char_Index(ft_face, ' ');
	ft_error = FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
	if(ft_error) {
		kernel_panic(U"FreeType error");
	}

	ft_error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
	if(ft_error) {
		kernel_panic(U"FreeType error");
	}

	adv_x = ft_face->glyph->linearHoriAdvance/65535.0;
	adv_y = ft_face->glyph->linearVertAdvance/65535.0;

	font_height = font_size;

	glyph_cache_entry_size = sizeof(glyph_cache_entry) + adv_x*adv_y*sizeof(pixel_bgrx8u);
	glyph_cache = kmalloc(GLYPH_CACHE_SIZE*glyph_cache_entry_size);
	memset(glyph_cache, 0, GLYPH_CACHE_SIZE*glyph_cache_entry_size);
}

void init_graphics(gop_framebuffer_info* info, int font_size) {
	if(mtx_init(&graphics_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	fb_info = *info;
	uint8_t* last = PAGE_BASE((uintptr_t)fb_info.addr + fb_info.hres*fb_info.vres*4);
	for(uint8_t* base = (uint8_t*)PAGE_BASE(fb_info.addr); base <= last; base += 0x1000) {
		map_page(base, base, PAGING_FLAG_READ_WRITE | PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE);
	}

	for(int i = 0; i < 256; i++) {
		bg_fg_lerp[i] = col_lerp(bg_col, fg_col, i/255.0);
	}

	init_freetype(font_size);
}


void fill_screen(float red, float green, float blue) {
	if(mtx_lock(&graphics_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
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
	if(mtx_unlock(&graphics_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}

static pixel_bgrx8u* get_glyph(uint32_t ch) {

	static uint64_t cur_priority = 0;
	cur_priority++;
	// rollover would take many years of continuous operation to occur
	// checking for it anyway...
	if(cur_priority == 0) {
		cur_priority = 1;
		for(int i = 0; i < GLYPH_CACHE_SIZE; i++) {
			glyph_cache_entry* gc_entry = (glyph_cache_entry*)((uintptr_t)glyph_cache + glyph_cache_entry_size*i);
			gc_entry->priority = 0;
		}
	}

	int gc_index = 0;	// should always be assigned in loop, initializing here avoids a compiler warning and acts as a failsafe
	int priority = cur_priority;
	for(int i = 0; i < GLYPH_CACHE_SIZE; i++) {
		glyph_cache_entry* gc_entry = (glyph_cache_entry*)((uintptr_t)glyph_cache + glyph_cache_entry_size*i);

		if(gc_entry->ch == ch) {
			gc_entry->priority = cur_priority;
			return gc_entry->bitmap;
		}
		if(gc_entry->priority < priority) {
			gc_index = i;
			priority = gc_entry->priority;
		}
	}

	glyph_cache_entry* gc_entry = (glyph_cache_entry*)((uintptr_t)glyph_cache + glyph_cache_entry_size*gc_index);
	gc_entry->priority = cur_priority;
	gc_entry->ch = ch;
	for(uint32_t y = 0; y < adv_y; y++) {
		for(uint32_t x = 0; x < adv_x; x++) {
			gc_entry->bitmap[y*adv_x + x] = bg_col;
		}
	}


	FT_UInt glyph_index = FT_Get_Char_Index(ft_face, ch);
	FT_Error ft_error = FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
	if(ft_error) {
		kernel_panic(U"FreeType error");
	}

	ft_error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
	if(ft_error) {
		kernel_panic(U"FreeType error");
	}

	FT_Bitmap* ft_bm = &ft_face->glyph->bitmap;
	for(uint32_t y = 0; y < ft_bm->rows; y++) {
		for(uint32_t x = 0; x < ft_bm->width; x++) {
			pixel_bgrx8u col = bg_fg_lerp[ft_bm->buffer[y*ft_bm->pitch + x]];
			uint32_t y_val = font_height+y-ft_face->glyph->bitmap_top;
			uint32_t x_val = x+ft_face->glyph->bitmap_left;
			gc_entry->bitmap[y_val*adv_x + x_val] = col;
		}
	}

	return gc_entry->bitmap;
}

static inline void check_charpos() {
	if(next_x + adv_x > fb_info.hres) {
		next_y += adv_y;
		next_x = 0;
		if(next_y + adv_y > fb_info.vres) {
			next_y = 0;
			fill_screen(bg_col.red, bg_col.green, bg_col.blue);
		}
	}
}

static void __print_char(uint32_t ch) {
	if(isspace(ch)) {
		switch(ch) {
			case '\n':
				next_y += adv_y;
				next_x = 0;
			case '\r':
				next_x = 0;
				break;
			case '\f':
				next_y += adv_y*ff_size;
				break;
			case '\t':
				next_x += adv_x*tab_size;
				break;
			case '\v':
				next_y += adv_y;
				break;
			case ' ':
			default:
				next_x += adv_x;
				break;
		}
		check_charpos();
		return;
	}

	pixel_bgrx8u* bm = get_glyph(ch);

	pixel_bgrx8u* fb = (pixel_bgrx8u*)fb_info.addr;
	for(uint32_t y = 0; y < adv_y; y++) {
		memcpy(fb + (next_y + y)*fb_info.width + next_x, bm + y*adv_x, adv_x*sizeof(pixel_bgrx8u));
	}

	next_x += adv_x;
	check_charpos();
}

void print_char(uint32_t ch) {
	if(mtx_lock(&graphics_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	__print_char(ch);
	if(mtx_unlock(&graphics_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}

void print_str(uint32_t* str) {
	if(mtx_lock(&graphics_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	while(*str) {
		__print_char(*str++);
	}
	if(mtx_unlock(&graphics_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}
