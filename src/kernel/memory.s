
.section PHYS_MMAP, "wa", @nobits

.global _phys_mmap_range_buf
_phys_mmap_range_buf:
.skip 0x1000


.section KERNEL_HEAP, "wa", @nobits

.global kernel_heap_start
kernel_heap_start:
.skip 0x1000


.section .text

.global __invalidate_tlbs
__invalidate_tlbs:
	mov rax, __invld_tlbs_addr[rip]
	invlpg [rax]
	ret


.section .data

invalidate_tlbs_tar:
.8byte 0