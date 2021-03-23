
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


.global __isr_timer
__isr_timer:
	push rax
	mov rax, __apic_timer_callback[rip]
	call rax
	mov rax, __apic_reg_eoi[rip]
	mov DWORD PTR [rax], 0
	pop rax
	iretq


.global __isr_timer_rdtsc
__isr_timer_rdtsc:
	lfence
	rdtsc
	shl rdx, 32
	or rax, rdx
	mov __apic_tsc_end[rip], rax
	mov rax, __apic_reg_eoi[rip]
	mov DWORD PTR [rax], 0
	iretq

.global __apic_read_tsc
__apic_read_tsc:
	rdtsc
	lfence
	shl rdx, 32
	or rax, rdx
	ret
