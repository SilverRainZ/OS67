OS67
===============================
A Simple Operate System.

67 is the magic number :)

####Requirements
* make
* gcc
* binutils
* nasm
* mkfs
* bochs/qemu
     
####Compile  
    ➜  git clone https://github.com/LastAvenger/OS67.git
    ➜  cd OS67
    ➜  make init     # for first time
    ➜  make fs       # build root file system
    ➜  make          
    ➜  make run

####TODO
- [x] Boot sector
- [x] Protect mode
- [x] Driver
- [x] Paging 
- [x] Memory alloc/free
- [x] Process
- [x] File system
- [ ] Syscall
- [ ] Interface and user programs

####References
* [《30天自制操作系统》](http://book.douban.com/subject/11530329/)
* [《Orange's 一个操作系统的实现》](http://book.douban.com/subject/3735649/)
* [Bran's Kernel Developments Toturial](http://www.osdever.net/bkerndev/Docs/gettingstarted.htm) and [中文译版](http://article.yeeyan.org/view/197439/161890)
* [《Linux 内核完全注释》](http://book.douban.com/subject/1231236/)
* [wiki.osdev.org](http://wiki.osdev.org/Main_Page)
* [hurlex-doc](https://github.com/hurley25/hurlex-doc)
* [fleurix](https://github.com/Fleurer/fleurix)
* [xv6 中文文档](https://github.com/ranxian/xv6-chinese)
* [MIT 6.828 xv6](http://pdos.csail.mit.edu/6.828/2011/xv6.html)

####Thanks to
* fleuria
* 郭家华
* farseerfc
* fixme
* nami

####Just for fun. 
