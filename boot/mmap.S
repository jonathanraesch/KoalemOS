bits 32


; void mmap_swap_entries(void* mmap, uint32_t index_a, uint32_t index_b)
global mmap_swap_entries
mmap_swap_entries:
	push ebp
	mov ebp, esp
	mov ebx, [ebp+8]
	mov eax, [ebp+12]
	mul dword [ebx+8]
	mov ecx, eax
	mov eax, [ebp+16]
	mul dword [ebx+8]
	mov edx, [ebx+8]
	add ebx, 16
	mov esi, ebx
	add ebx, edx
	mmap_swap_entries_loop:
	sub ebx, 4
	mov edx, [ebx+eax]
	xchg edx, [ebx+ecx]
	mov [ebx+eax], edx
	cmp ebx, esi
	ja mmap_swap_entries_loop
	pop ebp
	ret