#include "mmap.h"
#include "multiboot.h"
#include "boot.h"
#include <stdbool.h>


#define ALIGN_UP(P, B) ((void*)((((uintptr_t)(P))+(((uintptr_t)(B))-1)) & -((uintptr_t)(B))))


mb_info_tag_memory_map_t* mmap_find(mb_info_t* mb_info) {
	mb_info_tag_t* tag = mb_info_find_tag(mb_info, MB_INFO_TAG_MEMORY_MAP);
	if(tag->type == MB_INFO_TAG_END) {
		boot_error(str_error_no_mmap);
	}
	return (mb_info_tag_memory_map_t*)tag;
}


void mmap_sort(mb_info_tag_memory_map_t* mmap) {
	bool sorted = false;
	int entries = (mmap->size-sizeof(mmap))/mmap->entry_size;
	while(!sorted) {
		sorted = true;
		for(int i = 0; i < entries-1; i++) {
			if(mmap->entries[i].base_addr > mmap->entries[i+1].base_addr) {
				mmap_swap_entries(mmap, i, i+1);
				sorted = false;
			}
		}
	}
}

/* can't properly handle usable entries extending completely past unusable ones (will lead to "losing" memory regions)
   proper handling requires adding entries (impossible without dynamic allocation) */
void mmap_remove_overlap(mb_info_tag_memory_map_t* mmap) {
	int entries = (mmap->size-sizeof(mmap))/mmap->entry_size;
	for(int i = 0; i < entries-1; i++) {
		if(mmap->entries[i].base_addr + mmap->entries[i].length > mmap->entries[i+1].base_addr) {
			bool prioritize_current = false;
			if(mmap->entries[i].type == 2 && mmap->entries[i+1].type != 5) {
				prioritize_current = true;
			} else if(mmap->entries[i].type >= mmap->entries[i+1].type) {
				prioritize_current = true;
			}
			uint64_t overlap = mmap->entries[i].base_addr + mmap->entries[i].length - mmap->entries[i+1].base_addr;
			if(prioritize_current) {
				if(overlap >= mmap->entries[i+1].length) {
					mmap_delete_entry(mmap, i+1);
					entries = (mmap->size-sizeof(mmap))/mmap->entry_size;
					i--;
					continue;
				}
				mmap->entries[i+1].length -= overlap;
				mmap->entries[i+1].base_addr += overlap;
			} else {
				mmap->entries[i].length -= overlap;
			}
		}
	}
}

void mmap_merge_entries(mb_info_tag_memory_map_t* mmap) {
	int entries = (mmap->size-sizeof(mmap))/mmap->entry_size;
	for(int i = 0; i < entries-1; i++) {
		if((mmap->entries[i].base_addr + mmap->entries[i].length == mmap->entries[i+1].base_addr) && (mmap->entries[i].type == mmap->entries[i+1].type)) {
			mmap->entries[i].length += mmap->entries[i+1].length;
			mmap_delete_entry(mmap, i+1);
			entries = (mmap->size-sizeof(mmap))/mmap->entry_size;
			i--;
		}
	}
}

void mmap_sanitize(mb_info_tag_memory_map_t* mmap) {
	mmap_sort(mmap);
	mmap_remove_overlap(mmap);
	mmap_merge_entries(mmap);
}


void mmap_delete_entry(mb_info_tag_memory_map_t* mmap, uint32_t index) {
	int entries = (mmap->size-sizeof(mmap))/mmap->entry_size;
	for(int i = index; i < entries-1; i++) {
		mmap->entries[i] = mmap->entries[i+1];
	}
	mmap->size -= mmap->entry_size;
}