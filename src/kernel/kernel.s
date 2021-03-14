
.section .text

.global kernel_panic
kernel_panic:
	cli
	lea rdi, panic_str[rip]
	call print_str
	_kernel_panic_loop:
	hlt
	jmp _kernel_panic_loop


.section .rodata

panic_str:
.string32 "\n--- kernel panic! ---\n"
