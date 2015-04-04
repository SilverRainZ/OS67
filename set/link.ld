OUTPUT_FORMAT("binary")
ENTRY(start)
phys = 0x100000;
SECTIONS
{
  .text phys : AT(phys) {
    kernstart = .;
    code = .;
    *(.text)
    *(.rodata)
    . = ALIGN(4096); 
  }
  .data : AT(phys + (data - code)) {
    data = .;
    *(.data)
    . = ALIGN(4096); 
  }
  .bss : AT(phys + (bss - code)) {
    bss = .;
    *(.bss)
    . = ALIGN(4096); 
  }
  kernend = .;
}
