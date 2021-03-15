#include <stdint.h>


static _Thread_local uint8_t kernel_stack[0x2000];


void* __get_kernel_sp() {
	return &kernel_stack[0x2000];
}
