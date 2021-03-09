
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


.global isr_timer
isr_timer:
	nop
	mov rax, __apic_timer_callback[rip]
	call rax
	mov rax, __apic_reg_eoi[rip]
	mov DWORD PTR [rax], 0
	iretq
