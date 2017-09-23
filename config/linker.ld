ENTRY(start_kernel)

SECTIONS {
         . = 1M;
         
         .multiboot ALIGN(8) : {
           ${MBHEADEROBJ}
         }
         .text ALIGN(4K) : {
           ${BOOTBINDIR}/*(.text)
         }
         .rodata ALIGN(4K) : {
           ${BOOTBINDIR}/*(.rodata)
         }
         .data ALIGN(4K) : {
           ${BOOTBINDIR}/*(.data)
         }
         .bss ALIGN(4K) : {
           ${BOOTBINDIR}/*(COMMON)
           ${BOOTBINDIR}/*(.bss)
         }
}
