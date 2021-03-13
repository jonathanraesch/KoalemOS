#pragma once
#include <stdbool.h>
#include <stddef.h>


extern size_t ap_boot_image_size;

extern bool ap_boot_paddr_unset;
extern void* ap_boot_paddr;


void boot_aps();
