
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
	invlpg [%rdi]
	ret
