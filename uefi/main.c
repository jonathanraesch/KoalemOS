#include <efi.h>
#include <efilib.h>


EFI_STATUS
efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *system_table) {
	InitializeLib(image, system_table);
	
	Print(L"Testing, Testing, Testing.\n");

	return EFI_SUCCESS;
}