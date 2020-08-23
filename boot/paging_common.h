#pragma once


#define PAGING_FLAG_PRESENT						(1 << 0)
#define PAGING_FLAG_READ_WRITE					(1 << 1)
#define PAGING_FLAG_USER_SUPERVISOR				(1 << 2)
#define PAGING_FLAG_PAGE_LEVEL_WRITETHROUGH		(1 << 3)
#define PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE	(1 << 4)
#define PAGING_FLAG_ACCESSED					(1 << 5)
#define PAGING_FLAG_DIRTY						(1 << 6)
#define PAGING_FLAG_PAGE_SIZE					(1 << 7)
#define PAGING_FLAG_PAGE_ATTRIBUTE_TABLE_PTE	(1 << 7)
#define PAGING_FLAG_GLOBAL						(1 << 8)
#define PAGING_FLAG_PAGE_ATTRIBUTE_TABLE_PDP	(1 << 12)
#define PAGING_FLAG_PAGE_ATTRIBUTE_TABLE_PDPTE	(1 << 12)
#define PAGING_FLAG_EXECUTE_DISABLE				(1 << 63)

#define PAGING_PT_OFFSET(LIN_ADDR) (((LIN_ADDR)>>12) & 0x1FF)
#define PAGING_PD_OFFSET(LIN_ADDR) (((LIN_ADDR)>>21) & 0x1FF)
#define PAGING_PDPT_OFFSET(LIN_ADDR) (((LIN_ADDR)>>30) & 0x1FF)
#define PAGING_PML4_OFFSET(LIN_ADDR) ((LIN_ADDR)>>39)


#define KERNEL_LINADDR 0xFFFFFF0000000000