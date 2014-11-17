ENTRY(start)
OUTPUT_FORMAT("binary")
SECTIONS
{
    .text 0x8000: {
        *(.text)
    }
    .data : {
        *(.rodata);
        *(.data);
    }
    .bss : {
        *(.bss)
    }
}


