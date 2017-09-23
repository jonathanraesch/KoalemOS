#include <stdbool.h>
#include "boot.h"

const char* str_error_no_multiboot = "Error: Bootloader not Multiboot2-compliant";
const char* str_error_no_mmap = "Error: Memory Map not supported";

_Noreturn void boot_error(const char* err_str) {
	boot_print(err_str);
	asm volatile ("cli");
	while(true) {
		asm volatile (
			"xchg bx, bx\n\t"
			"mov eax, 0xefefefef\n\t"
			"hlt"
			);
	}
}