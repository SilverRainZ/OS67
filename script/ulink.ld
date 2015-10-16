OUTPUT_FORMAT("elf32-i386")
ENTRY(main)
va = 0xc0000000;
SECTIONS
{
  .text va : AT(va) 
  {
    code = .;
    *(.text)
    *(.rodata)
    . = ALIGN(0x1000); 
  }
  .data : AT(va + (data - code)) 
  {
    data = .;
    *(.data)
    . = ALIGN(0x1000); 
  }
  .stab : AT(va + (stab - code))
  {
      stab = .;
      *(.stab)
      . = ALIGN(0x1000);
  }
  .stabstr : AT(va + (stabstr - code))
  {
      stabstr = .;
      *(.stabstr)
      . = ALIGN(0x1000);
  }
  .bss : AT(va + (bss - code)) 
  {
    bss = .;
    *(.bss)
    . = ALIGN(0x1000); 
  }

  /DISCARD/ : { *(.comment) *(.eh_frame) }
}
