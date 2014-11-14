# makefile

MAKE = make -r
AS = nasm
CC = gcc
DEL = del
DBG = bochsdbg
VM = bochs
DEL = del
LD = ld
OBJCPY = objcopy
CFLAG = -Wall -Werror -nostdinc -fno-builtin -fno-stack-protector \
		 -finline-functions -finline-small-functions -findirect-inlining \
		 -finline-functions-called-once
OBJS = bin/loader.o bin/main.o

default:
	$(MAKE) install

bin/floppy.img: boot/floppy.asm bin/bootsect.bin bin/kernel 
	$(AS) -I ./bin/ -f bin -l lst/floppy.lst $< -o $@ 

bin/bootsect.bin: boot/bootsect.asm 
	$(AS) -I ./boot/ -f bin -l lst/bootsect.lst $< -o $@ 

bin/loader.o : kern/loader.asm makefile 
	$(AS) -I ./boot/ -f elf -l lst/loader.lst $< -o $@ 

bin/kernel.tmp: kern/link.ld $(OBJS) 
	$(LD) -T$< -static -o $@ $(OBJS)

bin/kernel: bin/kernel.tmp
	$(OBJCPY) -O binary $< $@ 

bin/main.o: kern/main.c
	$(CC) $(CFLAG) -c $^ -o $@  
		
install:
	$(MAKE) bin/floppy.img

# maybe a bug of MinGW32-make.exe, you can't use bochs -q -f xxxx directly
run:	
	call set/_bochs.bat

dbg:
	call set/_bochs.bat dbg

# using del *.* or * is dangerous
clean: 
	@$(DEL) bin\*.bin
	@$(DEL) bin\*.lst
	@$(DEL) bin\*.o
	@$(DEL) bin\*.bin
	@$(DEL) bin\kernel
	@$(DEL) bin\floppy.img

log:
	@$(DEL) lst\*.lst
	@$(DEL) lst\*.log

.PHONY: install run dbg clean log
