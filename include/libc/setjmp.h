#include <stdint.h>


typedef uint64_t jmp_buf[8];

int setjmp(jmp_buf env);
_Noreturn void longjmp(jmp_buf env, int status);
