
.global get_pml4
get_pml4:
	mov %cr3, %rax
	ret

.global boot_end
boot_end:
	mov %rdi, %cr3
	jmp *%rsi
