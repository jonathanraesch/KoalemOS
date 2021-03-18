
.section .bss

efi_mmap_data:
.8byte 0
fb_info:
.8byte 0
acpi_x_r_sdt:
.8byte 0
tsc_freq_hz:
.8byte 0


.section .text

# set bi_ptr to nullptr for APs
# _kernel_start(boot_info* bi_ptr);
.global _kernel_start
_kernel_start:
	cli

	mov rbx, rdi

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
