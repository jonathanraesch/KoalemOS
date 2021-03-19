#pragma once
#include <stdint.h>


enum {
    thrd_success,
    thrd_nomem,
    thrd_timedout,
    thrd_busy,
    thrd_error
};

enum {
    mtx_plain,
    mtx_recursive,
    mtx_timed
};


typedef volatile uint64_t mtx_t;


int mtx_init(mtx_t* mutex, int type);
int mtx_lock(mtx_t *mutex);
int mtx_unlock(mtx_t *mutex);
