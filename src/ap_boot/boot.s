
.section .text

.code16

_ap_bootstrap_start:
	cli
	mov ax, cs
	mov ds, ax
	# base address in di:si
	mov dx, 0x10
	mul dx
	mov di, dx
	mov si, ax

	lock inc word ptr [ap_count]

	lea ax, [gdt32]
	mov bx, 0
	add ax, si
	adc bx, di
	mov word ptr [gdtr32+2], ax
	mov word ptr [gdtr32+4], bx
	lgdt [gdtr32]

	lea ax, [_pm_jmp_target]
	mov bx, 0
	add ax, si
	adc bx, di
	mov word ptr [pm_jmp], ax
	mov word ptr [pm_jmp+2], bx

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	data32 ljmp [pm_jmp]

.code32
	_pm_jmp_target:
	mov ax, 16
	mov ds, ax
	mov ss, ax
	mov ax, 0
	mov es, ax
	mov fs, ax
	mov gs, ax

	# base address in ebp
	mov bp, di
	shl ebp, 16
	mov bp, si

	lock inc word ptr ap_count_done[bp]

	_ap_bootstrap_end:
	jmp _ap_bootstrap_end


.section .data

gdt32:
.8byte 0
.8byte 0xCF9A000000FFFF		# CS
.8byte 0xCF92000000FFFF		# DS
.balign 2
gdtr32:
.2byte 23
.4byte 0

.balign 2
pm_jmp:
.4byte 0
.2byte 8

ap_count_done:
.byte 0
ap_count:
.byte 0
