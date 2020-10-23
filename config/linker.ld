ENTRY(_kernel_start)

SECTIONS {
         /* kernel is located at 255TiB (virtual/linear address) */
         . = 0xFFFFFF0000000000;
         .kernel_text BLOCK(4K) : {
           ${KERNELBUILDDIR}/*(.text)
         }
         .kernel_rodata BLOCK(4K) : {
           ${KERNELBUILDDIR}/*(.rodata)
         }
         .kernel_data BLOCK(4K) : {
           ${KERNELBUILDDIR}/*(.data)
         }
         .kernel_bss BLOCK(4K) : {
           ${KERNELBUILDDIR}/*(COMMON)
           ${KERNELBUILDDIR}/*(.bss)
         }
         .phys_mmap BLOCK(4K) : {
           *(PHYS_MMAP)
           . = 0x100000;
         }
         .kernel_heap BLOCK(4K) : {
           *(KERNEL_HEAP)
         }
}
