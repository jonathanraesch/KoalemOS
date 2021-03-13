
.global get_pml4
get_pml4:
	mov %cr3, %rax
	ret

.global boot_end
boot_end:
	mov %rdi, %cr3
	jmp *%rsi


.global read_tsc
read_tsc:
	lfence
	rdtsc
	lfence
	shl $32, %rdx
	or %rdx, %rax
	ret
