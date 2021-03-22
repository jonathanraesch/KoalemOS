
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


	lea eax, gdt64[ebp]
	mov dword ptr gdtr64+2[ebp], eax

	lea eax, _lm_jmp_target[ebp]
	mov dword ptr lm_jmp[ebp], eax

	mov eax, cr4
	or eax, 0x20
	mov cr4, eax

	lea eax, pdpt[ebp]
	lock or dword ptr pml4[ebp], eax
	lea eax, pd[ebp]
	lock or dword ptr pdpt[ebp], eax
	lea eax, pml4[ebp]
	mov cr3, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 0x100
	wrmsr

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	lgdt gdtr64[ebp]
	ljmp lm_jmp[ebp]

.code64
	_lm_jmp_target:
	mov ax, 0
	mov ds, ax
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	lock inc word ptr ap_count_done[ebp]

	lea rsp, stack_bottom[ebp]

	mov rax, cr3_val[ebp]
	mov cr3, rax

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

gdt64:
.8byte 0
.8byte 0x209A0000000000
.balign 4
.2byte 0	# alignment padding
gdtr64:
.2byte 15
.4byte 0

.balign 0x1000
pml4:
.8byte 1 | 2
.skip 0xFF8
.balign 0x1000, 0
pdpt:
.8byte 1 | 2
.skip 0xFF8
.balign 0x1000, 0
pd:
.8byte 1 | 2 | 0x80
.skip 0xFF8

.balign 2
pm_jmp:
.4byte 0
.2byte 8

.balign 4
lm_jmp:
.4byte 0
.2byte 8

.balign 8
.skip 0x1000
stack_bottom:

cr3_val:
.8byte 0
ap_count_done:
.2byte 0
ap_count:
.2byte 0
