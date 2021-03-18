
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

# _kernel_start(boot_info*);
.global _kernel_start
_kernel_start:
	cli

	mov rbx, rdi

	call __tls_create_bsp

	call __get_kernel_sp
	mov rsp, rax

	mov rdi, rbx
	call __kernel_init

	call kmain

kloop:
	jmp kloop
