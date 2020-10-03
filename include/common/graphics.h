#pragma once
#include <stdint.h>

typedef struct {
	uint32_t hres;
	uint32_t vres;
	uint32_t width;
	void* addr;
} gop_framebuffer_info;
