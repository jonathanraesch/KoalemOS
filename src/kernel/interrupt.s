.section .bss

# interrupt handlers are currently limited to 1kiB of stack space
.global istack_0_bottom
.global istack_1_bottom
.global istack_2_bottom

.skip 1024
istack_2_bottom:

.skip 1024
istack_1_bottom:

.skip 1024
istack_0_bottom:


.section .text


.global get_cs
get_cs:
	mov rax, cs
	ret

.global load_idt
load_idt:
	nop
	enter 10, 0
	mov [rbp-8], rdi
	mov [rbp-10], si
	lidt [rbp-10]
	leave
	ret


.global isr_not_implemented
isr_not_implemented:
	call kernel_panic

.global isr_do_nothing
isr_do_nothing:
	nop
	iretq
