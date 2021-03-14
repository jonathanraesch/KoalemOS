
.section .bss

.skip 0x100000
stack_bottom:


.global _kernel_start

.section .text

_kernel_start:
	cli

	# load initial stack:
	lea rsp, stack_bottom[rip]
	mov rbp, rsp
	push rdx	#	efi_mmap_data	[rbp-8]
	push rcx	#	fb_info			[rbp-16]
	push r8		#	acpi_x_r_sdt	[rbp-24]
	push r9		#	tsc_freq_hz		[rbp-32]

	call __tls_create_bsp

	call init_gdt

	call setup_idt
	sti

	mov rdi, [rbp-8]
	call init_memory_management
	call kernel_post_init_check

	mov rdi, [rbp-16]
	mov rsi, [rbp-24]
	mov rdx, [rbp-32]
	call kmain

kloop:
	jmp kloop
