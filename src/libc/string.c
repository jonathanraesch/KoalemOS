#include "libc/string.h"
#include <stdint.h>


void* memset(void* dest, int ch, size_t count) {
	size_t qword_count = count >> 3;
	size_t byte_count = count&7u;
	uint64_t big_val = (uint8_t)ch;
	big_val = big_val<<8 | big_val;
	big_val = big_val<<16 | big_val;
	big_val = big_val<<32 | big_val;
	while(byte_count) {
		byte_count--;
		((uint8_t*)dest)[qword_count*8 + byte_count] = (uint8_t)ch;
	}
	while(qword_count) {
		qword_count--;
		((uint64_t*)dest)[qword_count] = big_val;
	}
	return dest;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t count) {
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

void* memmove(void* dest, const void* src, size_t count) {
	if (dest > src) {
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
	} else if(src > dest) {
		size_t qword_count = count >> 3;
		size_t byte_count = count&7u;
		for(size_t i = 0; i < qword_count; i++) {
			((uint64_t*)dest)[i] = ((uint64_t*)src)[i];
		}
		for(size_t i = 0; i < byte_count; i++) {
			((uint8_t*)dest)[qword_count*8 + i] = ((uint8_t*)src)[qword_count*8 + i];
		}
	}
	return dest;
}
