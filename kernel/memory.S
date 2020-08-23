
section .text

global invalidate_tlbs_for
invalidate_tlbs_for:
	invlpg [rdi]
