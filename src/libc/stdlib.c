#include "libc/stdlib.h"


#ifdef __klibc__

#include "kernel/memory.h"
#include "libc/ctype.h"
#include "libc/errno.h"
#include <stdbool.h>
#include <limits.h>


#define DIG_IN_BASE(C, B) (		\
	(B)>10 ?	\
	isdigit((C)) || ((C)>='a' && (C)<'a'+(B)-10) || ((C)>='A' && (C)<'A'+(B)-10) :	\
	((C)>='0' && (C)<'0'+(B))	\
)

long strtol(const char* restrict str, char** restrict str_end, int base) {
	if(base > 36 || base < 0 || base == 1) {
		errno = EINVAL;
		return 0;
	}

	const char* cur = str;
	while(isspace(*cur)) {
		cur++;
	}

	bool neg = false;
	if(*cur == '-') {
		neg = true;
		cur++;
	}

	if(*cur == '0') {
		if(cur[1] == 'x' && (base == 16 || base == 0)) {
			if(isalnum(cur[2])) {
				base = 16;
				cur += 2;
			}
		} else if((base == 8 || base == 0) && isalnum(cur[1])) {
			base = 8;
			cur++;
		}
	}
	if(base == 0) {
		base = 10;
	}

	long ret = 0;
	while(DIG_IN_BASE(*cur, base)) {
		long d;
		if(isdigit(*cur)) {
			d = *cur - '0';
		} else if(*cur >= 'a' && *cur <= 'z') {
			d = 10 + *cur - 'a';
		} else {
			d = 10 + *cur - 'A';
		}
		if(neg) {
			if(ret < LONG_MIN/base + d) {
				errno = ERANGE;
				ret = LONG_MIN;
				while(DIG_IN_BASE(*(++cur), base));
				break;
			}
			ret = ret*base - d;
		} else {
			if(ret > LONG_MAX/base - d) {
				errno = ERANGE;
				ret = LONG_MAX;
				while(DIG_IN_BASE(*(++cur), base));
				break;
			}
			ret = ret*base + d;
		}
		cur++;
	}

	if(str_end) {
		*str_end = (char*)cur;
	}
	return ret;
}


void* malloc(size_t size) {
	return kmalloc(size);
}

void* realloc(void* ptr, size_t size) {
	return krealloc(ptr, size);
}

void free(void* ptr) {
	kfree(ptr);
}


#endif

#include <stdint.h>


void memswap(void* lhs, void* rhs, size_t count) {
	for(size_t i = 0; i < count; i++) {
		uint8_t tmp = ((uint8_t*)lhs)[i];
		((uint8_t*)lhs)[i] = ((uint8_t*)rhs)[i];
		((uint8_t*)rhs)[i] = tmp;
	}
}

static size_t partition(void* ptr, size_t count, size_t size, int (*comp)(const void *, const void *), size_t piv_index) {
	memswap(ptr, (void*)((uintptr_t)ptr + piv_index), size);
	size_t i = 1;
	size_t j = count-1;
	while(i<j) {
		if(comp((void*)((uintptr_t)ptr + i), ptr) <= 0) {
			i++;
			continue;
		}
		if(comp((void*)((uintptr_t)ptr + j), ptr) > 0) {
			j--;
			continue;
		}
		memswap((void*)((uintptr_t)ptr + i), (void*)((uintptr_t)ptr + j), size);
		i++;
		j--;
	}
	if(i > 1) {
		if(comp((void*)((uintptr_t)ptr + i), ptr) > 0) {
			memswap(ptr, (void*)((uintptr_t)ptr + i-1), size);
			return i-1;
		} else {
			memswap(ptr, (void*)((uintptr_t)ptr + i), size);
			return i;
		}
	}
	return 0;
}

void qsort(void* ptr, size_t count, size_t size, int (*comp)(const void *, const void *)) {
	if(count <= 2) {
		if(count < 2) {
			return;
		}
		if(comp((void*)((uintptr_t)ptr + 1), ptr) < 0) {
			memswap(ptr, (void*)((uintptr_t)ptr + 1), size);
		}
		return;
	}
	size_t piv_index = size/2;
	piv_index = partition(ptr, count, size, comp, piv_index);
	qsort(ptr, piv_index, size, comp);
	qsort((void*)((uintptr_t)ptr + piv_index + 1), count-piv_index-1, size, comp);
}
