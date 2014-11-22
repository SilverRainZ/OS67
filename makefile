# makefile

.PHONY: install run dbg clean log
MAKE = make -r
AS = nasm
CC = gcc
DEL = del
DBG = bochsdbg
VM = bochs
DEL = del
LD = ld
OBJCPY = objcopy
CFLAGS = -Wall -Werror -nostdinc -fno-builtin -fno-stack-protector -funsigned-char \
		 -finline-functions -finline-small-functions -findirect-inlining \
		 -finline-functions-called-once -I./kern -m32 -g
OBJS = bin/loader.o bin/main.o bin/vga.o
DEL = rm -f

default:
	$(MAKE) install

bin/floppy.img: boot/floppy.asm bin/bootsect.bin bin/kernel 
	$(AS) -I ./bin/ -f bin -l lst/floppy.lst $< -o $@ 

bin/bootsect.bin: boot/bootsect.asm 
	$(AS) -I ./boot/ -f bin -l lst/bootsect.lst $< -o $@ 

bin/loader.o : kern/loader.asm makefile 
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

# maybe a bug of MinGW32-make.exe, you can't use bochs -q -f xxxx directly
run:
	$(VM) -q -f set/bochsrc.bxrc    # debug with shell

dbg:
	$(VM) -q -f set/dbg_bochsrc.bxrc # debug with X GUI

# using del *.* or * is dangerous
clean: 
	$(DEL) bin/*.lst 
	$(DEL) bin/*.o 
	$(DEL) bin/*.bin 
	$(DEL) bin/*.tmp 
	$(DEL) bin/kernel 
	$(DEL) bin/floppy.img

log:
	$(DEL) lst/*

