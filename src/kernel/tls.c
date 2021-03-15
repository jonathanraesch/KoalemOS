#include "kernel/tls.h"
#include "kernel/memory.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>


extern const uint8_t tls_image[];
extern void* __tls_image_size_sym;
const size_t tls_image_size = (size_t)&__tls_image_size_sym;
extern uint8_t __bsp_tls_buf[];

extern void* get_fsbase();
extern void set_fsbase(void*);


void __tls_create_bsp() {
	uint8_t* ptr = __bsp_tls_buf;
	*(uint64_t*)ptr = (uint64_t)&ptr[tls_image_size+8];
	// using a loop instead of memcpy limits stack usage
	for(int i = 0; i < tls_image_size; i++) {
		ptr[i+8] = tls_image[i];
	}
	set_fsbase(ptr);
}

bool tls_create() {
	uint8_t* ptr = kmalloc(tls_image_size + 8);
	if(!ptr) {
		return false;
	}
	*(uint64_t*)ptr = (uint64_t)&ptr[tls_image_size+8];
	memcpy(ptr+8, tls_image, tls_image_size);
	set_fsbase(ptr);
	return true;
}

void tls_done() {
	kfree(get_fsbase());
	set_fsbase(0);
}
