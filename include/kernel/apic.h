#pragma once
#include <stdint.h>
#include <stdbool.h>


void init_apic(uint64_t tsc_freq_hz);
void start_apic_timer(uint32_t count, uint8_t div_pow, bool periodic, void (*callback)());
void start_apic_timer_rt(double seconds, bool periodic, void (*callback)());
void stop_apic_timer();

void send_init_sipi_sipi(uint8_t vec);