
section .data
kernel_gdt:
;	dq 0
;	dq 0x209A0000000000
;	dq 0x920000000000
resb 24
kernel_gdtr:
resb 10
;	dw 16
;	dq kernel_gdt
kernel_gdt_jmp_target:
resb 10
;	dq kernel_gdt_loaded
;	dw 0x08
stack_bottom:
resb 0x100000
stack_top:


extern setup_idt
extern init_graphics
extern init_memory_management
extern kernel_post_init_check

global _kernel_start

section .kernel_start_text
_kernel_start:
	cli

	; load initial stack:
	mov rsp, stack_top
	mov rbp, rsp
	push rdx ; efi_mmap_data	[rbp-8]
	push rcx ; fb_info			[rbp-16]

	; set up far jump target
	lea rbx, [rel kernel_gdt_jmp_target]
	lea rax, [rel kernel_gdt_loaded]
	mov [rbx], rax
	mov ax, 8
	mov [rbx+8], ax

	; set up gdt
	lea rbx, [rel kernel_gdt]
	xor rax, rax
	mov [rbx], rax
	mov rax, 0x209A0000000000
	mov [rbx+8], rax
	mov rax, 0x920000000000
	mov [rbx+16], rax

	; set up gdtr
	mov rax, rbx
	lea rbx, [rel kernel_gdtr]
	mov [rbx+2], rax
	mov ax, 23
	mov [rbx], ax

	; load gdt
	lea rax, [rel kernel_gdt_jmp_target]
	lgdt [rbx]
	; load CS
	jmp qword far [rax]
kernel_gdt_loaded:
	mov ax, 16
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call setup_idt
	sti

	mov rdi, [rbp-16]
	call init_graphics

	mov rdi, [rbp-8]
	call init_memory_management
	call kernel_post_init_check

kloop:
	jmp kloop
