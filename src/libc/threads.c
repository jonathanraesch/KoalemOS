#include "libc/threads.h"
#include <stdbool.h>


int mtx_init(mtx_t* mutex, int type) {
	if(type & mtx_recursive || type & mtx_timed) {
		return thrd_error;
	}
	mutex->locked = false;
	return thrd_success;
}

int mtx_lock(mtx_t *mutex) {
	while(true) {
		if(!atomic_load(&(mutex->locked))) {
			if(!atomic_exchange(&(mutex->locked), true)) {
				break;
			}
		}
		__builtin_ia32_pause();
	}
	return thrd_success;
}

int mtx_trylock(mtx_t *mutex) {
	if(atomic_exchange(&(mutex->locked), true)) {
		return thrd_busy;
	}
	return thrd_success;
}

int mtx_unlock(mtx_t *mutex) {
	atomic_store(&(mutex->locked), false);
	return thrd_success;
}
