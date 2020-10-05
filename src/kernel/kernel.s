
.section .text

.global kernel_panic
kernel_panic:
	cli
	mov %rbx, 0
	div %rbx
	_kernel_panic_loop:
	jmp _kernel_panic_loop
