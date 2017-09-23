#pragma once
#include "multiboot.h"

#define NUM_INFO_REQUESTS 1

typedef struct mb_header_t {
    mb_header_common_t common_fields;
    mb_header_tag_information_request_t info_request_tag;
    uint32_t __attribute__((aligned(0))) info_request_types[NUM_INFO_REQUESTS];
    mb_header_tag_t end_tag;
} __attribute__((aligned(8))) mb_header_t;
