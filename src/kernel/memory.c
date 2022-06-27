#include "kernel/memory.h"
#include "kernel/kernel.h"
#include "kernel/mmap.h"
#include "kernel/ap_boot.h"
#include "kernel/interrupt.h"
#include "kernel/apic.h"
#include <stdalign.h>
#include <string.h>
#include <threads.h>


extern void __invalidate_tlbs();
static void map_page(void* vaddr, void* paddr, uint64_t flags);


typedef struct __attribute__((__packed__)) {
	uint32_t Type;
	uint32_t _padding;
	uint64_t PhysicalStart;
	uint64_t VirtualStart;
	uint64_t NumberOfPages;
	uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef enum {
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiPersistentMemory,
	EfiMaxMemoryType
} EFI_MEMORY_TYPE;


typedef struct _heap_entry {
	size_t size;
	bool used;
	struct _heap_entry* last;
	struct _heap_entry* next;
	max_align_t memory[];
} heap_entry;


#define ALIGN_DOWN(ADDR, ALIGN) ((uintptr_t)(ADDR)&~((uintptr_t)(ALIGN)-1u))
#define ALIGN_UP(ADDR, ALIGN) (ALIGN_DOWN((uintptr_t)(ADDR)-1, (uintptr_t)(ALIGN))+(uintptr_t)(ALIGN))
#define PAGE_BASE(X) ((void*)((uintptr_t)(X)&0xFFFFFFFFFFFFF000))


#define PHYS_MMAP_INIT_MAX_RANGE_COUNT (0x1000 / sizeof(memory_range))
_Static_assert (!((PHYS_MMAP_INIT_MAX_RANGE_COUNT * sizeof(memory_range)) & 0xfff), "physcial memory map not page-aligned");
extern memory_range _phys_mmap_range_buf[];
extern void* __phys_mmap_size_sym;
static size_t phys_mmap_max_size = (size_t)&__phys_mmap_size_sym;
static memory_map phys_mmap = {.memory_ranges=_phys_mmap_range_buf, .range_count=0, .max_range_count=PHYS_MMAP_INIT_MAX_RANGE_COUNT};

#define KERNEL_HEAP_MAX_SIZE 0x4000000000
#define KERNEL_HEAP_INIT_SIZE 0x1000
extern max_align_t kernel_heap_start[];
static max_align_t* kernel_heap_end = (max_align_t*)((uintptr_t)kernel_heap_start + KERNEL_HEAP_INIT_SIZE);
static heap_entry* first_heap_entry;
static heap_entry* last_heap_entry;

static memory_map virt_mmap;

static mtx_t heap_mutex;
static mtx_t phys_mmap_mutex;
static mtx_t phys_mmap_ext_mutex;
static mtx_t virt_mmap_mutex;
static mtx_t paging_mutex;

void* __invld_tlbs_addr;


static void* alloc_phys_pages(uint64_t pages) {
	if(mtx_lock(&phys_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	void* base_addr = mmap_get_pages(&phys_mmap, pages);
	if(mtx_unlock(&phys_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(base_addr) {
		return base_addr;
	}
	return 0;
}

// TODO: if feasible, resize memory map after merge
static bool free_phys_pages(void* base_addr, uint64_t count) {
	if(mtx_lock(&phys_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(mmap_add_range_merge(&phys_mmap, base_addr, count)) {
		if(mtx_unlock(&phys_mmap_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return true;
	}
	if(mtx_unlock(&phys_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(mtx_lock(&phys_mmap_ext_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(phys_mmap.max_range_count*sizeof(memory_range) + count*0x1000 >= phys_mmap_max_size) {
		if(mtx_unlock(&phys_mmap_ext_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return false;
	}
	map_page(phys_mmap.memory_ranges + phys_mmap.max_range_count, base_addr, PAGING_FLAG_READ_WRITE);
	phys_mmap.max_range_count += count*0x1000 / sizeof(memory_range);
	if(mtx_unlock(&phys_mmap_ext_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	return true;
}


static void virt_mmap_reduce() {
	if(virt_mmap.range_count*2 < virt_mmap.max_range_count) {
		void* new_ranges = krealloc(virt_mmap.memory_ranges, (virt_mmap.max_range_count/2)*sizeof(memory_range));
		if(new_ranges) {
			virt_mmap.memory_ranges = new_ranges;
			virt_mmap.max_range_count = virt_mmap.max_range_count/2;
		}
	}
}

static void* alloc_virt_pages(uint64_t pages) {
	if(mtx_lock(&virt_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	void* base_addr = mmap_get_pages(&virt_mmap, pages);
	if(base_addr) {
		virt_mmap_reduce();
		if(mtx_unlock(&virt_mmap_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return base_addr;
	}
	if(mtx_unlock(&virt_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	return 0;
}

static int free_virt_pages(void* base_addr, uint64_t count) {
	if(mtx_lock(&virt_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(mmap_add_range_merge(&virt_mmap, base_addr, count)) {
		virt_mmap_reduce();
		if(mtx_unlock(&virt_mmap_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return true;
	}
	void* new_ranges = krealloc(virt_mmap.memory_ranges, virt_mmap.range_count*2*sizeof(memory_range));
	if(!new_ranges) {
		if(mtx_unlock(&virt_mmap_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return false;
	}
	virt_mmap.memory_ranges = new_ranges;
	virt_mmap.max_range_count = virt_mmap.range_count*2;
	int ret = ret;
	if(mtx_unlock(&virt_mmap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	return free_virt_pages(base_addr, count);
}


// only call when current thread holds paging mutex
static void invalidate_tlbs_for(volatile void* vaddr) {
	__invld_tlbs_addr = (void*)vaddr;
	__invalidate_tlbs();
	inter_processor_call(__invalidate_tlbs);
}


#define PML4E_ADDR_OF(VADDR) ((volatile uint64_t*)(0xFFFFFFFFFFFFF000 | ((uintptr_t)(VADDR)&0xFF8000000000) >> 36))
#define PDPTE_ADDR_OF(VADDR) ((volatile uint64_t*)(0xFFFFFFFFFFE00000 | ((uintptr_t)(VADDR)&0xFFFFC0000000) >> 27))
#define PDE_ADDR_OF(VADDR)   ((volatile uint64_t*)(0xFFFFFFFFC0000000 | ((uintptr_t)(VADDR)&0xFFFFFFE00000) >> 18))
#define PTE_ADDR_OF(VADDR)   ((volatile uint64_t*)(0xFFFFFF8000000000 | ((uintptr_t)(VADDR)&0xFFFFFFFFF000) >>  9))

#define PHYS_ADDR_OF(VADDR) ((void*)((*PTE_ADDR_OF((VADDR)) & 0xFFFFFFFFF000) + ((uintptr_t)(VADDR) & 0xFFF)))

// TODO: make address calculation for zeroing structures more readable and/or performant
static void map_page(void* vaddr, void* paddr, uint64_t flags) {
	if(mtx_lock(&paging_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if (*PML4E_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT) {
		*PML4E_ADDR_OF(vaddr) |= flags;
	} else {
		uint64_t* pdpt_addr = alloc_phys_pages(1);
		if(pdpt_addr) {
			*PML4E_ADDR_OF(vaddr) = (uintptr_t)pdpt_addr | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
			for(uint64_t i = 0; i < 512; i++) {
				uint64_t* base_pdpte_addr = (uint64_t*)(((uintptr_t)vaddr&0xFFFFFF8000000000) + 0x40000000*i);
				*PDPTE_ADDR_OF(base_pdpte_addr) = 0;
			}
			*PML4E_ADDR_OF(vaddr) = (uintptr_t)pdpt_addr | PAGING_FLAG_PRESENT | flags;
		} else {
			kernel_panic(U"no space for PDPT");
		}
	}
	if (*PDPTE_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT) {
		*PDPTE_ADDR_OF(vaddr) |= flags;
	} else {
		uint64_t* pde_addr = alloc_phys_pages(1);
		if(pde_addr) {
			uint64_t pml4e_val = *PML4E_ADDR_OF(vaddr);
			*PML4E_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
			*PDPTE_ADDR_OF(vaddr) = (uintptr_t)pde_addr | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE;
			for(uint64_t i = 0; i < 512; i++) {
				uint64_t* base_pde_addr = (uint64_t*)(((uintptr_t)vaddr&0xFFFFFFFFC0000000) + 0x200000*i);
				*PDE_ADDR_OF(base_pde_addr) = 0;
			}
			*PDPTE_ADDR_OF(vaddr) = (uintptr_t)pde_addr | PAGING_FLAG_PRESENT | flags;
			*PML4E_ADDR_OF(vaddr) = pml4e_val;
		} else {
			kernel_panic(U"no space for PD");
		}
	}
	if (*PDE_ADDR_OF(vaddr) & PAGING_FLAG_PRESENT) {
		*PDE_ADDR_OF(vaddr) |= flags;
	} else {
		uint64_t* pte_addr = alloc_phys_pages(1);
		if(pte_addr) {
			uint64_t pml4e_val = *PML4E_ADDR_OF(vaddr);
			*PML4E_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
			uint64_t pdpte_val = *PDPTE_ADDR_OF(vaddr);
			*PDPTE_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
			*PDE_ADDR_OF(vaddr) = (uintptr_t)pte_addr | PAGING_FLAG_PRESENT | PAGING_FLAG_READ_WRITE | flags;
			for(uint64_t i = 0; i < 512; i++) {
				uint64_t* base_pte_addr = (uint64_t*)(((uintptr_t)vaddr&0xFFFFFFFFFFE00000) + 0x1000*i);
				*PTE_ADDR_OF(base_pte_addr) = 0;
			}
			*PDE_ADDR_OF(vaddr) = (uintptr_t)pte_addr | PAGING_FLAG_PRESENT | flags;
			*PDPTE_ADDR_OF(vaddr) = pdpte_val;
			*PML4E_ADDR_OF(vaddr) = pml4e_val;
		} else {
			kernel_panic(U"no space for PT");
		}
	}
	uint64_t pml4e_val = *PML4E_ADDR_OF(vaddr);
	*PML4E_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
	uint64_t pdpte_val = *PDPTE_ADDR_OF(vaddr);
	*PDPTE_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
	uint64_t pde_val = *PDE_ADDR_OF(vaddr);
	*PDE_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
	*PTE_ADDR_OF(vaddr) = (uintptr_t)paddr | PAGING_FLAG_PRESENT | flags;
	*PDE_ADDR_OF(vaddr) = pde_val;
	*PDPTE_ADDR_OF(vaddr) = pdpte_val;
	*PML4E_ADDR_OF(vaddr) = pml4e_val;

	invalidate_tlbs_for(vaddr);
	if(mtx_unlock(&paging_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}

// TODO: fix memory leak
static void unmap_page(void* vaddr) {
	if(mtx_lock(&paging_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	uint64_t pml4e_val = *PML4E_ADDR_OF(vaddr);
	*PML4E_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
	uint64_t pdpte_val = *PDPTE_ADDR_OF(vaddr);
	*PDPTE_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
	uint64_t pde_val = *PDE_ADDR_OF(vaddr);
	*PDE_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
	*PTE_ADDR_OF(vaddr) = 0;
	*PDE_ADDR_OF(vaddr) = pde_val;
	*PDPTE_ADDR_OF(vaddr) = pdpte_val;
	*PML4E_ADDR_OF(vaddr) = pml4e_val;

	invalidate_tlbs_for(vaddr);
	if(mtx_unlock(&paging_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}

static void unmap_page_fix_size(void* vaddr) {
	if(*PDPTE_ADDR_OF(vaddr) & PAGING_FLAG_PAGE_SIZE) {
		uint64_t flags = *PDPTE_ADDR_OF(vaddr) & 0x3E;
		uintptr_t page_addr = *PDPTE_ADDR_OF(vaddr) & 0xFFFFFFFFF000;
		uint64_t pml4e_val = *PML4E_ADDR_OF(vaddr);
		*PML4E_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
		*PDPTE_ADDR_OF(vaddr) = 0;
		*PML4E_ADDR_OF(vaddr) = pml4e_val;
		invalidate_tlbs_for(vaddr);
		for(uintptr_t offset = 0; offset < 0x40000000; offset += 0x1000) {
			map_page((void*)(ALIGN_DOWN(vaddr, 0x40000000)+offset), (void*)(page_addr+offset), flags);
		}
	} else if(*PDE_ADDR_OF(vaddr) & PAGING_FLAG_PAGE_SIZE) {
		uint64_t flags = *PDE_ADDR_OF(vaddr) & 0x3E;
		uintptr_t page_addr = *PDE_ADDR_OF(vaddr) & 0xFFFFFFFFF000;
		uint64_t pml4e_val = *PML4E_ADDR_OF(vaddr);
		*PML4E_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
		uint64_t pdpte_val = *PDPTE_ADDR_OF(vaddr);
		*PDPTE_ADDR_OF(vaddr) |= PAGING_FLAG_READ_WRITE;
		*PDE_ADDR_OF(vaddr) = 0;
		*PDPTE_ADDR_OF(vaddr) = pdpte_val;
		*PML4E_ADDR_OF(vaddr) = pml4e_val;
		invalidate_tlbs_for(vaddr);
		for(uintptr_t offset = 0; offset < 0x200000; offset += 0x1000) {
			map_page((void*)(ALIGN_DOWN(vaddr, 0x200000)+offset), (void*)(page_addr+offset), flags);
		}
	}

	unmap_page(vaddr);
}

void* create_virt_mapping(void* paddr, size_t size, uint64_t flags) {
	uintptr_t base = (uintptr_t)PAGE_BASE(paddr);
	uintptr_t page_count = ((uintptr_t)PAGE_BASE((uintptr_t)paddr + size - 1) - base)/0x1000 + 1;
	uintptr_t virt_base = (uintptr_t)alloc_virt_pages(page_count);
	if(!virt_base) {
		return 0;
	}
	for(uintptr_t offset = 0; offset <= page_count*0x1000; offset += 0x1000) {
		map_page((void*)(virt_base + offset), (void*)(base + offset), flags);
	}
	return (void*)(virt_base + ((uintptr_t)paddr & 0xFFF));
}

void delete_virt_mapping(void* vaddr, size_t size) {
	uintptr_t base = (uintptr_t)PAGE_BASE(vaddr);
	uintptr_t page_count = ((uintptr_t)PAGE_BASE((uintptr_t)vaddr + size - 1) - base)/0x1000 + 1;
	for(uintptr_t offset = 0; offset <= page_count*0x1000; offset += 0x1000) {
		unmap_page((void*)(base + offset));
	}
	free_virt_pages((void*)base, page_count);
}


static void init_mmap(efi_mmap_data* mmap_data) {
	void* const efi_mmap_start = mmap_data->descriptors;
	void* const efi_mmap_end = (void*)((uintptr_t)mmap_data->descriptors + mmap_data->mmap_size);
	const size_t efi_mmap_desc_size = mmap_data->descriptor_size;

	void *cur_desc_ptr = efi_mmap_start;
	while(cur_desc_ptr < efi_mmap_end) {

		EFI_MEMORY_DESCRIPTOR cur_desc = *((EFI_MEMORY_DESCRIPTOR*)cur_desc_ptr);
		switch(cur_desc.Type) {
			case EFI_MEM_TYPE_KERNEL:
				break;
			case EfiLoaderCode:
			case EfiLoaderData:
			case EfiBootServicesCode:
			case EfiBootServicesData:
			case EfiConventionalMemory:
			case EfiPersistentMemory:
				if(ap_boot_paddr_unset && cur_desc.PhysicalStart <= 0xFF000 && ap_boot_image_size <= 0x1000*cur_desc.NumberOfPages) {
					ap_boot_paddr_unset = false;
					ap_boot_paddr = (void*)cur_desc.PhysicalStart;
					size_t pgcnt = ap_boot_image_size % 0x1000 ? ap_boot_image_size/0x1000 + 1 : ap_boot_image_size/0x1000;
					if(cur_desc.NumberOfPages == pgcnt) {
						break;
					}
					cur_desc.NumberOfPages -= pgcnt;
					cur_desc.PhysicalStart += 0x1000*pgcnt;
				}
				if(cur_desc.PhysicalStart == 0) {
					if(cur_desc.NumberOfPages < 2u) {
						break;
					}
					cur_desc.NumberOfPages -= 1;
					cur_desc.PhysicalStart += 0x1000;
				}
				if(!mmap_add_range(&phys_mmap, (void*)cur_desc.PhysicalStart, cur_desc.NumberOfPages)) {
					kernel_panic(U"phys_mmap too large");
				}
			default:
				for(uint64_t i = 0; i < cur_desc.NumberOfPages; i++) {
					void* addr = (void*)(cur_desc.PhysicalStart + i*0x1000);
					if(addr < PAGE_BASE(efi_mmap_start) || addr > PAGE_BASE(efi_mmap_end)) {
						unmap_page_fix_size(addr);
					}
				}
				break;
		}

		cur_desc_ptr = (void*)((uintptr_t)cur_desc_ptr + efi_mmap_desc_size);
	}

	if(ap_boot_paddr_unset) {
		kernel_panic(U"no space for AP bootstrap code");
	}

	for(uintptr_t page_base = (uintptr_t)PAGE_BASE(efi_mmap_start); page_base<=(uintptr_t)efi_mmap_end; page_base+=0x1000) {
		unmap_page_fix_size((void*)page_base);
	}
}

static void init_heap() {
	last_heap_entry = (heap_entry*)kernel_heap_start;
	*last_heap_entry = (heap_entry){
		.size = KERNEL_HEAP_INIT_SIZE-sizeof(heap_entry),
		.used = false,
		.last = 0, .next = 0
	};
	first_heap_entry = last_heap_entry;
}

static void init_virt_mmap() {
	memory_range* v_ranges = kmalloc(sizeof(memory_range));
	uint64_t end = 0u-0x8000000000u;
	uint64_t start = (uint64_t)kernel_heap_start + KERNEL_HEAP_MAX_SIZE;
	v_ranges[0] = (memory_range){.base_addr=(void*)start, .pages=(end-start)/0x1000};
	virt_mmap = (memory_map){.memory_ranges=v_ranges, .range_count=1, .max_range_count=1};
}

void init_memory_management(efi_mmap_data* mmap_data) {
	if(mtx_init(&heap_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(mtx_init(&phys_mmap_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(mtx_init(&phys_mmap_ext_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(mtx_init(&virt_mmap_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(mtx_init(&paging_mutex, mtx_plain) != thrd_success) {
		kernel_panic(U"mutex failed");
	}

	init_mmap(mmap_data);
	init_heap();
	init_virt_mmap();
}

bool heap_consistency_check() {
	if(kernel_heap_start != (max_align_t*)first_heap_entry) {
		return false;
	}
	if((uintptr_t)kernel_heap_end != (uintptr_t)last_heap_entry->memory + last_heap_entry->size) {
		return false;
	}
	heap_entry* entry = first_heap_entry;
	while(entry->next) {
		if((uintptr_t)entry->next != (uintptr_t)entry->memory + entry->size) {
			return false;
		}
		entry = entry->next;
	}
	if(entry != last_heap_entry) {
		return false;
	}
	while(entry->last) {
		entry = entry->last;
	}
	if(entry != first_heap_entry) {
		return false;
	}
	return true;
}

void* kmalloc(size_t size) {
	size = ALIGN_UP(size, alignof(max_align_t));
	heap_entry* entry = first_heap_entry;

	if(mtx_lock(&heap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	while(entry) {
		if(!entry->used && entry->size >= size) {
			break;
		}
		entry = entry->next;
	}

	if(!entry) {
		uint64_t page_count = (size + sizeof(heap_entry)) / 0x1000 + 1;
		void* paddr = alloc_phys_pages(page_count);
		if(!paddr) {
			if(mtx_unlock(&heap_mutex) != thrd_success) {
				kernel_panic(U"mutex failed");
			}
			return 0;
		}
		for(uint64_t i = 0; i < page_count; i++) {
			map_page((void*)((uintptr_t)kernel_heap_end+i*0x1000), (void*)((uintptr_t)paddr+i*0x1000), PAGING_FLAG_READ_WRITE);
		}
		entry = (heap_entry*)kernel_heap_end;
		*entry = (heap_entry){
			.size = 0x1000*page_count - sizeof(heap_entry),
			.last = last_heap_entry, .next = 0,
		};
		last_heap_entry->next = entry;
		last_heap_entry = entry;
		kernel_heap_end += (page_count*0x1000)/sizeof(max_align_t);
	}
	entry->used = true;

	if(entry->size > size+sizeof(heap_entry)+sizeof(max_align_t)) {
		heap_entry* next_entry = (heap_entry*)((uintptr_t)entry->memory + size);
		*next_entry = (heap_entry){
			.size = entry->size - (size + sizeof(heap_entry)),
			.used = false,
			.last = entry, .next = entry->next,
		};
		entry->size = size;
		if(entry->next) {
			entry->next->last = next_entry;
		} else {
			last_heap_entry = next_entry;
		}
		entry->next = next_entry;
	}

	if(mtx_unlock(&heap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	return (void*)entry->memory;
}

void* krealloc(void* ptr, size_t size) {
	size = ALIGN_UP(size, alignof(max_align_t));
	heap_entry* entry = (heap_entry*)((uintptr_t)ptr - sizeof(heap_entry));

	if(mtx_lock(&heap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	if(size < entry->size) {
		if(entry->next) {
			if(!entry->next->used) {
				memmove((void*)((uintptr_t)entry->memory + size), (void*)entry->next, sizeof(heap_entry));
				entry->next = (heap_entry*)((uintptr_t)entry->memory + size);
				entry->next->size += entry->size - size;
				if(entry->next->next) {
					entry->next->next->last = entry->next;
				} else {
					last_heap_entry = entry->next;
				}
				if(mtx_unlock(&heap_mutex) != thrd_success) {
					kernel_panic(U"mutex failed");
				}
				return ptr;
			}
		}
		if(entry->size - size >= sizeof(heap_entry)+sizeof(max_align_t)) {
			heap_entry* next_entry = (heap_entry*)((uintptr_t)entry->memory + size);
			*next_entry = (heap_entry){
				.size = entry->size - (size + sizeof(heap_entry)),
				.used = false,
				.last = entry, .next = entry->next,
			};
			entry->size = size;
			if(entry->next) {
				entry->next->last = next_entry;
			} else {
				last_heap_entry = next_entry;
			}
			entry->next = next_entry;
		}
		if(mtx_unlock(&heap_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return ptr;
	}
	if(size == entry->size) {
		if(mtx_unlock(&heap_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return ptr;
	}

	size_t tot_size = entry->size;
	heap_entry* cur_entry = entry;
	while(tot_size < size) {
		cur_entry = cur_entry->next;
		if(!cur_entry) {
			break;
		}
		if(cur_entry->used) {
			break;
		}
		tot_size = cur_entry->size + sizeof(heap_entry);
	}

	if(tot_size >= size) {
		entry->next = cur_entry->next;
		if(entry->next) {
			entry->next->last = entry;
		} else {
			last_heap_entry = entry;
		}
		entry->size = tot_size;
		if(entry->size > size+sizeof(heap_entry)+sizeof(max_align_t)) {
			heap_entry* next_entry = (heap_entry*)((uintptr_t)entry->memory + size);
			*next_entry = (heap_entry){
				.size = entry->size - (size + sizeof(heap_entry)),
				.used = false,
				.last = entry, .next = entry->next,
			};
			entry->size -= (next_entry->size + sizeof(heap_entry));
			if(entry->next) {
				entry->next->last = next_entry;
			} else {
				last_heap_entry = next_entry;
			}
			entry->next = next_entry;
		}
		if(mtx_unlock(&heap_mutex) != thrd_success) {
			kernel_panic(U"mutex failed");
		}
		return ptr;
	}
	if(mtx_unlock(&heap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}

	void* ret = kmalloc(size);
	if(ret) {
		memcpy(ret, ptr, entry->size);
		kfree(ptr);
		return ret;
	}
	return 0;
}

void kfree(void* ptr) {
	heap_entry* entry = (heap_entry*)((uintptr_t)ptr - sizeof(heap_entry));
	if(mtx_lock(&heap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
	entry->used = false;
	if(entry->last) {
		if(!entry->last->used && (uintptr_t)entry->last->memory + entry->last->size == (uintptr_t)entry) {
			entry->last->size += sizeof(heap_entry) + entry->size;
			if(entry->next) {
				entry->next->last = entry->last;
			} else {
				last_heap_entry = entry->last;
			}
			entry->last->next = entry->next;
			entry = entry->last;
		}
	}
	if(entry->next) {
		if(!entry->next->used && (uintptr_t)entry->memory + entry->size == (uintptr_t)entry->next) {
			entry->size += sizeof(heap_entry) + entry->next->size;
			entry->next = entry->next->next;
			if(entry->next) {
				entry->next->last = entry;
			} else {
				last_heap_entry = entry;
			}
		}
	}

	if(!entry->next && entry->last && (void*)ALIGN_UP((uintptr_t)entry->memory + sizeof(max_align_t), 0x1000) != kernel_heap_end) {
		uintptr_t new_end = ALIGN_UP(entry, 0x1000);
		if(entry == (void*)new_end) {
			last_heap_entry = entry->last;
			last_heap_entry->next = 0;
		} else {
			entry->size = new_end - (uintptr_t)entry - sizeof(heap_entry);
		}
		uint64_t page_count = ((uintptr_t)kernel_heap_end - new_end)/0x1000;
		free_phys_pages(PHYS_ADDR_OF(new_end), page_count);
		for(uintptr_t page_base = (uintptr_t)kernel_heap_end; page_base < new_end; page_base++) {
			unmap_page((void*)page_base);
		}
		kernel_heap_end = (max_align_t*)new_end;
	}
	if(mtx_unlock(&heap_mutex) != thrd_success) {
		kernel_panic(U"mutex failed");
	}
}
