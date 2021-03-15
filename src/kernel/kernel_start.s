
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

.global _kernel_start
_kernel_start:
	cli

	mov efi_mmap_data[rip], rdx
	mov fb_info[rip], rcx
	mov acpi_x_r_sdt[rip], r8
	mov tsc_freq_hz[rip], r9

	call __tls_create_bsp

	call __get_kernel_sp
	mov rsp, rax

	call init_gdt

	call setup_idt
	sti

	mov rdi, efi_mmap_data[rip]
	call init_memory_management
	call kernel_post_init_check

	mov rdi, fb_info[rip]
	mov rsi, acpi_x_r_sdt[rip]
	mov rdx, tsc_freq_hz[rip]
	call kmain

kloop:
	jmp kloop
