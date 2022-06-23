
.section PHYS_MMAP, "wa", @nobits

.global _phys_mmap_range_buf
_phys_mmap_range_buf:
.skip 0x1000


.section KERNEL_HEAP, "wa", @nobits

.global kernel_heap_start
kernel_heap_start:
.skip 0x1000


.section .text

.global invalidate_tlbs_for
invalidate_tlbs_for:
	invlpg [rdi]
	mov invalidate_tlbs_tar[rip], rdi
	mov rax, 0
	mov al, __invld_tlbs_vec[rip]
	mov rdi, rax
	call broadcast_ipi
	ret


.global isr_invalidate_tlbs
isr_invalidate_tlbs:
	push rax
	lea rax, invalidate_tlbs_tar[rip]
	invlpg [rax]
	pop rax
	iretq


.section .data

invalidate_tlbs_tar:
.8byte 0