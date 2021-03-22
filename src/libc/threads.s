
.section .text

.global mtx_lock
mtx_lock:
	mov rcx, 1
	mtx_lock_loop:
	mov rax, 0
	lock cmpxchg [rdi], rcx
	je mtx_lock_done
	pause
	jmp mtx_lock_loop
	mtx_lock_done:
	mov rax, 0
	ret
