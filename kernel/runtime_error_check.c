#include "runtime_error_check.h"
#include "memory.h"


void test_kernel_heap() {
	void* a = kmalloc(100);
	void* b = kmalloc(0x2000);
	void* c = kmalloc(5);

	kfree(b);
	kfree(a);
	kfree(c);
}

void kernel_post_init_check() {
	test_kernel_heap();
}
