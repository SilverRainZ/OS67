; name: loader.asm
; func: loader kenel; enter pm 32; set temp GDT
; 
;

%INCLUDE "pm.inc"

[BITS 16]
ORG 0x8000
JMP loader

msg_loader:
    DB "loader loaded."
    DB 13,10
    DB 0

temp_print16:
_loop:
    LODSB
    OR AL,AL
	JZ _done 
	MOV	AH,0x0e		
	MOV	BX,15		
	INT	0x10		
	JMP	_loop
_done:
    RET

loader:
    XOR ESI, ESI
    MOV SI,msg_loader
    CALL temp_print16
    JMP LABEL_BEGIN

; ==================================================================
[SECTION .gdt]
    LABEL_GDT:	        Descriptor 0,       0,                  0           ; null
    LABEL_DESC_CODE32:  Descriptor 0,       SegCode32Len - 1,   DA_C + DA_32; 非一致代码段
    LABEL_DESC_DATA0:   Descriptor 0,       SegData0Len - 1,    DA_DRW      ; data seg r0
    LABEL_DESC_VIDEO:   Descriptor 0B8000h, 0ffffh,             DA_DRW      ; video RAM

    GdtLen	EQU	$ - LABEL_GDT	; GDT len
    GdtPtr	DW	GdtLen - 1	    ; GDT limit
    DD	0		                ; GDT Base

; GDT Selector 
    SelectorCode32	EQU	LABEL_DESC_CODE32 - LABEL_GDT
    SelectorData0   EQU LABEL_DESC_DATA0 - LABEL_GDT
    SelectorVideo	EQU	LABEL_DESC_VIDEO - LABEL_GDT
; END of [SECTION .gdt]

[SECTION .s16]
LABEL_BEGIN:
    MOV	AX, CS
    MOV	DS, AX
    MOV	ES, AX
    MOV	SS, AX
    MOV	SP, 0x0100

    FillDesc CS, LABEL_SEG_CODE32, LABEL_DESC_CODE32
    FillDesc DS, LABEL_SEG_DATA0, LABEL_DESC_DATA0

    ; 为加载 GDTR 作准备
    XOR	EAX, EAX
    MOV	AX, DS
    SHL	EAX, 4
    ADD	EAX, LABEL_GDT		; EAX <- gdt 基地址
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

    JMP	dword SelectorCode32:0	; special, clear pipe-line and jump 
; END of [SECTION .s16]
 
[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS 32]

LABEL_SEG_CODE32:
    MOV AX, SelectorData0
    MOV DS, AX

    MOV	AX, SelectorVideo
    MOV	GS, AX	 
    MOV	EDI,(160 * 2) + 0  	; 160 * 50  line 3 column 1 
    MOV	AH, 00001011b	; 0000: black 1100: cyan , i love this beautiful word

    XOR ESI,ESI         ; be careful !
    MOV SI, msg_GDT
    CALL temp_print32 

    JMP $

temp_print32:
    ADD EDI, 160
    PUSH EDI
    CLD
loop:
    LODSB
    ;MOV AL,[SI]
    ;ADD SI,1
    CMP AL, 0
    JE outloop
    MOV	[GS:EDI], AX
    ADD EDI,2
    JMP loop

outloop:
    POP EDI
    RET

SegCode32Len    EQU	$ - LABEL_SEG_CODE32

[SECTION .data0]
LABEL_SEG_DATA0:

_msg_GDT:
    DB "GDT loaded."
    DB  0
msg_GDT EQU _msg_GDT - $$

SegData0Len  EQU $ - LABEL_SEG_DATA0

