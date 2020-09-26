#include "acpi.h"
#include "memory.h"
#include "kernel.h"


#define SDT_HEADER_SIZE 36


enum {
	RSDT,
	XSDT
} sdt_type;

uint32_t* rsdt_entries;
uint64_t* xsdt_entries;
uint32_t sdt_entry_count;

uint8_t rsdt_signature[4] = {'R', 'S', 'D', 'T'};
uint8_t xsdt_signature[4] = {'X', 'S', 'D', 'T'};


void init_acpi(void* acpi_x_r_sdt) {
	map_page(PAGE_BASE(acpi_x_r_sdt), PAGE_BASE(acpi_x_r_sdt), 0);
	uint32_t sdt_len = ((uint32_t*)acpi_x_r_sdt)[1];
	if(*(uint32_t*)acpi_x_r_sdt == *(uint32_t*)xsdt_signature) {
		sdt_type = XSDT;
		xsdt_entries = (uint64_t*)((uintptr_t)acpi_x_r_sdt + SDT_HEADER_SIZE);
		sdt_entry_count = (sdt_len - SDT_HEADER_SIZE)/8;
	} else if(*(uint32_t*)acpi_x_r_sdt == *(uint32_t*)rsdt_signature) {
		sdt_type = RSDT;
		rsdt_entries = (uint32_t*)((uintptr_t)acpi_x_r_sdt + SDT_HEADER_SIZE);
		sdt_entry_count = (sdt_len - SDT_HEADER_SIZE)/4;
	} else {
		kernel_panic();
	}

	uint8_t* last = PAGE_BASE((uintptr_t)acpi_x_r_sdt + sdt_len);
	for(uint8_t* base = (uint8_t*)PAGE_BASE(acpi_x_r_sdt) + 0x1000; base <= last; base += 0x1000) {
		map_page(base, base, 0);
	}
	switch(sdt_type) {
	case RSDT:
		for(uint32_t i = 0; i < sdt_entry_count; i++) {
			map_page(PAGE_BASE(rsdt_entries[i]), PAGE_BASE(rsdt_entries[i]), 0);
		}
		break;
	case XSDT:
		for(uint32_t i = 0; i < sdt_entry_count; i++) {
			map_page(PAGE_BASE(xsdt_entries[i]), PAGE_BASE(xsdt_entries[i]), 0);
		}
		break;
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
			kernel_panic();	// this should never happen

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
