section .text

global _kernel_start
_kernel_start:
	xchg bx, bx
kloop:
	jmp kloop