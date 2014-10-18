;boot sector
%INCLUDE "textmode.inc"
[BITS 16]
OS_TEMP_STACK   EQU     0x7e00
OS_LOADER   EQU     0x8000
VRAM_TEXTMODE   EQU 0xb800
ORG	0x7c00		

BS_jmpBoot      JMP	entry
                DB	0x90
BS_OEMName      DB	"OS67CDDS"  ; OEM name / 8 B
BPB_BytsPerSec  DW	512		    ; 一个扇区512字节 
BPB_SecPerClus  DB	1		    ; 每个簇一个扇区
BPB_RsvdSecCnt  DW	1	        ; 保留扇区数, 必须为1
BPB_NumFATs     DB	2		    ; FAT表份数
BPB_RootEntCnt  DW	224		    ; 根目录项数
BPB_TotSec16    DW	2880		; RolSec16, 总扇区数
BPB_Media       DB	0xf0		; 介质种类: 移动介质
BPB_FATSz16     DW	9		    ; FATSz16 分区表占用扇区数
BPB_SecPerTrk   DW	18	        ; SecPerTrk, 磁盘 
BPB_NumHeads    DW	2	        ; 磁头数	
BPB_HiddSec     DD	0		    ; HiddSec
BPB_TotSec32    DD	2880		; 卡容量
BS_DrvNum       DB	0           ; DvcNum
BS_Reserved1    DB  0           ; NT保留    
BS_BootSig      DB  0x29        ; BootSig扩展引导标记
BS_VolD         DD	0xffffffff	; VolID 
BS_VolLab       DB	"FLOPPYCDDS "   ; 卷标
BS_FileSysType  DB	"FAT12   "	    ; FilesysType

TIMES 18 DB 0

; executable code 
entry:
    XOR AX,AX
    MOV DS,AX
    MOV SS,AX   ; segment is 0
    MOV SP,OS_TEMP_STACK    ; 0x7e00 -> 0x7c00
    
    MOV AX,VRAM_TEXTMODE
    MOV ES,AX
    

    

msg:
    DB	"bootsect loaded."
    DB  13,10
    DB	0

loadloader:
    JMP $

TIMES 510 - ($-$$) DB 0
DB	0x55, 0xaa

