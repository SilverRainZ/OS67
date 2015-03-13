#2015-3-13 PROVIDE( kernstart = .); doesn't work
master 88bb2b4
Q:
* 仿照浅奕翻译的教程，在ld脚本中使用`PROVIDE( kernstart = .);` 和 `PROVIDE( kernend = .);`来定位内核的位置，但是均得不到正确的结果.
A:
* 最终使用`kernstart = . ;` 配合在pmm.h中定义 `uint8_t kernstart`，取得了比较靠谱的结果，但是内核大小达到了20KB? 我的kernel文件才12KB, 怀疑问题出在 floppy.s
