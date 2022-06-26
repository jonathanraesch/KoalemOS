
.section .text


.global __apic_enable
__apic_enable:
	mov ecx, 0x1B
	rdmsr
	or eax, 1<<11
	wrmsr
	shl rdx, 32
	or rax, rdx
	mov rdx, 0xFFFFFF000
	and rax, rdx
	ret


.global __apic_read_tsc
__apic_read_tsc:
	rdtsc
	lfence
	shl rdx, 32
	or rax, rdx
	ret
