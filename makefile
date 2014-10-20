# makefile

MAKE = make -r
ASM = nasm
CC = gcc
DEL = del
VM =  "D:\Program Files\Oracle\VirtualBox\VirtualBox.exe" --comment "CDDS" --startvm "15140001-0014-433b-be28-8699f80f4d22"
DBG = bochsdbg
BOCHS = bochs
DEL = del

default:
	$(MAKE) install

floppy.img: floppy.asm bootsect.bin loader.bin makefile
	$(ASM) floppy.asm -f bin -l floppy.lst -o floppy.img

bootsect.bin: bootsect.asm makefile 
	$(ASM) bootsect.asm -f bin -l bootsect.lst -o bootsect.bin

loader.bin: loader.asm makefile
	$(ASM) loader.asm -f bin -l loader.lst -o loader.bin

	
install:
	$(MAKE) floppy.img

run:
	$(VM)

dbg:
	$(DBG) -q -f DBG_bochsrc.bxrc -log boshsout.log

bochs:
	$(BOCHS) -q -f bochsrc.bxrc 
clean: 
	$(DEL) *.bin
	$(DEL) *.img
	$(DEL) *.lst
	$(DEL) *.*~
	$(DEL) *~

