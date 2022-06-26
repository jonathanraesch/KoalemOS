#pragma once
#include <stdint.h>
#include <stdbool.h>


void init_apic(uint64_t tsc_freq_hz);
void start_apic_timer(uint32_t count, uint8_t div_pow, bool periodic, void (*callback)());
void start_apic_timer_rt(double seconds, bool periodic, void (*callback)());
void stop_apic_timer();

void broadcast_ipi(uint8_t vec);
void send_init_sipi_sipi(uint8_t vec);
void inter_processor_call(void (*f)(void));
void apic_write_eoi();
