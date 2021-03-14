#include "kernel/acpi.h"
#include "kernel/memory.h"
#include "kernel/kernel.h"


#define SDT_HEADER_SIZE 36


enum {
	RSDT,
	XSDT
} sdt_type;

static uint32_t* rsdt_entries;
static uint64_t* xsdt_entries;
static uint32_t sdt_entry_count;


void init_acpi(void* acpi_x_r_sdt) {
	uint32_t sdt_len = ((uint32_t*)acpi_x_r_sdt)[1];
	if(*(uint32_t*)acpi_x_r_sdt == ACPI_SIGNATURE_XSDT) {
		sdt_type = XSDT;
		xsdt_entries = (uint64_t*)((uintptr_t)acpi_x_r_sdt + SDT_HEADER_SIZE);
		sdt_entry_count = (sdt_len - SDT_HEADER_SIZE)/8;
	} else if(*(uint32_t*)acpi_x_r_sdt == ACPI_SIGNATURE_RSDT) {
		sdt_type = RSDT;
		rsdt_entries = (uint32_t*)((uintptr_t)acpi_x_r_sdt + SDT_HEADER_SIZE);
		sdt_entry_count = (sdt_len - SDT_HEADER_SIZE)/4;
	} else {
		kernel_panic(U"neither XSDT nor RSDT present");
	}
}

bool get_acpi_table(uint32_t signature, acpi_sdt* table) {
	void* table_addr = 0;
	switch(sdt_type) {
		case RSDT:
			for(uint32_t i = 0; i < sdt_entry_count; i++) {
				if(*(uint32_t*)(uintptr_t)rsdt_entries[i] == signature) {
					table_addr = (void*)(uintptr_t)rsdt_entries[i];
				}
			}
			break;
		case XSDT:
			for(uint32_t i = 0; i < sdt_entry_count; i++) {
				if(*(uint32_t*)xsdt_entries[i] == signature) {
					table_addr = (void*)xsdt_entries[i];
				}
			}
			break;
		default:
			kernel_panic(U"ACPI not initialized properly");	// this should never happen

	}
	if(!table_addr) {
		table = 0;
		return false;
	}
	uint32_t table_size = *(uint32_t*)((uintptr_t)table_addr + 4);
	uint8_t* last = PAGE_BASE((uintptr_t)table_addr + table_size);
	for(uint8_t* base = (uint8_t*)PAGE_BASE(table_addr) + 0x1000; base <= last; base += 0x1000) {
		map_page(base, base, 0);
	}

	void* table_body = (void*)((uintptr_t)table_addr + SDT_HEADER_SIZE);
	*table = (acpi_sdt){.addr=table_body, .length=table_size-SDT_HEADER_SIZE};
	return true;
}
