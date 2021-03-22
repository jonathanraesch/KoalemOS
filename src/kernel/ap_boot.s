
.section .text

.global __ap_boot_get_cr3
__ap_boot_get_cr3:
	mov rax, cr3
	ret

.section .rodata

.global ap_boot_image
ap_boot_image:
.incbin "ap_boot.bin"
_ap_boot_image_end:

.global ap_boot_image_size
ap_boot_image_size:
.quad _ap_boot_image_end - ap_boot_image
