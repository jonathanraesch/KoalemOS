#include <efi.h>
#include <efilib.h>
#include "paging.h"
#include "graphics_common.h"
#include "boot.h"


typedef struct __attribute__((__packed__)) {
	uint64_t signature;
	uint8_t checksum;
	uint32_t oemid_lo;
	uint16_t oemid_hi;
	uint8_t revision;
	uint32_t rsdt_addr;
	uint32_t length;
	uint64_t xsdt_addr;
	uint8_t ext_checksum;
	uint16_t _reserved_lo;
	uint8_t _reserved_hi;
} acpi_rsdp;


EFI_STATUS
efi_main (EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
	InitializeLib(image_handle, system_table);

	EFI_STATUS status;
	EFI_BOOT_SERVICES *bs = system_table->BootServices;


	EFI_GUID simple_tout_pguid = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
	EFI_SIMPLE_TEXT_OUT_PROTOCOL *simple_tout_prot;
	status = uefi_call_wrapper(bs->LocateProtocol, 3, &simple_tout_pguid, NULL, &simple_tout_prot);
	if (status != EFI_SUCCESS) {
		return status;
	}

	status = uefi_call_wrapper(simple_tout_prot->ClearScreen, 1, simple_tout_prot);
	if (status != EFI_SUCCESS) {
		return status;
	}


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
	status = uefi_call_wrapper(file_prot_root->Open, 5, file_prot_root, &file_prot_kernel, L"\\EFI\\BOOT\\koalemos.bin", EFI_FILE_MODE_READ, 0);
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
	status = uefi_call_wrapper(bs->AllocatePages, 4, AllocateAnyPages, EFI_MEM_TYPE_KERNEL, kernel_pages, &kernel_addr);
	if (status != EFI_SUCCESS) {
		return status;
	}

	UINTN kernel_buf_size = 4096*kernel_pages;
	status = uefi_call_wrapper(file_prot_kernel->Read, 3, file_prot_kernel, &kernel_buf_size, (void*)kernel_addr);
	if (status != EFI_SUCCESS) {
		return status;
	}
	Print(L"Loaded kernel: ");
	Print(((EFI_FILE_INFO*)file_info_buf)->FileName);
	Print(L"\n");

	UINTN paging_pages = PAGING_STRUCTS_SIZE/4096 + 1;
	EFI_PHYSICAL_ADDRESS paging_buf;
	status = uefi_call_wrapper(bs->AllocatePages, 4, AllocateAnyPages, EFI_MEM_TYPE_KERNEL, paging_pages, &paging_buf);
	if (status != EFI_SUCCESS) {
		return status;
	}
	paging_set_up_boot_mapping((void*)paging_buf, get_pml4(), kernel_addr);


	EFI_GUID gop_pguid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	status = uefi_call_wrapper(bs->LocateProtocol, 3, &gop_pguid, NULL, &gop);
	if (status != EFI_SUCCESS) {
		return status;
	}

	uint64_t max_pixel_count = 0;
	UINT32 best_mode_num;
	for(UINT32 mode_num = 0; mode_num < gop->Mode->MaxMode; mode_num++) {
		EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;
		UINTN mode_info_size;
		status = uefi_call_wrapper(gop->QueryMode, 4, gop, mode_num, &mode_info_size, &mode_info);
		if (status != EFI_SUCCESS) {
			continue;
		}
		if(mode_info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
#ifdef MAX_HRES
			if(mode_info->HorizontalResolution > MAX_HRES) {
				continue;
			}
#endif
#ifdef MAX_VRES
			if(mode_info->VerticalResolution > MAX_VRES) {
				continue;
			}
#endif
			uint64_t pixel_count = (uint64_t)mode_info->HorizontalResolution * (uint64_t)mode_info->VerticalResolution;
			if(pixel_count > best_mode_num) {
				max_pixel_count = pixel_count;
				best_mode_num = mode_num;
			}
		}
	}
	if(max_pixel_count == 0) {
		return EFI_UNSUPPORTED;
	}

	status = uefi_call_wrapper(gop->SetMode, 2, gop, best_mode_num);
	if (status != EFI_SUCCESS) {
		return status;
	}

	gop_framebuffer_info fb_info;
	fb_info.hres = gop->Mode->Info->HorizontalResolution;
	fb_info.vres = gop->Mode->Info->VerticalResolution;
	fb_info.width = gop->Mode->Info->PixelsPerScanLine;
	fb_info.addr = (void*)gop->Mode->FrameBufferBase;


	EFI_GUID acpi20_table_guid_ = ACPI_20_TABLE_GUID;
	EFI_GUID acpi10_table_guid_ = ACPI_TABLE_GUID;
	uint64_t* acpi20_table_guid = (uint64_t*)&acpi20_table_guid_;
	uint64_t* acpi10_table_guid = (uint64_t*)&acpi10_table_guid_;
	acpi_rsdp* rsdp_10 = 0;
	acpi_rsdp* rsdp_20 = 0;
	for(UINTN i = 0; i < system_table->NumberOfTableEntries; i++) {
		uint64_t* vendor_guid = (uint64_t*)&system_table->ConfigurationTable[i].VendorGuid;
		if(acpi20_table_guid[0] == vendor_guid[0] && acpi20_table_guid[1] == vendor_guid[1]) {
			rsdp_20 = (acpi_rsdp*)system_table->ConfigurationTable[i].VendorTable;
			break;
		}
		if(acpi10_table_guid[0] == vendor_guid[0] && acpi10_table_guid[1] == vendor_guid[1]) {
			rsdp_10 = (acpi_rsdp*)system_table->ConfigurationTable[i].VendorTable;
		}
	}
	void* acpi_x_r_sdt;
	if(rsdp_20) {
		if(rsdp_20->revision >= 2) {
			acpi_x_r_sdt = (void*)(uintptr_t)rsdp_20->xsdt_addr;
		}
	}
	if(!acpi_x_r_sdt && rsdp_20) {
		acpi_x_r_sdt = (void*)(uintptr_t)rsdp_20->rsdt_addr;
	}
	if(!acpi_x_r_sdt && rsdp_10) {
		acpi_x_r_sdt = (void*)(uintptr_t)rsdp_10->rsdt_addr;
	}
	if(!acpi_x_r_sdt) {
		return EFI_UNSUPPORTED;
	}


	UINTN mmap[1000];
	UINTN mmap_buf_size = sizeof(UINTN)*1000;
	UINTN mmap_key;
	UINTN descr_size;
	UINT32 desc_ver;
	status = uefi_call_wrapper(bs->GetMemoryMap, 5, &mmap_buf_size, &mmap, &mmap_key, &descr_size, &desc_ver);
	if (status != EFI_SUCCESS) {
		return status;
	}

	status = uefi_call_wrapper(bs->ExitBootServices, 2, image_handle, mmap_key);
	if (status != EFI_SUCCESS) {
		return status;
	}


	efi_mmap_data mmap_data = {.descriptors = mmap, .mmap_size=mmap_buf_size, .descriptor_size=descr_size};
	boot_end((void*)paging_buf, (void*)KERNEL_LINADDR, &mmap_data, &fb_info, acpi_x_r_sdt);
}
