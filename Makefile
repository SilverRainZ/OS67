# makefile

.PHONY: install run dbg clean log
MAKE = make -r
AS = nasm
CC = gcc
DEL = del
DBG = bochs
VM = /usr/lib/virtualbox/VirtualBox --comment "OS67" --startvm "656a1a52-5a3b-4091-ae1a-437cb9c5284d"
DEL = del
LD = ld
OBJCPY = objcopy
CFLAGS = -Wall -Werror -nostdinc -fno-builtin -fno-stack-protector -funsigned-char \
		 -finline-functions -finline-small-functions -findirect-inlining \
		 -finline-functions-called-once -I./kern -m32 
OBJS = bin/loader.o bin/main.o bin/vga.o bin/func.o bin/gdt.o bin/idt.o bin/isrs.o
DEL = rm -f

default:
	$(MAKE) install

bin/floppy.img: boot/floppy.asm bin/bootsect.bin bin/kernel 
	$(AS) -I ./bin/ -f bin -l lst/floppy.lst $< -o $@ 

bin/bootsect.bin: boot/bootsect.asm 
	$(AS) -I ./boot/ -f bin -l lst/bootsect.lst $< -o $@ 

bin/loader.o : kern/loader.asm Makefile 
	$(AS) -I ./boot/ -f elf -l lst/loader.lst $< -o $@ 

bin/kernel: kern/link.ld $(OBJS) 
	$(LD) -T$< -melf_i386 -static -o $@ $(OBJS)

# bin/kernel: bin/kernel.tmp
# 	$(OBJCPY) -O binary $< $@ 

bin/%.o: kern/%.c
	$(CC) $(CFLAGS) -c $^ -o $@  
	$(CC) $(CFLAGS) -S $^ -o lst/$*.asm  
		
install:
	$(MAKE) bin/floppy.img

vb:
	$(VM)   # run with virtualbox
    
run:
	$(DBG) -q -f set/bochsrc.bxrc    # debug with shell

dbg:
	$(DBG) -q -f set/dbg_bochsrc.bxrc # debug with X GUI

clean: 
	$(DEL) bin/*.lst 
	$(DEL) bin/*.o 
	$(DEL) bin/*.bin 
	$(DEL) bin/*.tmp 
	$(DEL) bin/kernel 
	$(DEL) bin/floppy.img

log:
	$(DEL) lst/*

