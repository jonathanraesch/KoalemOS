ENTRY(_kernel_start)

SECTIONS {
	/* mapping of all/first 64GB physical addresses starting at 255TiB */
	/* kernel starting at 255TiB + 64GB */
	. = 0xFFFFFF1000000000;
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
	.tdata : {
		tls_image = ABSOLUTE(.);
		*(.tdata)
	}
	.tbss : {
		*(.tbss)
		. = ALIGN(8);
		__tls_image_end = ABSOLUTE(.);
	}
	__tls_image_size_sym = __tls_image_end - tls_image;
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
