; name: loader.asm
; func: loader kenel; enter pm 32; set temp GDT
; 
;

%INCLUDE "pm.inc"

[BITS 16]
org 0x8000
jmp loader

msg_loader:
    db "loader loaded."
    db 13,10
    db 0

temp_print16:
_loop:
    lodsb
    OR al,al
	JZ _done 
	mov	ah,0x0e		
	mov	bx,15		
	int	0x10		
	jmp	_loop
_done:
    RET

loader:
    xor esi, esi
    mov si,msg_loader
    call temp_print16
    jmp BEGIN

; ==================================================================
[SECTION .gdt]
GDT:
DESC_NULL:        Descriptor 0,       0,                  0             ; null
DESC_CODE32_R0:   Descriptor 0,       SegCode32Len - 1,   DA_C+DA_32    ; 非一致代码段
DESC_CODE16_R0:   Descriptor 0,       0xffff,             DA_C		    ; 非一致,16
DESC_DATA_R0:     Descriptor 0,       SegData0Len - 1,    DA_DRW        ; data seg r0
DESC_VIDEO_R0:    Descriptor 0xb8000, 0xffff,             DA_DRW        ; video RAM
DESC_STACK_R0:    Descriptor 0,       TopOfStack_R0,      DA_DRWA+DA_32
DESC_STACK_R3:    Descriptor 0,       TopOfStack_R3,      DA_DRWA+DA_32+DA_DPL3

GdtLen	equ	$ - GDT	; GDT len
GdtPtr	DW	GdtLen - 1	    ; GDT limit
DD	0		                ; GDT Base

; GDT Selector 
Selec_Code32_R0	equ	DESC_CODE32_R0    - DESC_NULL
Selec_Data_R0   equ DESC_DATA_R0      - DESC_NULL 
Selec_Video_R0	equ	DESC_VIDEO_R0     - DESC_NULL
Selec_Stack_R0  equ	DESC_STACK_R0     - DESC_NULL
Selec_Stack_R3  equ	DESC_STACK_R3     - DESC_NULL
; END of [SECTION .gdt]

[SECTION .s16]
BEGIN:
    mov	ax, CS
    mov	DS, ax
    mov	ES, ax
    mov	SS, ax
    mov	SP, 0x7c00  ; temp 

    FillDesc CS, SEG_CODE32_R0, DESC_CODE32_R0
    FillDesc DS, SEG_DATA_R0, DESC_DATA_R0
    FillDesc DS, SEG_STACK_R0, DESC_STACK_R0
    FillDesc DS, SEG_STACK_R3, DESC_STACK_R3

    ; 为加载 GDTR 作准备
    xor	eax, eax
    mov	ax, DS
    SHL	eax, 4
    add	eax, GDT		; eax <- gdt 基地址
    mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

    LGDT	[GdtPtr]
    CLI

    IN	al, 0x92    ; turn on A20 line
    OR	al, 00000010b
    OUT	0x92, al

    ; 准备切换到保护模式
    mov	eax, cr0
    OR	eax, 1
    mov	cr0, eax

    jmp	dword Selec_Code32_R0:0	; special, clear pipe-line and jump 
; END of [SECTION .s16]
 
[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS 32]

SEG_CODE32_R0:
    mov ax, Selec_Data_R0
    mov ds, ax
    mov ax, Selec_Stack_R0
    mov	ss, ax
    mov esp, TopOfStack_R0

    mov	ax, Selec_Video_R0 
    mov	gs, ax	 
    mov	edi,(160 * 2) + 0  	; 160 * 50  line 3 column 1 
    mov	ah, 00001011b	; 0000: black 1100: cyan , i love this beautiful word

    xor esi,esi         ; be careful !
    mov si, msg_GDT
    call temp_print32 

    jmp $

temp_print32:
    add edi, 160
    push edi
    cld
loop:
    lodsb
    ;mov al,[si]
    ;add si,1
    cmp al, 0
    je outloop
    mov	[gs:edi], ax
    add edi,2
    jmp loop

outloop:
    pop edi
    ret

SegCode32Len    equ	$ - SEG_CODE32_R0

[SECTION .data0]
SEG_DATA_R0:

_msg_GDT:
    db "GDT loaded."
    db  0
msg_GDT equ _msg_GDT - $$

SegData0Len  equ $ - SEG_DATA_R0

[SECTION .stack]
SEG_STACK_R0:   ; ring 0 stack segment 
    times 512 db 0
TopOfStack_R0 equ $ - SEG_STACK_R0 - 1

SEG_STACK_R3: ; ring 3 stack segment
	times 512 db 0
TopOfStack_R3 equ $ - SEG_STACK_R3 - 1

