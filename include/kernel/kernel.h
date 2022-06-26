#pragma once
#include <stdint.h>

void kernel_panic(uint32_t* err_str);

uint16_t get_working_processor_count();
