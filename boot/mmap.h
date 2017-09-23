#include "multiboot.h"

mb_info_tag_memory_map_t* mmap_find(mb_info_t* mb_info);

void mmap_sort(mb_info_tag_memory_map_t* mmap);
void mmap_remove_overlap(mb_info_tag_memory_map_t* mmap);
void mmap_merge_entries(mb_info_tag_memory_map_t* mmap);

void mmap_swap_entries(mb_info_tag_memory_map_t* mmap, uint32_t index_a, uint32_t index_b);
void mmap_delete_entry(mb_info_tag_memory_map_t* mmap, uint32_t index);