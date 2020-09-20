OUTPUT_FORMAT("binary")
ENTRY(_kernel_start)

SECTIONS {
         /* kernel is located at 255TiB (virtual/linear address) */
         . = 0xFFFFFF0000000000;

         .kernel_start_text BLOCK(4K) : {
           ${KERNELBINDIR}/*(.kernel_start_text)
         }
         .kernel_text BLOCK(4K) : {
           ${KERNELBINDIR}/*(.text)
         }
         .kernel_rodata BLOCK(4K) : {
           ${KERNELBINDIR}/*(.rodata) 
         }
         .kernel_data BLOCK(4K) : {
           ${KERNELBINDIR}/*(.data)
         }
         .kernel_bss BLOCK(4K) : {
           ${KERNELBINDIR}/*(COMMON)
           ${KERNELBINDIR}/*(.bss)
         }
         .phys_mmap BLOCK(4K) : {
           */(PHYS_MMAP)
           . = 0x8000000; /* reserve memory to handle up to 64GiB physical memory in 1-page regions */
         }
         .kernel_heap BLOCK(4K) : {
           */(KERNEL_HEAP)
         }
}
