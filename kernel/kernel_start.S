section .text

global _kernel_start
_kernel_start:
	mov ah, 0x04
	mov al, 0x58
	mov rcx, 0xFFFF8000000b8000
	mov [rcx], ax
	; xchg bx, bx
kloop:
	jmp kloop