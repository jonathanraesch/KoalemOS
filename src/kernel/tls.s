
.section .text

.global get_fsbase
get_fsbase:
	mov ecx, 0xC0000100
	rdmsr
	shl rdx, 32
	or rax, rdx
	ret

.global set_fsbase
set_fsbase:
	mov rax, rdi
	mov rdx, rax
	shr rdx, 32
	mov ecx, 0xC0000100
	wrmsr
	ret
