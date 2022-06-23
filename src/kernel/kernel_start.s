
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
	je __ap_load_init_stack
	call __tls_create_bsp
	jmp _kernel_start_tls_done
	__ap_load_init_stack_done:
	call tls_create
	cmp rax, 0
	jne _kernel_start_tls_done
	call kernel_panic
	_kernel_start_tls_done:

	call __get_kernel_sp
	mov rsp, rax

	mov rdi, rbx
	call __kernel_init

	call kmain

kloop:
	jmp kloop


__ap_load_init_stack:
	mov rax, 1
	__ap_load_init_stack_mtx_loop:
	xchg __ap_init_stack_mutex[rip], rax
	cmp rax, 0
	je __ap_load_init_stack_mtx_done
	pause
	jmp __ap_load_init_stack_mtx_loop

	__ap_load_init_stack_mtx_done:
	mov rax, __ap_init_stack_ptr[rip]
	cmp rax, 0
	je __ap_load_init_stack_mtx_done
	cmp rax, __ap_init_stack_limit[rip]
	jge kernel_panic

	add rax, __ap_init_stack_size[rip]
	mov rsp, rax
	mov __ap_init_stack_ptr[rip], rax

	mov rax, 0
	mov __ap_init_stack_mutex[rip], rax
	jmp __ap_load_init_stack_done


.global set_ap_init_stack
set_ap_init_stack:
	mov __ap_init_stack_limit[rip], rsi
	mov __ap_init_stack_size[rip], rdx
	mov __ap_init_stack_ptr[rip], rdi
	ret


.section .data

__ap_init_stack_mutex:
.8byte 0
__ap_init_stack_size:
.8byte 0
__ap_init_stack_ptr:
.8byte 0
__ap_init_stack_limit:
.8byte 0