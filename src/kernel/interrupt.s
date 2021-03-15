
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


.global isr_divide_error
isr_divide_error:
	lea rdi, isr_errstr_divide_error[rip]
	call kernel_panic

.global isr_debug
isr_debug:
	iretq

.global isr_nmi
isr_nmi:
	lea rdi, isr_errstr_nmi[rip]
	call kernel_panic

.global isr_breakpoint
isr_breakpoint:
	iretq

.global isr_overflow
isr_overflow:
	lea rdi, isr_errstr_overflow[rip]
	call kernel_panic

.global isr_bound_range
isr_bound_range:
	lea rdi, isr_errstr_bound_range[rip]
	call kernel_panic

.global isr_invalid_opcode
isr_invalid_opcode:
	lea rdi, isr_errstr_invalid_opcode[rip]
	call kernel_panic

.global isr_no_coproc
isr_no_coproc:
	lea rdi, isr_errstr_no_coproc[rip]
	call kernel_panic

.global isr_double_fault
isr_double_fault:
	lea rdi, isr_errstr_double_fault[rip]
	call kernel_panic

.global isr_invalid_tss
isr_invalid_tss:
	lea rdi, isr_errstr_invalid_tss[rip]
	call kernel_panic

.global isr_segment_not_present
isr_segment_not_present:
	lea rdi, isr_errstr_segment_not_present[rip]
	call kernel_panic

.global isr_ss_fault
isr_ss_fault:
	lea rdi, isr_errstr_ss_fault[rip]
	call kernel_panic

.global isr_general_protection
isr_general_protection:
	lea rdi, isr_errstr_general_protection[rip]
	call kernel_panic

.global isr_page_fault
isr_page_fault:
	lea rdi, isr_errstr_page_fault[rip]
	call kernel_panic

.global isr_math_fault
isr_math_fault:
	lea rdi, isr_errstr_math_fault[rip]
	call kernel_panic

.global isr_align_check
isr_align_check:
	lea rdi, isr_errstr_align_check[rip]
	call kernel_panic

.global isr_machine_check
isr_machine_check:
	lea rdi, isr_errstr_machine_check[rip]
	call kernel_panic

.global isr_simd_exception
isr_simd_exception:
	lea rdi, isr_errstr_simd_exception[rip]
	call kernel_panic

.global isr_virtualization_exception
isr_virtualization_exception:
	lea rdi, isr_errstr_virtualization_exception[rip]
	call kernel_panic

.global isr_control_protection
isr_control_protection:
	lea rdi, isr_errstr_control_protection[rip]
	call kernel_panic


.section .rodata

isr_errstr_divide_error:
.string32 "divide error [#DE]"
isr_errstr_nmi:
.string32 "NMI"
isr_errstr_overflow:
.string32 "overflow trap [#OF] (this should never happen in long mode)"
isr_errstr_bound_range:
.string32 "BOUND range exceeded [#BR]"
isr_errstr_invalid_opcode:
.string32 "invalid opcode [#UD]"
isr_errstr_no_coproc:
.string32 "device not available [#NM] (no math coprocessor)"
isr_errstr_double_fault:
.string32 "double fault [#DF]"
isr_errstr_invalid_tss:
.string32 "invalid TSS [#TS]"
isr_errstr_segment_not_present:
.string32 "segment not present [#NP]"
isr_errstr_ss_fault:
.string32 "stack-segment fault [#SS]"
isr_errstr_general_protection:
.string32 "general protection [#GP]"
isr_errstr_page_fault:
.string32 "page fault [#PF]"
isr_errstr_math_fault:
.string32 "x87 FPU floating-point error [#MF]"
isr_errstr_align_check:
.string32 "alignment check [#AC]"
isr_errstr_machine_check:
.string32 "machine check [#MC]"
isr_errstr_simd_exception:
.string32 "SIMD floating-point exception [#XM]"
isr_errstr_virtualization_exception:
.string32 "virtualization exception [#VE]"
isr_errstr_control_protection:
.string32 "control protection exception [#CP]"
