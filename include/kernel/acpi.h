#pragma once
#include <stdint.h>
#include <stdbool.h>


// ACPI defined
#define ACPI_SIGNATURE_RSDT 0x54445352
#define ACPI_SIGNATURE_XSDT 0x54445358

// ACPI reserved
#define ACPI_SIGNATURE_MCFG 0x4746434D


typedef struct {
	void* addr;
	uint32_t length;
} acpi_sdt;


void init_acpi(void* acpi_sdt_paddr);

bool get_acpi_table(uint32_t signature, acpi_sdt* table);
