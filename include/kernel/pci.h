#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


bool init_pci();

uint16_t pci_config_read16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset);
uint32_t pci_config_read32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset);

void pci_config_write16(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t val);
void pci_config_write32(uint16_t seg_group, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t val);
