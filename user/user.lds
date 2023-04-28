/*
 * Set the architecture to mips.
 */
OUTPUT_ARCH(riscv)

/*
 * Set the ENTRY point of the program to _start.
 */
ENTRY(_start)

PHDRS {
	code PT_LOAD FLAGS (5);
	data PT_LOAD FLAGS (6);
}

SECTIONS {
	. = 0x00400000;

	.text ALIGN(0x1000) : {
		*(.text)
		*(.rodata)
		*(.rodata.*)
	} : code

	.data ALIGN(0x1000) : {
		*(.data .data* .sdata .sdata*)
	} : data

	.bss ALIGN(0x1000) : {
		*(.bss .bss* .sbss .sbss*)
	} : data

	end = . ;
}
