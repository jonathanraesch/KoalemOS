ENTRY(_kernel_start)

SECTIONS {
	/* kernel is located at 255TiB (virtual/linear address) */
	. = 0xFFFFFF0000000000;
	.text : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
		*(.rodata.cst4)
		*(.rodata.cst8)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
		*(COMMON)
	}
	tls_image = .;
	.tdata : {
		*(.tdata)
	}
	.tbss : {
		*(.tbss)
	}
	__tls_image_end = .;
	__tls_image_size_sym = SIZEOF(.tdata) + SIZEOF(.tbss);
	__bsp_tls_buf = .;
	__tcb_size_sym = 8;
	. += __tls_image_size_sym + __tcb_size_sym;
	.phys_mmap BLOCK(4K) : {
		*(PHYS_MMAP)
	}
	__phys_mmap_size_sym = 0x1000000;
	. += __phys_mmap_size_sym;
	.heap BLOCK(4K) : {
		*(KERNEL_HEAP)
	}
}
