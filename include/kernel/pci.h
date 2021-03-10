#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


typedef struct {
	uint16_t segment_group;
	uint8_t bus;
	uint8_t device;
	uint8_t function;
} pci_function_addr;


bool init_pci();

uint16_t pci_config_read16(pci_function_addr fun_addr, uint16_t offset);
uint32_t pci_config_read32(pci_function_addr fun_addr, uint16_t offset);

void pci_config_write16(pci_function_addr fun_addr, uint16_t offset, uint16_t val);
void pci_config_write32(pci_function_addr fun_addr, uint16_t offset, uint32_t val);
