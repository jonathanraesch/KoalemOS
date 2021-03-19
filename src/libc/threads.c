#include "libc/threads.h"


int mtx_init(mtx_t* mutex, int type) {
	if(type & mtx_recursive || type & mtx_timed) {
		return thrd_error;
	}
	*mutex = 0;
	return thrd_success;
}

int mtx_unlock(mtx_t *mutex) {
	*mutex = 0;
	return thrd_success;
}
