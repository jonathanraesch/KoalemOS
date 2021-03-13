#pragma once
#include "common/paging.h"
#include <stdint.h>
#include <stddef.h>

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

void* alloc_virt_pages(uint64_t pages);
int free_virt_pages(void* base_addr, uint64_t count);

void map_page(void* vaddr, void* paddr, uint64_t flags);
void unmap_page(void* vaddr);

#define PAGE_BASE(X) ((void*)((uintptr_t)(X)&0xFFFFFFFFFFFFF000))
