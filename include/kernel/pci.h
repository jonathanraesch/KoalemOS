#pragma once
#include <stdint.h>


void init_pci();

uint16_t pcie_read16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset);
uint32_t pcie_read32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset);

void pcie_write16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t val);
void pcie_write32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t val);
