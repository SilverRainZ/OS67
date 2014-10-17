
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
	MOV	AX,0		
	MOV	SS,AX
	MOV	SP,0x7c00
	MOV	DS,AX
	MOV	ES,AX

	MOV	SI,msg1
putloop:
	MOV	AL,[SI]
	ADD	SI,1		
	CMP	AL,0
	JE  ProtectMode
	MOV	AH,0x0e		
	MOV	BX,15		
	INT	0x10		
	JMP	putloop

ProtectMode:
%INCLUDE "pm.inc"	; 常量, 宏, 以及一些说明

[SECTION .gdt]
LABEL_GDT:	        Descriptor 0, 0, 0           ; 空描述符
LABEL_DESC_CODE32:  Descriptor 0, SegCode32Len - 1, DA_C + DA_32; 非一致代码段
LABEL_DESC_VIDEO:   Descriptor 0xb8000, 0xffff, DA_DRW	     ; 显存首地址
; GDT 结束

GdtLen	EQU	$ - LABEL_GDT	; GDT长度
GdtPtr	DW	GdtLen - 1	    ; GDT界限
		DD	0		        ; GDT基地址

; GDT 选择子
SelectorCode32	EQU	LABEL_DESC_CODE32 - LABEL_GDT
SelectorVideo	EQU	LABEL_DESC_VIDEO - LABEL_GDT
; END of [SECTION .gdt]

[SECTION .s16]
[BITS	16]
LABEL_BEGIN:
	MOV	AX, CS
	MOV	DS, AX
	MOV	ES, AX
	MOV	SS, AX
	MOV	SP, 0x0100
	
	; 初始化 32 位代码段描述符
	XOR	EAX, EAX
	MOV	AX, CS
	SHL	EAX, 4
	ADD	EAX, LABEL_SEG_CODE32
	MOV	word [LABEL_DESC_CODE32 + 2], AX
	shr	EAX, 16
	MOV	byte [LABEL_DESC_CODE32 + 4], AL
	MOV	byte [LABEL_DESC_CODE32 + 7], AH
	
	; 为加载 GDTR 作准备
	XOR	EAX, EAX
	MOV	AX, DS
	SHL	EAX, 4
	ADD	EAX, LABEL_GDT		; eAX <- gdt 基地址
	MOV	dword [GdtPtr + 2], EAX	; [GdtPtr + 2] <- gdt 基地址
	
	LGDT	[GdtPtr]
	CLI

	IN	AL, 0x92    ; turn on A20 line
	OR	AL, 00000010b
	OUT	0x92, AL
	
	; 准备切换到保护模式
	MOV	EAX, CR0
	OR	EAX, 1
	MOV	CR0, EAX
	
	; JMP in ProtectMode!:w
	JMP	dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs,
    ; 并跳转到 Code32Selector:0  处
    ; END of [SECTION .s16]

[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS 32]

LABEL_SEG_CODE32:
    MOV	AX, SelectorVideo
    MOV	GS, AX			; 视频段选择子(目的)

    MOV	EDI, (80 * 1 + 0) * 2	; 屏幕第 2 行, 第 1 列。
    MOV	AH, 0x0c			; 0000: 黑底    1100: 红字
    MOV SI, msg2
putloop2:
    MOV	AL, [SI]
    CMP AL, 0
    JE  outloop2
    MOV	[GS:EDI], AX
    ADD SI, 1 
    ADD EDI, 2
    JMP putloop2

outloop2:
    ; 到此停止
    JMP	$

SegCode32Len	EQU	$ - LABEL_SEG_CODE32
    ; END of [SECTION .s32]

msg1:
    DB	"bootsect loaded."
    DB	0x0a		
    DB	0
msg2:
    DB  "GDT loaded."
    DB  0

    TIMES 510-($-$$) DB 0
    DB	0x55, 0xaa

