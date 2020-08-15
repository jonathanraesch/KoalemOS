
// void get_pml4();
.global get_pml4
get_pml4:
	mov %cr3, %rax
	ret

// void boot_end()
.global boot_end
boot_end:
	mov %rdi, %cr3
	mov $0xFFFFFF0000000000, %rax
	jmp %rax
