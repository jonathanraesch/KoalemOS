#pragma once
#include "paging_common.h"


/* sets up minimal paging to boot kernel
   returns address of pml4 */
void* paging_set_up_boot_mapping();