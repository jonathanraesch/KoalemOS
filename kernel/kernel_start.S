section .text

global _kernel_start
_kernel_start:
kloop:
	jmp kloop