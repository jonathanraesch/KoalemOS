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


void init_acpi(void* acpi_sdt_paddr) {
	void* acpi_x_r_sdt = create_virt_mapping(acpi_sdt_paddr, SDT_HEADER_SIZE, 0);
	if(!acpi_x_r_sdt) {
		kernel_panic(U"failed to create virt mapping for SDT header");
	}
	uint32_t sdt_len = ((uint32_t*)acpi_x_r_sdt)[1];
	delete_virt_mapping(acpi_x_r_sdt, SDT_HEADER_SIZE);
	acpi_x_r_sdt = create_virt_mapping(acpi_sdt_paddr, sdt_len, 0);
	if(!acpi_x_r_sdt) {
		kernel_panic(U"failed to create virt mapping for SDT");
	}

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
	void* table_paddr = 0;
	uint32_t table_size;
	for(uint32_t i = 0; i < sdt_entry_count; i++) {
		switch(sdt_type) {
			case RSDT:
				table_paddr = (void*)(uintptr_t)rsdt_entries[i];
				break;
			case XSDT:
				table_paddr = (void*)xsdt_entries[i];
				break;
			default:
				break;
		}
		void* table_addr = create_virt_mapping(table_paddr, SDT_HEADER_SIZE, 0);
		if(!table_addr) {
			kernel_panic(U"failed to create virt mapping for ACPI table header");
		}
		if(*(uint32_t*)table_addr != signature) {
			delete_virt_mapping(table_addr, SDT_HEADER_SIZE);
			table_paddr = 0;
		} else {
			table_size = *(uint32_t*)((uintptr_t)table_addr + 4);
			delete_virt_mapping(table_addr, SDT_HEADER_SIZE);
			break;
		}
	}
	if(!table_paddr) {
		table = 0;
		return false;
	}

	void* table_addr = create_virt_mapping(table_paddr, table_size, 0);
	if(!table_addr) {
		kernel_panic(U"failed to create virt mapping for ACPI table");
	}

	void* table_body = (void*)((uintptr_t)table_addr + SDT_HEADER_SIZE);
	*table = (acpi_sdt){.addr=table_body, .length=table_size-SDT_HEADER_SIZE};
	return true;
}
