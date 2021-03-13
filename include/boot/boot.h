#pragma once
#include "common/mmap.h"


void* get_pml4();
void __attribute__((noreturn)) boot_end(void* pml4, void* kernel_addr, efi_mmap_data* mmap_data, gop_framebuffer_info* fb_info, void* acpi_x_r_sdt, uint64_t tsc_freq_hz);
