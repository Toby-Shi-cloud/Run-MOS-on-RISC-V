OUTPUT_ARCH(riscv)

ENTRY(_start)

SECTIONS {
	. = 0x80200000;

	.text ALIGN(0x1000) : {
		_text = .;
		*(.text.start)
		*(.text.entry)
		*(.text)
		*(.rodata)
		*(.rodata.*)
	}

	.data ALIGN(0x1000) : {
		_data = .;
		*(.data .data* .sdata .sdata*)
	}

	.bss ALIGN(0x1000) : {
		_bss = .;
		*(.bss .bss* .sbss .sbss*)
	}

	_bss_end = .;
	. = 0x80600000;
	end = .;
}
