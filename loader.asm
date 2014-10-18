; a loader to load kernel & others
[BITS 16]
ORG 0x8000 
JMP loader

label1 ;-----------------------
msg:
    DB "loader loaded."

len EQU ($ - msg) 
loader:
    MOV	AX,0
    MOV	ES,AX
    MOV	AX,msg
    MOV	BP,AX	    ; ES:BP addr
    MOV	CX,0x000F
    MOV	AX,0x1301	;AH = 0x13，AL = 0x01
    MOV	BX,0x000c	;Page Number BH = 0 黑底红字 BL = 0x0c
    MOV	DL,0
    INT	0x10
    JMP $

label2 ;------------------------
%INCLUDE "pm.inc"
ProtectMode:
[SECTION .gdt]
    LABEL_GDT:	        Descriptor 0, 0, 0           ; 空描述符
    LABEL_DESC_CODE32:  Descriptor 0, SegCode32Len - 1, DA_C + DA_32; 非一致代码段
    LABEL_DESC_VIDEO:   Descriptor 0xb8000, 0xffff, DA_DRW	     ; 显存首地址

    GdtLen	EQU	$ - LABEL_GDT	; GDT长度
    GdtPtr	DW	GdtLen - 1	    ; GDT界限
    DD	0		        ; GDT基地址

    ; GDT 选择子
    SelectorCode32	EQU	LABEL_DESC_CODE32 - LABEL_GDT
    SelectorVideo	EQU	LABEL_DESC_VIDEO - LABEL_GDT
    ; END of [SECTION .gdt]

[SECTION .s16]
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
    SHR	EAX, 16
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
    MOV SI, msg
loop:
    MOV	AL, [SI]
    CMP AL, 0
    JE  outloop
    MOV	[GS:EDI], AX
    ADD SI, 1 
    ADD EDI, 2
    JMP loop

outloop:
    JMP	$

    SegCode32Len    EQU	$ - LABEL_SEG_CODE32
    ; END of [SECTION .s32]

msg:
    DB  "GDT loaded."
    DB  0
