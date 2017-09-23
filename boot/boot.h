#pragma once

extern const char* str_error_no_multiboot;
extern const char* str_error_no_mmap;

void boot_print(const char* str);
_Noreturn void boot_error(const char* err_str);