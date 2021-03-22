
.section .text

# set bi_ptr to nullptr for APs
# _kernel_start(boot_info* bi_ptr);
.global _kernel_start
_kernel_start:
	cli

	mov rbx, rdi

	# initialize SSE
	mov rax, cr4
	or rax, 0x600
	mov cr4, rax
	mov rax, cr0
	and rax, 0xFFFFFFFFFFFFFFFB
	or rax, 0x2
	mov cr0, rax

	cmp rbx, 0
	je _kernel_start_tls_ap
	call __tls_create_bsp
	jmp _kernel_start_tls_done
	_kernel_start_tls_ap:
	call tls_create
	_kernel_start_tls_done:

	call __get_kernel_sp
	mov rsp, rax

	mov rdi, rbx
	call __kernel_init

	call kmain

kloop:
	jmp kloop
