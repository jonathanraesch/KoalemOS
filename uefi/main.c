#include <efi.h>
#include <efilib.h>


EFI_STATUS
efi_main (EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
	InitializeLib(image_handle, system_table);

	EFI_STATUS status;
	EFI_BOOT_SERVICES *bs = system_table->BootServices;


	EFI_GUID simple_file_system_pguid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simple_file_system_prot;
	status = uefi_call_wrapper(bs->LocateProtocol, 3, &simple_file_system_pguid, NULL, &simple_file_system_prot);
	if (status != EFI_SUCCESS) {
		return status;
	}

	EFI_FILE_PROTOCOL *file_prot_root;
	status = uefi_call_wrapper(simple_file_system_prot->OpenVolume, 2, simple_file_system_prot, &file_prot_root);
	if (status != EFI_SUCCESS) {
		return status;
	}

	EFI_FILE_PROTOCOL *file_prot_kernel;
	status = uefi_call_wrapper(file_prot_root->Open, 5, file_prot_root, &file_prot_kernel, L"\\EFI\\BOOT\\kernel.elf", EFI_FILE_MODE_READ, 0);
	if (status != EFI_SUCCESS) {
		return status;
	}

	EFI_GUID file_info_id = EFI_FILE_INFO_ID;
	UINTN file_info_buf[100];
	UINTN buf_size = sizeof(UINTN)*100;
	status = uefi_call_wrapper(file_prot_kernel->GetInfo, 4, file_prot_kernel, &file_info_id, &buf_size, file_info_buf);
	if (status != EFI_SUCCESS) {
		return status;
	}

	UINT64 kernel_size = ((EFI_FILE_INFO*)file_info_buf)->FileSize;
	UINTN kernel_pages = kernel_size/4096 + 1;
	EFI_PHYSICAL_ADDRESS kernel_addr;
	status = uefi_call_wrapper(bs->AllocatePages, 4, AllocateAnyPages, EfiLoaderCode, kernel_pages, &kernel_addr);
	if (status != EFI_SUCCESS) {
		return status;
	}

	status = uefi_call_wrapper(file_prot_kernel->Read, 3, file_prot_kernel, 4096*kernel_pages, (void*)kernel_addr);
	if (status != EFI_SUCCESS) {
		CHAR16 *str = L"a";
		str[0] += status;
		Print(str);
		while(TRUE){}
		return status;
	}


	Print(L"Loaded kernel: ");
	Print(((EFI_FILE_INFO*)file_info_buf)->FileName);
	Print(L"\n");


	while(TRUE) {

	}

	return EFI_SUCCESS;
}