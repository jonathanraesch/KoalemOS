#pragma once
#include <stddef.h>

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

long strtol(const char* restrict str, char** restrict str_end, int base);
