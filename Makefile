# makefile

.PHONY: install run bochs qemu clean log
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
#OBJS = $(wildcard bin/*.o)
OBJS = bin/loader.o bin/main.o bin/vga.o bin/gdt.o bin/idt.o \
	   bin/isrs.o bin/irq.o bin/timer.o bin/asm.o bin/kb.o \
	   bin/string.o	bin/queue.o bin/printk.o bin/pmm.o bin/vmm.o \
	   bin/dbg.o bin/heap.o bin/task.o bin/sched.o

# create a 1.44MB floppy include kernel and bootsector
bin/floppy.img: boot/floppy.asm bin/bootsect.bin bin/kernel 
	$(AS) -I ./bin/ -f bin -l lst/floppy.asm $< -o $@ 

# bootsector
bin/bootsect.bin: boot/bootsect.asm 
	$(AS) -I ./boot/ -f bin -l lst/bootsect.asm $< -o $@ 

bin/loader.o : kern/loader.asm
	$(AS) -I ./boot/ -f elf32 -g -F stabs -l lst/loader.asm $< -o $@ 

# link loader.o and c objfile 
# generate a symbol file(kernel.elf) and a flat binary kernel file(kernel)
bin/kernel: set/link.ld $(OBJS) 
	$(LD) -T$< -melf_i386 -static -o $@.elf $(OBJS) -M>lst/map.map
	$(OBJCPY) -O binary $@.elf $@

# compile c file in all directory
bin/%.o: libs/%.c
	$(CC) $(CFLAGS) -c $^ -o $@  
	$(CC) $(CFLAGS) -S $^ -o lst/$*.asm  

bin/%.o: kern/%.c
	$(CC) $(CFLAGS) -c $^ -o $@  
	$(CC) $(CFLAGS) -S $^ -o lst/$*.asm  
#----------------------------------------

default: Makefile
	$(MAKE) bin/floppy.img

# copy kernel file to floppy
install : bin/floppy.img bin/kernel
	sudo mount bin/floppy.img /mnt/kernel
	sudo cp bin/kernel /mnt/kernel
	sleep 1
	sudo umount /mnt/kernel

# default run with bochs
run:
	$(DBG) -q -f set/bochsrc.bxrc 

# run with qemu 
run-qemu:
	$(QEMU) -fda bin/floppy.img -boot a

# debug with Bochs X GUI
bochs:
	$(BOCHS) -q -f set/dbg_bochsrc.bxrc 

# debug with qemu
qemu: 
	$(QEMU) -S -s -fda bin/floppy.img -boot a &
	sleep 1
	cgdb -x set/gdbinit

# clean the binary file
clean: 
	$(DEL) bin/*.lst 
	$(DEL) bin/*.o 
	$(DEL) bin/*.bin 
	$(DEL) bin/*.tmp 
	$(DEL) bin/kernel 
	$(DEL) bin/floppy.img

# clean list file under lst/
log:
	$(DEL) lst/*

