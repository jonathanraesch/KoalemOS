#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


extern size_t ap_boot_image_size;

extern bool ap_boot_paddr_unset;
extern void* ap_boot_paddr;


uint16_t boot_aps();
