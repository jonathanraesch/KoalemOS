#pragma once
#include <stdint.h>

/* bootloader / mb info definitions */

#define MB_BOOTLOADER_MAGIC_NUMBER 0x36D76289u

#define MB_INFO_MEMORY_TYPE_RESERVED 0
#define MB_INFO_MEMORY_TYPE_AVAILABLE 1
#define MB_INFO_MEMORY_TYPE_ACPI_INFO 3
#define MB_INFO_MEMORY_TYPE_ACPI_NVS 4
#define MB_INFO_MEMORY_TYPE_DEFECTIVE 5

#define MB_INFO_FRAMEBUFFER_TYPE_INDEXED 0
#define MB_INFO_FRAMEBUFFER_TYPE_RGB 1

#define MB_INFO_TAG_END 0
#define MB_INFO_TAG_COMMAND_LINE 1
#define MB_INFO_TAG_BOOTLOADER_NAME 2
#define MB_INFO_TAG_MODULES 3
#define MB_INFO_TAG_MEMORY_INFO 4
#define MB_INFO_TAG_BOOT_DEVICE 5
#define MB_INFO_TAG_MEMORY_MAP 6
#define MB_INFO_TAG_VBE_INFO 7
#define MB_INFO_TAG_FRAMEBUFFER_INFO 8
#define MB_INFO_TAG_ELF_SYMBOLS 9
#define MB_INFO_TAG_APM_TABLE 10
#define MB_INFO_TAG_EFI_I386_SYSTEM_TABLE 11
#define MB_INFO_TAG_EFI_AMD64_SYSTEM_TABLE 12
#define MB_INFO_TAG_SMBIOS_TABLES 13
#define MB_INFO_TAG_ACPI_1_0_RSDP 14
#define MB_INFO_TAG_ACPI_2_0_RSDP 15
#define MB_INFO_TAG_DHCP_NETWORK_INFO 16
#define MB_INFO_TAG_EFI_MEMORY_MAP 17
#define MB_INFO_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED 18
#define MB_INFO_TAG_EFI_I386_IMAGE_HANDLE 19
#define MB_INFO_TAG_EFI_AMD64_IMAGE_HANDLE 20
#define MB_INFO_TAG_IMAGE_LOAD_BASE_ADDRESS 21


/* os / mb header definitions */

#define MB_HEADER_MAGIC_NUMBER 0xE85250D6u

#define MB_HEADER_ARCHITECTURE_I386 0
#define MB_HEADER_ARCHITECTURE_MIPS 4

#define MB_HEADER_FLAG_REQUIRED 0
#define MB_HEADER_FLAG_OPTIONAL 1

#define MB_HEADER_TAG_END 0
#define MB_HEADER_TAG_INFORMATION_REQUEST 1
#define MB_HEADER_TAG_ADDRESS 2
#define MB_HEADER_TAG_ENTRY_ADDRESS 3
#define MB_HEADER_TAG_FLAGS 4
#define MB_HEADER_TAG_FRAMEBUFFER 5
#define MB_HEADER_TAG_MODULE_ALIGNMENT 6
#define MB_HEADER_TAG_EFI_BOOT_SERVIES 7
#define MB_HEADER_TAG_EFI_I386_ENTRY_ADDRESS 8
#define MB_HEADER_TAG_EFI_AMD64_ENTRY_ADDRESS 9
#define MB_HEADER_TAG_RELOCATABLE_HEADER 10


typedef struct __attribute__((__packed__)) mb_info_t {
    uint32_t total_size;
    uint32_t reserved;
    uint8_t tags[];
} __attribute__((aligned(8))) mb_info_t;

/* Multiboot Info Tags */

typedef struct __attribute__((__packed__)) mb_info_tag_t {
    uint32_t type;
    uint32_t size;
} __attribute__((aligned(8))) mb_info_tag_t;

typedef struct __attribute__((__packed__)) mb_info_tag_memory_info_t {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((aligned(8))) mb_info_tag_memory_info_t;

typedef struct __attribute__((__packed__)) mb_info_tag_boot_device_t {
    uint32_t type;
    uint32_t size;
    uint32_t biosdev;
    uint32_t partition;
    uint32_t sub_partition;
} __attribute__((aligned(8))) mb_info_tag_boot_device_t;

typedef struct __attribute__((__packed__)) mb_info_tag_command_line_t {
    uint32_t type;
    uint32_t size;
    uint8_t string[];
} __attribute__((aligned(8))) mb_info_tag_command_line_t;

typedef struct __attribute__((__packed__)) mb_info_tag_modules_t {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    uint8_t string[];
} __attribute__((aligned(8))) mb_info_tag_modules_t;

typedef struct __attribute__((__packed__)) mb_info_tag_elf_symbols_t {
    uint32_t type;
    uint32_t size;
    uint16_t num;
    uint16_t entsize;
    uint16_t shndx;
    uint16_t reserved;
    uint8_t section_headers[];
} __attribute__((aligned(8))) mb_info_tag_elf_symbols_t;

typedef struct __attribute__((__packed__)) mb_memory_map_entry_t {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
}
mb_memory_map_entry_t;

typedef struct __attribute__((__packed__)) mb_info_tag_memory_map_t {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    mb_memory_map_entry_t entries[];
} __attribute__((aligned(8))) mb_info_tag_memory_map_t;

typedef struct __attribute__((__packed__)) mb_info_tag_bootloader_name_t {
    uint32_t type;
    uint32_t size;
    uint8_t string[];
} __attribute__((aligned(8))) mb_info_tag_bootloader_name_t;

typedef struct __attribute__((__packed__)) mb_info_tag_apm_table_t {
    uint32_t type;
    uint32_t size;
    uint16_t version;
    uint16_t cseg;
    uint32_t offset;
    uint16_t cseg_16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg_16_len;
    uint16_t dseg_len;
} __attribute__((aligned(8))) mb_info_tag_apm_table_t;

typedef struct __attribute__((__packed__)) mb_info_tag_vbe_info_t {
    uint32_t type;
    uint32_t size;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint8_t vbe_control_info[512];
    uint8_t vbe_mode_info[256];
} __attribute__((aligned(8))) mb_info_tag_vbe_info_t;

typedef struct __attribute__((__packed__)) mb_color_descriptor {
    uint8_t red_value;
    uint8_t green_value;
    uint8_t blue_value;
} mb_color_descriptor;

typedef struct __attribute__((__packed__)) mb_info_tag_framebuffer_info_t {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
    union {
        struct {
            uint32_t framebuffer_pallette_num_colors;
            mb_color_descriptor framebuffer_pallette[];
        };
        struct {
            uint8_t framebuffer_red_field_position;
            uint8_t framebuffer_red_mask_size;
            uint8_t framebuffer_green_field_position;
            uint8_t framebuffer_green_mask_size;
            uint8_t framebuffer_blue_field_position;
            uint8_t framebuffer_blue_mask_size;
        };
    };
} __attribute__((aligned(8))) mb_info_tag_framebuffer_info_t;

typedef struct __attribute__((__packed__)) mb_info_tag_efi_i386_system_table_t {
    uint32_t type;
    uint32_t size;
    uint32_t pointer;
} __attribute__((aligned(8))) mb_info_tag_efi_i386_system_table_t;

typedef struct __attribute__((__packed__)) mb_info_tag_efi_amd64_system_table_t {
    uint32_t type;
    uint32_t size;
    uint32_t pointer;
} __attribute__((aligned(8))) mb_info_tag_efi_amd64_system_table_t;

typedef struct __attribute__((__packed__)) mb_info_tag_smbios_tables_t {
    uint32_t type;
    uint32_t size;
    uint8_t major;
    uint8_t minor;
    uint8_t reserved[6];
    uint8_t smbios_tables[];
} __attribute__((aligned(8))) mb_info_tag_smbios_tables_t;

typedef struct __attribute__((__packed__)) mb_info_tag_acpi_1_0_rsdp_t {
    uint32_t type;
    uint32_t size;
    uint8_t rsdp[];
} __attribute__((aligned(8))) mb_info_tag_acpi_1_0_rsdp_t;

typedef struct __attribute__((__packed__)) mb_info_tag_acpi_2_0_rsdp_t {
    uint32_t type;
    uint32_t size;
    uint8_t rsdp[];
} __attribute__((aligned(8))) mb_info_tag_acpi_2_0_rsdp_t;

typedef struct __attribute__((__packed__)) mb_info_tag_dhcp_network_info_t {
    uint32_t type;
    uint32_t size;
    uint8_t dhcp_ack[];
} __attribute__((aligned(8))) mb_info_tag_dhcp_network_info_t;

typedef struct __attribute__((__packed__)) mb_info_tag_efi_memory_map_t {
    uint32_t type;
    uint32_t size;
    uint32_t descriptor_size;
    uint32_t descriptor_version;
    uint8_t efi_memory_map[];
} __attribute__((aligned(8))) mb_info_tag_efi_memory_map_t;

typedef struct __attribute__((__packed__)) mb_info_tag_efi_boot_services_not_terminated_t {
    uint32_t type;
    uint32_t size;
} __attribute__((aligned(8))) mb_info_tag_efi_boot_services_not_terminated_t;

typedef struct __attribute__((__packed__)) mb_info_tag_efi_i386_image_handle_t {
    uint32_t type;
    uint32_t size;
    uint32_t pointer;
} __attribute__((aligned(8))) mb_info_tag_efi_i386_image_handle_t;

typedef struct __attribute__((__packed__)) mb_info_tag_efi_amd64_image_handle_t {
    uint32_t type;
    uint32_t size;
    uint32_t pointer;
} __attribute__((aligned(8))) mb_info_tag_efi_amd64_image_handle_t;

typedef struct __attribute__((__packed__)) mb_info_tag_image_load_base_address_t {
    uint32_t type;
    uint32_t size;
    uint32_t load_base_addr;
} __attribute__((aligned(8))) mb_info_tag_image_load_base_address_t;


/* Multiboot Header Tags */

typedef struct __attribute__((__packed__)) mb_header_tag_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} __attribute__((aligned(8))) mb_header_tag_t;

typedef struct __attribute__((__packed__)) mb_header_tag_information_request_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t mbi_tag_types[];
} __attribute__((aligned(8))) mb_header_tag_information_request_t;

typedef struct __attribute__((__packed__)) mb_header_tag_address_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
} __attribute__((aligned(8))) mb_header_tag_address_t;

typedef struct __attribute__((__packed__)) mb_header_tag_entry_address_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t entry_addr;
} __attribute__((aligned(8))) mb_header_tag_entry_address_t;

typedef struct __attribute__((__packed__)) mb_header_tag_efi_i386_entry_address_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t entry_addr;
} __attribute__((aligned(8))) mb_header_tag_efi_i386_entry_address_t;

typedef struct __attribute__((__packed__)) mb_header_tag_efi_amd64_entry_address_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t entry_addr;
} __attribute__((aligned(8))) mb_header_tag_efi_amd64_entry_address_t;

typedef struct __attribute__((__packed__)) mb_header_tag_flags_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t console_flags;
} __attribute__((aligned(8))) mb_header_tag_flags_t;

typedef struct __attribute__((__packed__)) mb_header_tag_framebuffer_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} __attribute__((aligned(8))) mb_header_tag_framebuffer_t;

typedef struct __attribute__((__packed__)) mb_header_tag_module_alignment_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} __attribute__((aligned(8))) mb_header_tag_module_alignment_t;

typedef struct __attribute__((__packed__)) mb_header_tag_efi_boot_services_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} __attribute__((aligned(8))) mb_header_tag_efi_boot_services_t;

typedef struct __attribute__((__packed__)) mb_header_tag_relocatable_header_t {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t min_addr;
    uint32_t max_addr;
    uint32_t align;
    uint32_t preference;
} __attribute__((aligned(8))) mb_header_tag_relocatable_header_t;


typedef struct __attribute__((__packed__)) mb_header_common_t {
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
} __attribute__((aligned(8))) mb_header_common_t;


mb_info_tag_t* mb_info_find_tag(mb_info_t* mb_info, uint32_t tag_type);