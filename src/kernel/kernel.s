
.section .text

.global kernel_panic
kernel_panic:
	cli
	mov rbx, rdi

	lea rdi, panic_str_front[rip]
	call print_str

	mov rdi, rbx
	call print_str

	lea rdi, panic_str_back[rip]
	call print_str
	_kernel_panic_loop:
	hlt
	jmp _kernel_panic_loop


.section .rodata

panic_str_front:
.string32 "\n--- kernel panic: "
panic_str_back:
.string32 "! ---\n"
