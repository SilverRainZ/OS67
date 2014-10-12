# makefile

MAKE = make -r
NASM = D:\Users\LastAvengers\AppData\Local\nasm\nasm.exe
GCC = gcc
VIRTUALBOX = 
SHORTCUT = "D:\Program Files\Oracle\VirtualBox\VirtualBox.exe" --comment "CDDS" --startvm "15140001-0014-433b-be28-8699f80f4d22"
DEL = del

default:
	$(MAKE) install
	$(MAKE) run 

floppy.img: floppy.asm bootsect.bin makefile
	$(NASM) floppy.asm -f bin -l floppy.lst -o floppy.img

bootsect.bin: bootsect.asm makefile init.bin
	$(NASM) bootsect.asm -f bin -l bootsect.lst -o bootsect.bin

init.bin: init.asm makefile
	$(NASM) init.asm -f bin -l init.lst -o init.bin
	
install:
	$(MAKE) floppy.img

run:
	$(SHORTCUT)

clean: 
	$(DEL) *.bin
	$(DEL) *.img
	$(DEL) *.lst
	$(DEL) *.*~
	$(DEL) *~

