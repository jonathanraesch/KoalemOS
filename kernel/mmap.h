#pragma once
#include <stdint.h>

typedef struct {
	void* base_addr;
	uint64_t pages;
} memory_range;

typedef struct {
	memory_range* memory_ranges;
	uint64_t range_count;
	uint64_t max_range_count;
} memory_map;


int mmap_add_range_merge(memory_map* mmap, void* base_addr, uint64_t pages);
int mmap_add_range(memory_map* mmap, void* base_addr, uint64_t pages);
void* mmap_get_pages(memory_map* mmap, uint64_t pages);
uint64_t mmap_get_range(memory_map* mmap, void* base_addr);
