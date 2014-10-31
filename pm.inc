; vim: set ft=asm:

; 描述符类型
DA_32		EQU	0x4000	; 32 位段

DA_DPL0		EQU	  0x00	; DPL = 0
DA_DPL1		EQU	  0x20	; DPL = 1
DA_DPL2		EQU	  0x40	; DPL = 2
DA_DPL3		EQU	  0x60	; DPL = 3

; 存储段描述符类型
DA_DR		EQU	0x90	; 存在的只读数据段类型值
DA_DRW		EQU	0x92	; 存在的可读写数据段属性值
DA_DRWA		EQU	0x93	; 存在的已访问可读写数据段类型值
DA_C		equ	0x98	; 存在的只执行代码段属性值
DA_CR		equ	0x9a	; 存在的可执行可读代码段属性值
DA_CCO		equ	0x9c	; 存在的只执行一致代码段属性值
DA_CCOR		equ	0x9e	; 存在的可执行可读一致代码段属性值

; 系统段描述符类型
DA_LDT		EQU	  0x82	; 局部描述符表段类型值
DA_TaskGate	EQU	  0x85	; 任务门类型值
DA_386TSS	EQU	  0x89	; 可用 386 任务状态段类型值
DA_386CGATE	EQU	  0x8c	; 386 调用门类型值
DA_386IGATE	EQU	  0x8e	; 386 中断门类型值
DA_386TGATE	EQU	  0x8f	; 386 陷阱门类型值


; Selector Attribute
SA_RPL0		EQU	0	; ┓
SA_RPL1		EQU	1	; ┣ RPL
SA_RPL2		EQU	2	; ┃
SA_RPL3		EQU	3	; ┛

SA_TIG		EQU	0	; ┓TI
SA_TIL		EQU	4	; ┛


; usage: Descriptor Base, Limit, Attr
;        Base:  dd
;        Limit: dd (low 20 bits available)
;        Attr:  dw (lower 4 bits of higher byte are always 0)
%macro Descriptor 3
	DW	%2 & 0xffff			    ; 段界限1
	DW	%1 & 0xffff				; 段基址1
	DB	(%1 >> 16) & 0xff		; 段基址2
	DW	((%2 >> 8) & 0xf00) | (%3 & 0xf0ff)	; 属性1 + 段界限2 + 属性2
	DB	(%1 >> 24) & 0xff			; 段基址3
%endmacro ; 共 8 字节


; usage: Gate Selector, Offset, DCount, Attr
;        Selector:  dw
;        Offset:    dd
;        DCount:    db
;        Attr:      db
%macro Gate 4
	DW	(%2 & 0xffff)				; 偏移1
	DW	%1					; 选择子
	DW	(%3 & 0x1f) | ((%4 << 8) & 0xff00)	; 属性
	DW	((%2 >> 16) & 0xffff)			; 偏移2
%endmacro ; 共 8 字节

; usage segment reg, selector, gdt entry
%macro FillDesc 3
    xor	eax, eax
	mov	ax, %1 
	shl	eax, 4
	add	eax, %2 
	mov	word [%3 + 2], ax
	shr	eax, 16
	mov	byte [%3 + 4], al
	mov	byte [%3 + 7], ah
%endmacro
