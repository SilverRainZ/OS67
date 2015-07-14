OS67
===============================
A Simple Operate System.

67 is the magic number :)

####Requirements
* gcc + nasm + ld + make
* bochs
* qemu
     
####How to compile  
          ➜  ~  git clone https://github.com/LastAvenger/OS67.git
          ➜  OS67 cd OS67
          ➜  OS67 make init     # for first time
          ➜  OS67 make          # compile
          ➜  OS67 make run      # run
          ➜  OS67 make dbg      # debug

####TODO
- [x] Boot Sector
- [x] Protect Mode
- [x] Paging 
- [x] Mem alloc 
- [ ] Process schedule
- [ ] File System
- [ ] Executable file

####References
* Bran's Kernel Developments Toturial
* wiki.osdev.org
* 30天自制操作系统
* Orange's 一个操作系统的诞生
* hurlex-doc
* xv6

####Just for fun. 
