
.section .data
kernel_gdt:
.quad 0
.quad 0x209A0000000000
.quad 0x920000000000
kernel_gdtr:
.short 23
.quad kernel_gdt
kernel_gdt_jmp_target:
.quad kernel_gdt_loaded
.short 0x08

.section .bss
stack_bottom:
.skip 0x100000
stack_top:


.global _kernel_start

.section .text

_kernel_start:
	cli

	# load initial stack:
	lea %rsp, stack_top[%rip]
	mov %rbp, %rsp
	push %rdx	#	efi_mmap_data	[%rbp-8]
	push %rcx	#	fb_info			[%rbp-16]
	push %r8	#	acpi_x_r_sdt	[%rbp-24]

	# load gdt
	lea %rax, kernel_gdt_jmp_target[%rip]
	lea %rbx, kernel_gdtr[%rip]
	lgdt [%rbx]
	# load CS
	rex.w ljmp [%rax]
kernel_gdt_loaded:
	mov %ax, 16
	mov %ss, %ax
	mov %ds, %ax
	mov %es, %ax
	mov %fs, %ax
	mov %gs, %ax

	call setup_idt
	sti

	mov %rdi, [%rbp-8]
	call init_memory_management

	call kernel_post_init_check

	mov %rdi, [%rbp-16]
	mov %rsi, [%rbp-24]
	call kmain

kloop:
	jmp kloop
