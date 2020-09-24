#pragma once
#include <stdint.h>
#include <stddef.h>

void* kmalloc(size_t size);
void kfree(void* ptr);

void map_page(void* vaddr, void* paddr, uint64_t flags);
void unmap_page(void* vaddr);

#define PAGE_BASE(X) ((void*)((uintptr_t)(X)&0xFFFFFFFFFFFFF000))
