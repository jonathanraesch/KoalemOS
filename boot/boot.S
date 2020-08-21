
// void get_pml4();
.global get_pml4
get_pml4:
	mov %cr3, %rax
	ret

// void __attribute__((noreturn)) boot_end(void* pml4, void* kernel_addr, efi_mmap_data* mmap_data);
.global boot_end
boot_end:
	mov %rdi, %cr3
	jmp *%rsi
