#include "multiboot.h"


#define ALIGN_UP(P, B) ((void*)((((uintptr_t)(P))+(((uintptr_t)(B))-1)) & -((uintptr_t)(B))))


mb_info_tag_t* mb_info_find_tag(mb_info_t* mb_info, uint32_t tag_type) {
	mb_info_tag_t* cur_tag = (mb_info_tag_t*)mb_info->tags;
	while(cur_tag->type != MB_INFO_TAG_END && cur_tag->type != tag_type) {
		cur_tag = (mb_info_tag_t*)ALIGN_UP((uintptr_t)cur_tag + cur_tag->size, 8);
	}
	return cur_tag;
}