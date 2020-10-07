#include "libc/string.h"
#include <stdint.h>
#include <stdbool.h>


size_t strlen(const char* str) {
	size_t len = 0;
	while(str[len]) {
		len++;
	}
	return len;
}

int strcmp(const char* lhs, const char* rhs) {
	size_t i = 0;
	while(lhs[i] == rhs[i]) {
		if(!lhs[i]) {
			return 0;
		}
		i++;
	}
	return (int)lhs[i] - (int)rhs[i];
}

int strncmp(const char* lhs, const char* rhs, size_t count) {
	for(size_t i = 0; i < count; i++) {
		if(lhs[i] != rhs[i]) {
			return (int)lhs[i] - (int)rhs[i];
		}
		if(!lhs[i]) {
			return 0;
		}
	}
	return 0;
}

char* strchr(const char* str, int ch) {
	while(*str) {
		if((unsigned char)*str == (char)ch) {
			return (char*)str;
		}
	}
	if((char)ch) {
		return 0;
	}
	return (char*)str;
}

char* strrchr(const char* str, int ch) {
	char* ret = 0;
	while(*str) {
		if((unsigned char)*str == (char)ch) {
			ret = (char*)str;
		}
		str++;
	}
	if(!(char)ch && !*str) {
		return (char*)str;
	}
	return ret;
}

char* strstr(const char* str, const char* substr) {
	size_t substr_len = strlen(substr);
	if(substr_len == 0) {
		return (char*)str;
	}

	while(*str) {
		if(*str == *substr) {
			size_t i = 1;
			bool match = true;
			for(; i < substr_len; i++) {
				if(str[i] != substr[i]) {
					match = false;
					break;
				}
			}
			if(match) {
				return (char*)str;
			}
			str += i;
		} else {
			str++;
		}
	}
	return 0;
}


int memcmp(const void* lhs, const void* rhs, size_t count) {
	size_t qword_count = count >> 3;
	int last_qword = 0;
	for(; last_qword < qword_count; last_qword++) {
		if(((uint64_t*)lhs)[last_qword] != ((uint64_t*)rhs)[last_qword]) {
			break;
		}
	}
	for(int i = last_qword << 3; i < count; i++) {
		int diff = (int)((uint8_t*)lhs)[i] - (int)((uint8_t*)rhs)[i];
		if(diff) {
			return diff;
		}
	}
	return 0;
}

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
