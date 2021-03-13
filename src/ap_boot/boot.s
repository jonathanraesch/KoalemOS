
.section .text

.code16

_ap_bootstrap_start:
	cli
	mov ax, cs
	mov ds, ax
	
	lock inc word ptr [ap_count]

	_ap_bootstrap_end:
	jmp _ap_bootstrap_end


.section .data

ap_count:
.byte 0
