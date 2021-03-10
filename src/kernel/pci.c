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

static pci_config_base_addr* group_config_addrs;
static uint32_t group_count;

pci_config_header** pci_devices;
static size_t device_capacity;
size_t pci_device_count;


#define PCIE_CONF_ADDR(SEG_GROUP, BUS, DEV, FUN, OFFSET) ((void*)(group_config_addrs[SEG_GROUP].base_addr + (((BUS)-group_config_addrs[SEG_GROUP].start_bus_num)<<20) + ((DEV)<<15) + ((FUN)<<12) + OFFSET))


static bool find_devices() {
	pci_devices = kmalloc(2*sizeof(pci_config_header*));
	if(!pci_devices) {
		return false;
	}
	device_capacity = 2;
	pci_device_count = 0;

	for(int group = 0; group < group_count; group++) {
		for(int bus = group_config_addrs[group].start_bus_num; bus < group_config_addrs[group].end_bus_num; bus++) {
			for(int dev = 0; dev < 32; dev++) {
				pci_config_header* header = (pci_config_header*)PCIE_CONF_ADDR(group, bus, dev, 0, 0);
				if(header->vendor_id == 0xFFFF) {
					continue;
				}
				if(pci_device_count == device_capacity) {
					void* new_devs = krealloc(pci_devices, 2*device_capacity*sizeof(pci_config_header*));
					if(!new_devs) {
						kfree(pci_devices);
						return false;
					} else {
						pci_devices = new_devs;
						device_capacity *= 2;
					}
				}
				pci_devices[pci_device_count++] = header;
				if(header->type & (1u << 7)) {
					for(int fun = 1; fun < 8; fun++) {
						header = (pci_config_header*)PCIE_CONF_ADDR(group, bus, dev, fun, 0);
						if(header->vendor_id == 0xFFFF) {
							continue;
						}
						if(pci_device_count == device_capacity) {
							void* new_devs = krealloc(pci_devices, 2*device_capacity*sizeof(pci_config_header*));
							if(!new_devs) {
								kfree(pci_devices);
								return false;
							} else {
								pci_devices = new_devs;
								device_capacity *= 2;
							}
						}
						pci_devices[pci_device_count++] = header;
					}
				}
			}
		}
	}
	return true;
}


bool init_pci() {
	acpi_sdt mcfg;
	if(!get_acpi_table(ACPI_SIGNATURE_MCFG, &mcfg)) {
		return false;
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
	return find_devices();
}


uint16_t pci_config_read16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(seg_group, bus, device, function, offset);
	return *(uint16_t*)addr;
}

uint32_t pci_config_read32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(seg_group, bus, device, function, offset);
	return *(uint32_t*)addr;
}


void pci_config_write16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t val) {
	*(uint16_t*)PCIE_CONF_ADDR(seg_group, bus, device, function, offset) = val;
}

void pci_config_write32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t val) {
	*(uint32_t*)PCIE_CONF_ADDR(seg_group, bus, device, function, offset) = val;
}
