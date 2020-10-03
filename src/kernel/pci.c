#include "kernel/pci.h"
#include "kernel/acpi.h"
#include "kernel/kernel.h"
#include "kernel/memory.h"
#include "common/paging.h"


typedef struct {
	uint64_t base_addr;
	uint16_t segment_group_num;
	uint8_t start_bus_num;
	uint8_t end_bus_num;
	uint32_t reserved;
} pci_config_base_addr;

pci_config_base_addr* group_config_addrs;
uint32_t group_count;


#define PCIE_CONF_ADDR(SEG_GROUP, BUS, DEV, FUN, OFFSET) ((void*)(group_config_addrs[SEG_GROUP].base_addr + ((BUS)<<20) + ((DEV)<<15) + ((FUN)<<12) + OFFSET))


void init_pci() {
	acpi_sdt mcfg;
	if(!get_acpi_table(ACPI_SIGNATURE_MCFG, &mcfg)) {
		kernel_panic();
	}
	group_config_addrs = (pci_config_base_addr*)((uintptr_t)mcfg.addr + 8);
	group_count = (mcfg.length - 8)/16;

	for(uint16_t group = 0; group < group_count; group++) {
		uint8_t bus_count = group_config_addrs[group].end_bus_num - group_config_addrs[group].start_bus_num;
		uintptr_t conf_addrs_end = group_config_addrs[group].base_addr + bus_count*0x100000;
		for(uintptr_t cur_page = group_config_addrs[group].base_addr; cur_page < conf_addrs_end; cur_page+=0x1000) {
			map_page((void*)cur_page, (void*)cur_page, PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE);
		}
	}
}


uint16_t pcie_read16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(seg_group, bus, device, function, offset);
	return *(uint16_t*)addr;
}

uint32_t pcie_read32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(seg_group, bus, device, function, offset);
	return *(uint32_t*)addr;
}


void pcie_write16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t val) {
	*(uint16_t*)PCIE_CONF_ADDR(seg_group, bus, device, function, offset) = val;
}

void pcie_write32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t val) {
	*(uint32_t*)PCIE_CONF_ADDR(seg_group, bus, device, function, offset) = val;
}
