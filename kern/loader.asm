; name: loader.asm
; func: loader kenel; enter pm 32; set temp GDT
; 
;
%INCLUDE "pm.inc"
;org 0x8000
[BITS 32]
ALIGN 4
[extern osmain]
[SECTION .text]

[global start]
start:
    xor eax, eax
    mov ax, 0x10
    mov ds, ax
    mov ax, 0x18
    mov gs, ax
    mov ax, 0x20
    mov ss, ax
    mov esp, 0xfff

    mov	edi,(160 * 3) + 0  	; 160 * 50  line 3 column 1 
    mov	ah, 00001100b	    ; red  
    ; mov	ah, 00001011b	; 0000: black 1011: cyan , i love this beautiful word

    xor esi,esi         ; be careful !
    mov si, msg_pm  
    call temp_print32 
    
    jmp osmain ; never return, 对吧, 亲爱的10567?

    xor esi,esi         
    mov si, msg_return  
    call temp_print32 
    jmp $

temp_print32:
    add edi, 160
    push edi
    cld

loop:
    lodsb
    cmp al, 0
    je outloop
    mov	[gs:edi], ax
    add edi,2
    jmp loop

outloop:
    pop edi
    ret

msg_pm:
    db "In protect mode now.", 0
msg_return:
    db "I loved you, No.67.", 0

[SECTION .data]

[SECTION .rdata]
