#include "kernel/pci.h"
#include "kernel/acpi.h"
#include "kernel/kernel.h"
#include "kernel/memory.h"
#include "common/paging.h"


#define PCI_CONF_OFFS_BARS			0x10
#define PCI_CONF0_OFFS_BARS_END		0x28
#define PCI_CONF1_OFFS_BARS_END		0x18

#define PCI_HEADER_TYPE_MF_MASK		0x80
#define PCI_HEADER_TYPE_MASK		0xFF7F


typedef struct {
	uint64_t base_addr;
	uint16_t segment_group_num;
	uint8_t start_bus_num;
	uint8_t end_bus_num;
	uint32_t reserved;
} pci_config_base_addr;

typedef struct {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t command;
	uint16_t status;
	uint8_t revision_id;
	uint8_t class_interface;
	uint8_t class_sub;
	uint8_t class_base;
	uint8_t cache_line_size;
	uint8_t master_latency_timer;
	uint8_t type;
	uint8_t bist;
	uint8_t __type_specific_01[36];
	uint8_t capabilities_ptr;
	uint8_t __type_specific_02[7];
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
	uint8_t __type_specific_03[2];
} pci_config_header;

typedef struct {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t command;
	uint16_t status;
	uint8_t revision_id;
	uint8_t class_interface;
	uint8_t class_sub;
	uint8_t class_base;
	uint8_t cache_line_size;
	uint8_t master_latency_timer;
	uint8_t type;
	uint8_t bist;
	uint32_t bars[6];
	uint32_t cardbus_cis_ptr;
	uint16_t subsys_ven_id;
	uint16_t subsys_id;
	uint32_t expansion_rom_bar;
	uint8_t capabilities_ptr;
	uint8_t __reserved[7];
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
	uint8_t min_grant;
	uint8_t max_latency;
} pci_config0_header;

typedef struct {
	void* addr;
	size_t size;
	bool is_io_space;
} pci_bar_desc;

typedef struct {
	pci_config_header* conf;
	pci_bar_desc bars[6];
} pci_dev_desc;


static pci_config_base_addr* group_config_addrs;
static uint32_t group_count;

static pci_dev_desc* pci_devices;
static size_t device_capacity;
static size_t pci_device_count;


#define PCIE_CONF_ADDR(SEG_GROUP, BUS, DEV, FUN, OFFSET) ((void*)(group_config_addrs[SEG_GROUP].base_addr + (((BUS)-group_config_addrs[SEG_GROUP].start_bus_num)<<20) + ((DEV)<<15) + ((FUN)<<12) + OFFSET))


static size_t bar_size(volatile uint32_t* bar, bool is_64bit) {
	if(is_64bit) {
		volatile uint64_t* bar64 = (uint64_t*)bar;
		uint64_t old_val = *bar64;
		*bar64 = 0xFFFFFFFFFFFFFFFFu;
		uint64_t ret = *bar64;
		ret &= ~((uint64_t)0xF);
		ret = ~ret;
		ret += 1;
		*bar64 = old_val;
		return ret;
	}
	uint32_t old_val = *bar;
	*bar = 0xFFFFFFFFu;
	uint32_t ret = *bar;
	ret &= ~((uint32_t)0xF);
	ret = ~ret;
	ret += 1;
	*bar = old_val;
	return ret;
}

static pci_dev_desc make_dev_desc(pci_config_header* header) {
	pci_dev_desc ret;
	ret.conf = header;
	for(int i = 0; i < 6; i++) {
		ret.bars[i] = (pci_bar_desc){0,0,0};
	}

	size_t bars_sz;
	if((header->type & PCI_HEADER_TYPE_MASK) == 0) {
		bars_sz = (PCI_CONF0_OFFS_BARS_END - PCI_CONF_OFFS_BARS)/4;
	} else if((header->type & PCI_HEADER_TYPE_MASK) == 1) {
		bars_sz = (PCI_CONF1_OFFS_BARS_END - PCI_CONF_OFFS_BARS)/4;
	} else {
		return ret;
	}

	uint16_t cmd = header->command;
	header->command = 0;

	size_t offset = 0;
	pci_config0_header* header_0 = (pci_config0_header*)header;
	while(offset < bars_sz) {
		uint32_t* bar = &(header_0->bars[offset]);
		if(!(*bar) || (*bar & 0x7) == 0x2) {	// deprecated <1MB bar not (yet) implemented
			offset++;
			continue;
		}
		if(*bar & 1) {
			ret.bars[offset].is_io_space = true;
		}
		if((*bar & 0x7) == 0x4) {
			ret.bars[offset].size = bar_size(bar, true);
			ret.bars[offset].addr = (void*)(*(uint64_t*)bar);
			offset += 2;
		} else {
			ret.bars[offset].size = bar_size(bar, false);
			ret.bars[offset].addr = (void*)(uintptr_t)(*(uint32_t*)bar);
			offset++;
		}
	}

	header->command = cmd;
	return ret;
}

static bool find_devices() {
	pci_devices = kmalloc(2*sizeof(pci_dev_desc));
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
					void* new_devs = krealloc(pci_devices, 2*device_capacity*sizeof(pci_dev_desc));
					if(!new_devs) {
						kfree(pci_devices);
						return false;
					} else {
						pci_devices = new_devs;
						device_capacity *= 2;
					}
				}
				pci_devices[pci_device_count++] = make_dev_desc(header);
				if(header->type & PCI_HEADER_TYPE_MF_MASK) {
					for(int fun = 1; fun < 8; fun++) {
						header = (pci_config_header*)PCIE_CONF_ADDR(group, bus, dev, fun, 0);
						if(header->vendor_id == 0xFFFF) {
							continue;
						}
						if(pci_device_count == device_capacity) {
							void* new_devs = krealloc(pci_devices, 2*device_capacity*sizeof(pci_dev_desc));
							if(!new_devs) {
								kfree(pci_devices);
								return false;
							} else {
								pci_devices = new_devs;
								device_capacity *= 2;
							}
						}
						pci_devices[pci_device_count++] = make_dev_desc(header);
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
			map_page((void*)cur_page, (void*)cur_page, PAGING_FLAG_PAGE_LEVEL_CACHE_DISABLE | PAGING_FLAG_READ_WRITE);
		}
	}
	return find_devices();
}


uint8_t pci_config_read8(pci_function_addr fun_addr, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset);
	return *(uint8_t*)addr;
}

uint16_t pci_config_read16(pci_function_addr fun_addr, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset);
	return *(uint16_t*)addr;
}

uint32_t pci_config_read32(pci_function_addr fun_addr, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset);
	return *(uint32_t*)addr;
}

uint64_t pci_config_read64(pci_function_addr fun_addr, uint16_t offset) {
	void* addr = PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset);
	return *(uint64_t*)addr;
}


void pci_config_write8(pci_function_addr fun_addr, uint16_t offset, uint8_t val) {
	*(uint8_t*)PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset) = val;
}

void pci_config_write16(pci_function_addr fun_addr, uint16_t offset, uint16_t val) {
	*(uint16_t*)PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset) = val;
}

void pci_config_write32(pci_function_addr fun_addr, uint16_t offset, uint32_t val) {
	*(uint32_t*)PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset) = val;
}

void pci_config_write64(pci_function_addr fun_addr, uint16_t offset, uint64_t val) {
	*(uint64_t*)PCIE_CONF_ADDR(fun_addr.segment_group, fun_addr.bus, fun_addr.device, fun_addr.function, offset) = val;
}
