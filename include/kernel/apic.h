#pragma once
#include <stdint.h>
#include <stdbool.h>


void init_apic();
void start_apic_timer(uint32_t count, uint8_t div_pow, bool periodic, void (*callback)());
void stop_apic_timer();
