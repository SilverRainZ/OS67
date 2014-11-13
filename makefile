# makefile

MAKE = make -r
AS = nasm
CC = gcc
DEL = del
DBG = bochsdbg
VM = bochs
DEL = rm 

default:
	$(MAKE) install

floppy.img: floppy.asm bootsect.bin loader.bin makefile
	$(AS) floppy.asm -f bin -l floppy.lst -o floppy.img

bootsect.bin: bootsect.asm makefile 
	$(AS) bootsect.asm -f bin -l bootsect.lst -o bootsect.bin

loader.bin: loader.asm makefile main
	$(AS) loader.asm -f bin -l loader.lst -o loader.bin

main: main.c
	$(CC) -S -o main.lst main.c 
	$(CC) -c -o main.o  main.c 
	
install:
	$(MAKE) floppy.img

run:
	$(VM) -q -f bochsrc.bxrc 

dbg:
	$(DBG) -q -f DBG_bochsrc.bxrc -log boshsout.log

clean: 
	$(DEL) *.bin
	$(DEL) *.img
	$(DEL) *.lst
	$(DEL) *.o
	$(DEL) *.*~
	$(DEL) *~

