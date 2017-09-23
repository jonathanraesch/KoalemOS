#include "multiboot_header.h"


const struct mb_header_t multiboot_header = {
  .common_fields = {
  	MB_HEADER_MAGIC_NUMBER,
  	MB_HEADER_ARCHITECTURE_I386,
  	sizeof(mb_header_t),
  	-(uint32_t)(MB_HEADER_MAGIC_NUMBER + MB_HEADER_ARCHITECTURE_I386 + sizeof(mb_header_t))
  },
  .info_request_tag = {
  	MB_HEADER_TAG_INFORMATION_REQUEST,
  	MB_HEADER_FLAG_REQUIRED,
  	sizeof(mb_header_tag_information_request_t) + NUM_INFO_REQUESTS * sizeof(uint32_t),
  },
  .info_request_types = {MB_INFO_TAG_MEMORY_MAP},
  .end_tag = {
  	MB_HEADER_TAG_END,
  	0,
  	sizeof(mb_header_tag_t)
  }
};
