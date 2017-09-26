section .bss
align 16
stack_bottom:
resb 32768
stack_top:


extern str_error_no_multiboot
extern str_error_no_mmap

section .text
bits 32

extern boot_error
extern mmap_find
extern mmap_sanitize
extern paging_set_up_boot_mapping
extern boot_set_up_gdt32
extern boot_set_up_gdt64

extern _kernel_start

global _boot_start
_boot_start:
	; disable nmi
	lea esp, [stack_top]
	mov ebp, esp
	sub esp, 8		;reserve space on stack
	; -04 MB_INFO addr
	; -08 MMAP addr
	mov [ebp-4], ebx

	push eax
	in al, 0x70
	or al, 0x80
	out 0x70, al
	pop eax

	cmp eax, 0x36d76289
	je mb_magic_check_success
	push dword [str_error_no_multiboot]
	call boot_error
	mb_magic_check_success:

	call boot_set_up_gdt32
	lgdt [eax]
	jmp (1 << 3):new_cseg_start
new_cseg_start:
	mov ax, (2 << 3)
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push dword [ebp-4]
	call mmap_find
	add esp, 4
	mov [ebp-8], eax

	push dword [ebp-8]
	call mmap_sanitize
	add esp, 4

	call paging_set_up_boot_mapping;
	mov cr3, eax

	call boot_set_up_gdt64
	mov ebx, eax

	mov eax, cr4
	or eax, (1 << 5)
	mov cr4, eax

	mov ecx, 0xc0000080
	rdmsr
	or eax, (1 << 8)
	wrmsr

	mov eax, cr0
	or eax, (1 << 31)
	mov cr0, eax

	jmp 0x18:boot_64_start
boot_64_start:
bits 64
	lgdt [ebx]
	jmp dword far [boot_64_farjump_target]
boot_64_farjump_target:
	dd boot_64_new_gdt
	dw 0x08
boot_64_new_gdt:
	jmp [kernel_start_addr]
kernel_start_addr:
dq _kernel_start

bits 32


global boot_print
; void boot_print(char* str)
boot_print:
	push ebp
	mov ebp, esp
	push ebx
	mov ebx, [ebp+8]
	mov ecx, 0
	mov ah, 0x04
	boot_print_loop:
	mov al, [ebx + ecx]
	cmp al, 0
	je boot_print_end
	mov [0xb8000 + 2*ecx], ax
	inc ecx
	jmp boot_print_loop
	boot_print_end:
	pop ebx
	pop ebp
	ret