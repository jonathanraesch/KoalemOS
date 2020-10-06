#include "libc/string.h"
#include <stdint.h>


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
		size_t diff = (uintptr_t)dest-(uintptr_t)src;
		size_t rest = 0;
		switch(diff) {
			case 1:
				for(int i = count - 1; i >= 0; i--) {
					((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
				}
				break;
			case 2:
				for(int i = count/2 - 1; i >= 0; i--) {
					((uint16_t*)dest)[i] = ((uint16_t*)src)[i];
				}
				break;
			case 3:
				rest = 1;
				for(int i = count/2 - 1; i >= 0; i--) {
					((uint16_t*)((uintptr_t)dest + 1))[i] = ((uint16_t*)((uintptr_t)src + 1))[i];
				}
				break;
			case 4:
				for(int i = count/4 - 1; i >= 0; i--) {
					((uint32_t*)dest)[i] = ((uint32_t*)src)[i];
				}
				break;
			case 5:
				rest = 1;
				for(int i = count/4 - 1; i >= 0; i--) {
					((uint32_t*)((uintptr_t)dest + 1))[i] = ((uint32_t*)((uintptr_t)src + 1))[i];
				}
				break;
			case 6:
				rest = 2;
				for(int i = count/4 - 1; i >= 0; i--) {
					((uint32_t*)((uintptr_t)dest + 2))[i] = ((uint32_t*)((uintptr_t)src + 2))[i];
				}
				break;
			case 7:
				rest = 3;
				for(int i = count/4 - 1; i >= 0; i--) {
					((uint32_t*)((uintptr_t)dest + 3))[i] = ((uint32_t*)((uintptr_t)src + 3))[i];
				}
				break;
			default:
				rest = count%8;
				for(int i = count/8 - 1; i >= 0; i--) {
					((uint64_t*)((uintptr_t)dest + rest))[i] = ((uint64_t*)((uintptr_t)src + rest))[i];
				}
				break;
		}
		for(int i = rest-1; i >= 0; i--) {
			((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
		}
	} else if(src > dest) {
		size_t diff = (uintptr_t)src-(uintptr_t)dest;
		size_t rest = 0;
		switch(diff) {
			case 1:
				for(int i = 0; i < count; i++) {
					((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
				}
				break;
			case 2:
				for(int i = 0; i < count/2; i++) {
					((uint16_t*)dest)[i] = ((uint16_t*)src)[i];
				}
				break;
			case 3:
				rest = 1;
				for(int i = 0; i < count/2; i++) {
					((uint16_t*)dest)[i] = ((uint16_t*)src)[i];
				}
				break;
			case 4:
				for(int i = 0; i < count/4; i++) {
					((uint32_t*)dest)[i] = ((uint32_t*)src)[i];
				}
				break;
			case 5:
				rest = 1;
				for(int i = 0; i < count/4; i++) {
					((uint32_t*)dest)[i] = ((uint32_t*)src)[i];
				}
				break;
			case 6:
				rest = 2;
				for(int i = 0; i < count/4; i++) {
					((uint32_t*)dest)[i] = ((uint32_t*)src)[i];
				}
				break;
			case 7:
				rest = 3;
				for(int i = 0; i < count/4; i++) {
					((uint32_t*)dest)[i] = ((uint32_t*)src)[i];
				}
				break;
			default:
				rest = count%8;
				for(int i = 0; i < count/8; i++) {
					((uint64_t*)dest)[i] = ((uint64_t*)src)[i];
				}
				break;
		}
		for(int i = rest; i > 0; i--) {
			((uint8_t*)dest)[count-i] = ((uint8_t*)src)[count-i];
		}
	} else {
		return memcpy(dest, src, count);
	}
	return dest;
}
