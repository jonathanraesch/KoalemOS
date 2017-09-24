ENTRY(start_kernel)

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
         _kernel_begin_linaddr = 0xFF0000000000;
         _kernel_begin_physaddr = .;

         .text ALIGN(_boot_end_addr, 4K) : AT(ALIGN(4K)) {
           ${KERNELBINDIR}/*(.text)
         }
         .rodata ALIGN(ADDR(.text)+SIZEOF(.text), 4K) : AT(ALIGN(4K)) {
           ${KERNELBINDIR}/*(.rodata)
         }
         .data ALIGN(ADDR(.rodata)+SIZEOF(.rodata), 4K) : AT(ALIGN(4K)) {
           ${KERNELBINDIR}/*(.data)
         }
         .bss ALIGN(ADDR(.data)+SIZEOF(.data), 4K) : AT(ALIGN(4K)) {
           ${KERNELBINDIR}/*(COMMON)
           ${KERNELBINDIR}/*(.bss)
         }

         _kernel_end_linaddr = ADDR(.bss) + SIZEOF(.bss);
         _kernel_end_physaddr =  .;
}
