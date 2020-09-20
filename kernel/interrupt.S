section .text

extern kernel_panic

global load_idt
load_idt:
	enter 10, 0
	mov [rbp-8], rdi
	mov [rbp-10], rsi
	lidt [rbp-10]
	leave
	ret


global isr_not_implemented
isr_not_implemented:
	call kernel_panic

global isr_do_nothing
isr_do_nothing:
	iret
