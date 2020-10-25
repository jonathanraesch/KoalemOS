

.global longjmp
longjmp:
	mov rax, rsi
	jmp _setjmp_restore

.global setjmp
setjmp:
	mov [rdi+0x08], rsp
	mov rax, [rsp]
	mov [rdi+0x00], rax
	mov [rdi+0x10], rbp
	mov [rdi+0x18], rbx
	mov [rdi+0x20], r12
	mov [rdi+0x28], r13
	mov [rdi+0x30], r14
	mov [rdi+0x38], r15
	mov rax, 0

	_setjmp_restore:
	mov rsp, [rdi+0x08]
	mov rbx, [rdi+0x00]
	mov [rsp], rbx
	mov rbp, [rdi+0x10]
	mov rbx, [rdi+0x18]
	mov r12, [rdi+0x20]
	mov r13, [rdi+0x28]
	mov r14, [rdi+0x30]
	mov r15, [rdi+0x38]

	ret
