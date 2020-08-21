#include "kernel.h"


void kernel_panic() {
	asm ("cli");
	while(0) {}
}
