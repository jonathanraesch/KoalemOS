section .text

; void get_pml4();
global get_pml4
get_pml4:
	mov rax, cr3
	ret

; void boot_end()
global boot_end
boot_end:
	jmp [kernel_start_addr]
kernel_start_addr:
dq 0xFFFFFF0000000000
