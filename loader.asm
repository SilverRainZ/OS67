; name: loader.asm
; func: loader kenel; enter pm 32; set temp GDT
; 
;

%INCLUDE "pm.inc"

[BITS 16]
ALIGN 4
[SECTION .text]

global start
start:
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


PageDirBase		equ	0x200000	; 页目录开始地址: 2M
PageTblBase		equ	0x201000	; 页表开始地址: 2M+4K

;[SECTION .gdt]
GDT:
DESC_NULL:        Descriptor 0,       0,                  0             ; null
DESC_CODE32_R0:   Descriptor 0,       SegCode32Len - 1,   DA_C+DA_32    ; 非一致代码段
DESC_CODE16_R0:   Descriptor 0,       0xffff,             DA_C		    ; 非一致,16
DESC_DATA_R0:     Descriptor 0,       SegData0Len - 1,    DA_DRW        ; data seg r0
DESC_VIDEO_R0:    Descriptor 0xb8000, 0xffff,             DA_DRW        ; video RAM
DESC_STACK_R0:    Descriptor 0,       TopOfStack_R0,      DA_DRWA+DA_32
DESC_STACK_R3:    Descriptor 0,       TopOfStack_R3,      DA_DRWA+DA_32+DA_DPL3
DESC_PAGE_DIR:	  Descriptor PageDirBase,   4095,         DA_DRW		; Page Directory
DESC_PAGE_TBL:    Descriptor PageTblBase,   1023,         DA_DRW|DA_LIMIT_4K; Page Tables

GdtLen	equ	$ - GDT	; GDT len
GdtPtr	DW	GdtLen - 1	    ; GDT limit
DD	0		                ; GDT Base

; GDT Selector 
Selec_Code32_R0	equ	DESC_CODE32_R0    - DESC_NULL
Selec_Data_R0   equ DESC_DATA_R0      - DESC_NULL 
Selec_Video_R0	equ	DESC_VIDEO_R0     - DESC_NULL
Selec_Stack_R0  equ	DESC_STACK_R0     - DESC_NULL
Selec_Stack_R3  equ	DESC_STACK_R3     - DESC_NULL
Selec_PageDir	equ	DESC_PAGE_DIR	  - DESC_NULL 
Selec_PageTbl	equ	DESC_PAGE_TBL	  - DESC_NULL
; END of [SECTION .gdt]

;[SECTION .s16]
BEGIN:
    mov	ax, cs
    mov	ds, ax
    mov	es, ax
    mov	ss, ax
    mov	sp, 0x7c00  ; temp 

    ; init segment descriptor 
    FillDesc cs, SEG_CODE32_R0, DESC_CODE32_R0
    FillDesc ds, SEG_DATA_R0,   DESC_DATA_R0
    FillDesc ds, SEG_STACK_R0,  DESC_STACK_R0
    FillDesc ds, SEG_STACK_R3,  DESC_STACK_R3

    ; prepare to load GDTR 
    xor	eax, eax
    mov	ax, ds
    SHL	eax, 4
    add	eax, GDT		; eax <- gdt base 
    mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt base 

    lgdt	[GdtPtr]
    cli

    in	al, 0x92    ; turn on A20 line
    or	al, 00000010b
    out	0x92, al

    ; shift to protectmode  
    mov	eax, cr0
    or	eax, 1
    mov	cr0, eax

    jmp	dword Selec_Code32_R0:0	; special, clear pipe-line and jump 
; END of [SECTION .s16]
 
;[SECTION .s32]; 32bit code seg, jmp form real mode.
[BITS 32]

SEG_CODE32_R0:
    call SetupPaging 
    mov ax, Selec_Data_R0
    mov ds, ax
    mov ax, Selec_Stack_R0
    mov	ss, ax
    mov esp, TopOfStack_R0

    mov	ax, Selec_Video_R0 
    mov	gs, ax	 
    mov	edi,(160 * 2) + 0  	; 160 * 50  line 3 column 1 
    mov	ah, 00001100b	    ; red  
    ; mov	ah, 00001011b	; 0000: black 1011: cyan , i love this beautiful word

    xor esi,esi         ; be careful !
    mov si, msg_GDT
    cld
    call temp_print32 

	;call SetupPaging ; error why?

    xor esi,esi         
    mov si, msg_Paged
    cld
    call temp_print32 
    extern main
    call main
    jmp $

SetupPaging: 
	; to simplification, linear addr = physical addr

	; inti page dircectory entry
	mov	ax, Selec_PageDir   ; base
	mov	es, ax
	mov	ecx, 1024		    ; 1K entry 
	xor	edi, edi
	xor	eax, eax
	mov	eax, PageTblBase|PG_P|PG_USU|PG_RWW

_fillPDE:
	stosd           ; eax -> es:edi dword
	add	eax, 4096	; page table is continual 
	loop _fillPDE	

	; init all page table
	mov	ax, Selec_PageTbl   ; base
	mov	es, ax
	mov	ecx, 1024 * 1024	; 1M entry
	xor	edi, edi
	xor	eax, eax
	mov	eax, PG_P|PG_USU|PG_RWW

_fillPTE:
	stosd
	add	eax, 4096		; each page has 4K space 
	loop _fillPTE	

	mov	eax, PageDirBase
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 0x80000000
	mov	cr0, eax
	jmp	short _filled 

_filled:
	nop ; why nop

	ret

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

[SECTION .data]
SEG_DATA_R0:

_msg_GDT:
    db "GDT loaded.", 0
msg_GDT equ _msg_GDT - $$

_msg_Paged:
    db "Paged.", 0
msg_Paged equ _msg_Paged - $$
   
SegData0Len  equ $ - SEG_DATA_R0

[SECTION .bss]
SEG_STACK_R0:   ; ring 0 stack segment 
    resb 512
TopOfStack_R0 equ $ - SEG_STACK_R0 - 1

SEG_STACK_R3:   ; ring 3 stack segment
    resb 512	
TopOfStack_R3 equ $ - SEG_STACK_R3 - 1

