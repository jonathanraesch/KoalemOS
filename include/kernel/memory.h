#pragma once
#include "common/paging.h"
#include "common/mmap.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

void* alloc_virt_pages(uint64_t pages);
int free_virt_pages(void* base_addr, uint64_t count);

void* create_virt_mapping(void* paddr, size_t size, uint64_t flags);
void delete_virt_mapping(void* vaddr, size_t size);

void init_memory_management(efi_mmap_data* mmap_data);
bool heap_consistency_check();

#define PAGE_BASE(X) ((void*)((uintptr_t)(X)&0xFFFFFFFFFFFFF000))
