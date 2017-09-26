ENTRY(_boot_start)

SECTIONS {
         . = 1M;

         _boot_begin_addr = .;
         
         .multiboot ALIGN(8) : {
           ${MBHEADEROBJ}
         }
         .boot_text ALIGN(4K) : {
           ${BOOTBINDIR}/*(.text)
         }
         .boot_rodata ALIGN(4K) : {
           ${BOOTBINDIR}/*(.rodata)
         }
         .boot_data ALIGN(4K) : {
           ${BOOTBINDIR}/*(.data)
         }
         .boot_bss ALIGN(4K) : {
           ${BOOTBINDIR}/*(COMMON)
           ${BOOTBINDIR}/*(.bss)
         }

         _boot_end_addr = .;


         /* kernel is located at 255TiB (virtual/linear address) */
         . = 0xFFFFFF0000000000;
         _kernel_begin_linaddr = .;
         _kernel_begin_physaddr = _boot_end_addr;

         .kernel_text ALIGN(4K) : AT(ALIGN(_kernel_begin_physaddr, 4K)) {
           ${KERNELBINDIR}/*(.text)
         }
         .kernel_rodata ALIGN(4K) : AT(ALIGN(LOADADDR(.kernel_text)+SIZEOF(.kernel_text), 4K)) {
           ${KERNELBINDIR}/*(.rodata) 
         }
         .kernel_data ALIGN(4K) : AT(ALIGN(LOADADDR(.kernel_rodata)+SIZEOF(.kernel_rodata), 4K)) {
           ${KERNELBINDIR}/*(.data)
         }
         .kernel_bss ALIGN(4K) : AT(ALIGN(LOADADDR(.kernel_data)+SIZEOF(.kernel_data), 4K)) {
           ${KERNELBINDIR}/*(COMMON)
           ${KERNELBINDIR}/*(.bss)
         }

         _kernel_end_linaddr = .;
         _kernel_end_physaddr =  LOADADDR(.kernel_bss) + SIZEOF(.kernel_bss);
}
