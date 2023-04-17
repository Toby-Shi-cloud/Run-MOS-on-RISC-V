OUTPUT_ARCH(riscv)

ENTRY(_start)

SECTIONS {
    kernel_start = .;
    . = 0x80200000;

    ._start : {
        *(.text._start)
    }

    .text : {
        *(.boot)
        *(.text)
    }

    .data : {
        *(.data .data* .sdata .sdata*)
    }

    .bss  : {
        *(.bss .bss* .sbss .sbss*)
    }
    bss_end = .;
    . = 0x80600000;
    end = . ;
}
