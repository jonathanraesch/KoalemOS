#pragma once
#include <stdatomic.h>


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


typedef struct {
    _Alignas(128) _Atomic _Bool locked;
} mtx_t;


int mtx_init(mtx_t* mutex, int type);
int mtx_lock(mtx_t *mutex);
int mtx_trylock(mtx_t *mutex);
int mtx_unlock(mtx_t *mutex);
