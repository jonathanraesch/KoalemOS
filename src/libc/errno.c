#include "libc/errno.h"


#ifdef __klibc__

_Thread_local int __errno = 0;

#endif
