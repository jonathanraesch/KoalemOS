#include "libc/stdlib.h"


#ifdef __klibc__
#include "kernel/memory.h"

	void* malloc(size_t size) {
		return kmalloc(size);
	}
	void free(void* ptr) {
		kfree(ptr);
	}

#endif