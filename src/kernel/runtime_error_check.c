#include "kernel/runtime_error_check.h"
#include "kernel/memory.h"


static bool test_kernel_heap() {
	void* a = kmalloc(100);
	if(!heap_consistency_check()) {
		return false;
	}

	void* b = kmalloc(0x2000);
	if(!heap_consistency_check()) {
		return false;
	}
	void* c = kmalloc(5);
	if(!heap_consistency_check()) {
		return false;
	}

	kfree(b);
	if(!heap_consistency_check()) {
		return false;
	}
	kfree(a);
	if(!heap_consistency_check()) {
		return false;
	}
	kfree(c);
	if(!heap_consistency_check()) {
		return false;
	}

	return true;
}

bool kernel_post_init_check() {
	return test_kernel_heap();
}
