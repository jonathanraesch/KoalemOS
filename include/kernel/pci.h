#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


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


extern pci_config_header** devices;
extern size_t device_count;


bool init_pci();

uint16_t pcie_read16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset);
uint32_t pcie_read32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset);

void pcie_write16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t val);
void pcie_write32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t val);
