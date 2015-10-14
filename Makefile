# makefile

.PHONY: init install run fs bochs qemu clean log count
MAKE = make -r
AS = nasm
CC = gcc
DEL = rm -f
DBG = bochs
BOCHS = bochs
QEMU = qemu
LD = ld
OBJCPY = objcopy
CFLAGS = -c -O0 -Wall -Werror -nostdinc -fno-builtin -fno-stack-protector -funsigned-char \
		 -finline-functions -finline-small-functions -findirect-inlining \
		 -finline-functions-called-once -Iinc -m32 -ggdb -gstabs+ 
OBJS = bin/loader.o \
	   bin/main.o bin/dbg.o bin/timer.o bin/asm.o \
	   bin/gdt.o bin/idt.o \
	   bin/isr.o bin/irq.o bin/fault.o bin/syscall.o \
	   bin/vga.o bin/kb.o bin/ide.o \
	   bin/string.o bin/vsprint.o bin/printk.o \
	   bin/pmm.o bin/vmm.o \
	   bin/bcache.o bin/sb.o bin/bitmap.o bin/inode.o \
	   bin/dir.o bin/p2i.o bin/fstest.o bin/file.o bin/sysfile.o \
	   bin/init.o bin/proc.o bin/sysproc.o bin/exec.o \
	   bin/pipe.o bin/dev.o bin/con.o

UDEPS = bin/usys.o bin/uio.o bin/string.o bin/vsprint.o

UOBJS = $(UPROGS:%=%.o)

UPROGS =  bin/cinit bin/sh bin/cat bin/ls

# create a 1.44MB floppy include kernel and bootsector
bin/floppy.img: boot/floppy.asm bin/bootsect.bin bin/kernel 
	$(AS) -I ./bin/ -f bin -l lst/floppy.s $< -o $@ 

# bootsector
bin/bootsect.bin: boot/bootsect.asm 
	$(AS) -I ./boot/ -f bin -l lst/bootsect.s $< -o $@ 

bin/loader.o : kern/loader.asm
	$(AS) -I ./boot/ -f elf32 -g -F stabs -l lst/loader.s $< -o $@ 

bin/init.o: proc/init.asm 
	$(AS) -I ./boot/ -f elf32 -g -F stabs -l lst/init.s $< -o $@ 

# link loader.o and c objfile 
# generate a symbol file(kernel.elf) and a flat binary kernel file(kernel)
bin/kernel: script/link.ld $(OBJS) 
	$(LD) -T$< -melf_i386 -static -o $@.elf $(OBJS) -M>lst/map.map
	$(OBJCPY) -O binary $@.elf $@

# compile c file in all directory
bin/%.o: */%.c
	$(CC) $(CFLAGS) -c $^ -o $@  
	$(CC) $(CFLAGS) -S $^ -o lst/$*.s

bin/usys.o: usr/usys.asm
	$(AS) -f elf32 -g -F stabs -l lst/usys.s $< -o $@ 

$(UPROGS): $(UDEPS)
	$(LD) -e main -Ttext 0xc0000000 -melf_i386 -static $+ $@.o -o $@ 

#----------------------------------------

init:
	sudo mkdir /mnt/kernel
	sudo mkdir /mnt/fs
	mkdir lst
	mkdir bin

default: Makefile
	$(MAKE) bin/floppy.img

# make a disk with minix v1 file system
fs: $(UOBJS) $(UDEPS)
	for i in $(UPROGS); do $(MAKE) $$i; done
	$(DEL) bin/rootfs.img
	bximage bin/rootfs.img -hd=10M -imgmode=flat -mode=create -q
	mkfs.minix bin/rootfs.img -1 -n14
	sudo mount -o loop -t minix bin/rootfs.img /mnt/fs
	sudo cp usr/README /mnt/fs/
	sudo cp usr/logo.txt /mnt/fs
	for i in $(UPROGS); do sudo cp $$i /mnt/fs; done
	sleep 1
	sudo umount /mnt/fs

# check root file system
fsck:
	fsck.minix -fsl ./bin/rootfs.img

# default run with bochs
run:
	$(DBG) -q -f script/bochsrc.bxrc -rc script/bochsinit

# debug with Bochs X GUI
bochs:
	$(BOCHS) -q -f script/dbg_bochsrc.bxrc -log lst/bochsdbgout.log

# debug with qemu and gdb
qemu: 
	$(QEMU) -S -s -fda bin/floppy.img -hda bin/rootfs.img -boot a &
	sleep 1
	cgdb -x script/gdbinit

# clean the binary file
clean: 
	$(DEL) bin/*.lst 
	$(DEL) bin/*.o 
	$(DEL) bin/*.bin 
	$(DEL) bin/*.tmp 
	$(DEL) bin/kernel 
	$(DEL) bin/kernel.elf
	$(DEL) bin/floppy.img

# clean list file under lst/
log:
	$(DEL) lst/*

count:
	./line.sh

