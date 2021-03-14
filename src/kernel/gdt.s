
.section .text

.global __load_task_register
__load_task_register:
	ltr di
	ret

.global __load_gdt
__load_gdt:
	enter 20, 0
	
	mov [rbp-8], rdi
	mov [rbp-10], si
	lea rax, [rbp-10]

	lea rcx, __gdt_init_finish_ljmp[rip]
	mov [rbp-12], dx
	mov [rbp-20], rcx
	lea rcx, [rbp-20]

	lgdt [rax]
	rex.w ljmp [rcx]
	__gdt_init_finish_ljmp:
	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov gs, ax

	leave
	ret
	