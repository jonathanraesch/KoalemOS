#pragma once
#include "common/mmap.h"
#include "common/graphics.h"
#include <stdint.h>


typedef struct {
	efi_mmap_data mmap_data;
	gop_framebuffer_info gop_fb_info;
	void* acpi_x_r_sdt;
	uint64_t tsc_freq_hz;
} boot_info;
