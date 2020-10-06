#include "libc/string.h"
#include <stdint.h>


void* memcpy(void *restrict dest, const void *restrict src, size_t count) {
	size_t qword_count = count >> 3;
	size_t byte_count = count&7u;
	while(byte_count) {
		byte_count--;
		((uint8_t*)dest)[qword_count*8 + byte_count] = ((uint8_t*)src)[qword_count*8 + byte_count];
	}
	while(qword_count) {
		qword_count--;
		((uint64_t*)dest)[qword_count] = ((uint64_t*)src)[qword_count];
	}
	return dest;
}
