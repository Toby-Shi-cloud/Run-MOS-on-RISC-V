OUTPUT_ARCH(riscv)

ENTRY(_start)

SECTIONS {
    . = 0x80200000;

    .text : ALIGN(0x1000) {
        _text = .;
        *(.text.start)
        *(.text)
    }

    /* Porting note: use Vectored Interrupt mode in risc-v, 128-byte align is required. */
    .entry : ALIGN(128) {
        *(.text.entry)
    }

    .data : ALIGN(0x1000) {
        _data = .;
        *(.data .data* .sdata .sdata*)
    }

    .bss : ALIGN(0x1000) {
        _bss = .;
        *(.bss .bss* .sbss .sbss*)
    }

    _bss_end = .;
    . = 0x80600000;
    end = .;
}
