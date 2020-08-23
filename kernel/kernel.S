
section .text

global kernel_panic
kernel_panic:
	cli
	_kernel_panic_loop:
	jmp _kernel_panic_loop
