SECTIONS {
	. = 0;
	.text : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.bss : {
		*(.bss)
		*(COMMON)
	}
	.data : {
		*(.data)
	}
}
