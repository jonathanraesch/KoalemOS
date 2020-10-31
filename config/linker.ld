ENTRY(_kernel_start)

SECTIONS {
	/* kernel is located at 255TiB (virtual/linear address) */
	. = 0xFFFFFF0000000000;
	.text BLOCK(4K) : {
		*(.text)
	}
	.rodata BLOCK(4K) : {
		*(.rodata)
		*(.rodata.cst4)
		*(.rodata.cst8)
	}
	.data BLOCK(4K) : {
		*(.data)
	}
	.bss BLOCK(4K) : {
		*(.bss)
		*(COMMON)
	}
	.phys_mmap BLOCK(4K) : {
		*(PHYS_MMAP)
	}
	. += 0x1000000;
	.heap BLOCK(4K) : {
		*(KERNEL_HEAP)
	}
}
