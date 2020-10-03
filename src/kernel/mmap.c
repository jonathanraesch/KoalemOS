#include "kernel/mmap.h"
#include <stdbool.h>


int mmap_add_range_merge(memory_map* mmap, void* base_addr, uint64_t pages) {
	if(pages == 0) {
		return false;
	}
	void* merge_addr = (void*)((uintptr_t)base_addr + 4096*pages);
	for(int i = 0; i < mmap->range_count; i++) {
		if(mmap->memory_ranges[i].base_addr == merge_addr) {
			mmap->memory_ranges[i].base_addr = base_addr;
			mmap->memory_ranges[i].pages += pages;
			return true;
		}
	}
	return mmap_add_range(mmap, base_addr, pages);
}

int mmap_add_range(memory_map* mmap, void* base_addr, uint64_t pages) {
	if(pages == 0) {
		return false;
	}
	if(mmap->range_count >= mmap->max_range_count) {
		return false;
	}
	memory_range range = {.base_addr=base_addr, .pages=pages};
	mmap->memory_ranges[mmap->range_count] = range;
	mmap->range_count++;
	return true;
}

void* mmap_get_pages(memory_map* mmap, uint64_t pages) {
	for(int i = 0; i < mmap->range_count; i++) {
		if(mmap->memory_ranges[i].pages == pages) {
			void* base_addr = mmap->memory_ranges[i].base_addr;
			for(int j = i+1; j < mmap->range_count; j++) {
				mmap->memory_ranges[j-1] = mmap->memory_ranges[j];
			}
			mmap->range_count--;
			return base_addr;
		}
		if(mmap->memory_ranges[i].pages > pages) {
			void* base_addr = mmap->memory_ranges[i].base_addr;
			mmap->memory_ranges[i].base_addr = (void*)((uintptr_t)mmap->memory_ranges[i].base_addr + 4096*pages);
			mmap->memory_ranges[i].pages -= pages;
			return base_addr;
		}
	}
	return 0;
}

uint64_t mmap_get_range(memory_map* mmap, void* base_addr) {
	for(int i = 0; i < mmap->range_count; i++) {
		if(mmap->memory_ranges[i].base_addr == base_addr) {
			uint64_t pages = mmap->memory_ranges[i].pages;
			mmap->range_count--;
			return pages;
		}
	}
	return 0;
}
