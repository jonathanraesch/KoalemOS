
section .data
kernel_gdt:
;	dq 0
;	dq 0x209A0000000000
resb 16
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

section .text

global _kernel_start
_kernel_start:
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

	; set up gdtr
	mov rax, rbx
	lea rbx, [rel kernel_gdtr]
	mov [rbx+2], rax
	mov ax, 16
	mov [rbx], ax

	;load gdt
	lea rax, [rel kernel_gdt_jmp_target]
	lgdt [rbx]
	jmp qword far [rax]
kernel_gdt_loaded:

	; load initial stack:
	mov rsp, stack_top

kloop:
	jmp kloop