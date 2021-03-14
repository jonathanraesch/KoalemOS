
.section .data

kernel_gdt:
# GDT[0]: zero descriptor
.quad 0
# GDT[1]: code segment descriptor
.quad 0x209A0000000000
# GDT[2]: data segment descriptor
.quad 0x920000000000
# GDT[3]: TSS descriptor
.fill 16, 1, 0
kernel_gdt_end:
kernel_gdtr:
.short kernel_gdt_end-kernel_gdt-1
.quad kernel_gdt
kernel_gdt_jmp_target:
.quad kernel_gdt_loaded
.short 0x08

.align 8
kernel_tss:
.4byte 0
.8byte 0	# privilege level 0 rsp
.8byte 0	# privilege level 1 rsp
.8byte 0	# privilege level 2 rsp
.8byte 0
.8byte istack_0_bottom
.8byte istack_1_bottom
.8byte istack_2_bottom
.fill 74, 1, 0
.4byte 104


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

	# initialize TSS descriptor
	lea rbx, kernel_gdt+24[rip]
	mov word ptr [rbx], 103
	lea rdx, kernel_tss[rip]
	mov rax, 0xFFFFFF
	and rax, rdx
	mov [rbx+2], rax
	mov rax, 0xFFFFFFFFFF000000
	and rax, rdx
	shr rax, 24
	mov [rbx+7], rax
	mov ax, 0x89
	mov [rbx+5], ax

	# load gdt
	lea rax, kernel_gdt_jmp_target[rip]
	lea rbx, kernel_gdtr[rip]
	lgdt [rbx]
	# load CS
	rex.w ljmp [rax]
kernel_gdt_loaded:
	mov ax, 16
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	# load TSS
	mov ax, 24
	ltr ax

	call setup_idt
	sti

	mov rdi, [rbp-8]
	call init_memory_management

	call tls_create
	test rax, rax
	jnz tls_create_success
	call kernel_panic
tls_create_success:

	call kernel_post_init_check

	mov rdi, [rbp-16]
	mov rsi, [rbp-24]
	mov rdx, [rbp-32]
	call kmain

kloop:
	jmp kloop
