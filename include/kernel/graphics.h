#pragma once
#include "common/graphics.h"
#include <stdint.h>


void init_graphics(gop_framebuffer_info* info, int font_size);

void fill_screen(float red, float green, float blue);
void print_char(uint32_t ch);
void print_str(uint32_t* str);
