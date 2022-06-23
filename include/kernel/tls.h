#pragma once
#include <stdbool.h>
#include <stdint.h>

bool tls_reserve_ap_space(uint16_t ap_count);

bool tls_create();
void tls_done();
