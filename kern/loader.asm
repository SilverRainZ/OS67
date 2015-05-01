; loader.asm
; jmp to C kernel, achieve some function in asm 
; 

align 4

[bits 32]
[section .text]

[extern osmain]
[global start]
start:
    xor eax,eax
    mov ax,0x10
    mov ds,ax
    mov ax,0x10
    mov es,ax
    mov ax,0x18
    mov gs,ax
    mov ax,0x10
    mov ss,ax
    mov esp,0x7c00

; mov the kernel to 0x100000
[extern kernstart]
[extern kernend]
    mov eax, kernend
    mov ecx, kernstart
    sub eax, ecx
    mov ecx, eax
    mov esi, 0x8000
    mov edi, 0x100000
    cld
    rep movsb
    jmp dword 0x08:go
go:

    mov	edi,(160*3)+0   ; 160*50 line 3 column 1 
    mov	ah,00001100b    ; red  

    
    mov esi,msg_pm  
    call temp_print32 

    jmp osmain          ; never return? 

    mov esi,msg_return  
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

msg_get_mem_map_err:
    db "Get memory message error, system halt.", 0

; function used in kernel 

; *******************gdt.c
[global gdt_flush]
[extern gp]

gdt_flush:      
    lgdt [gp]
    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    jmp 0x08:flush2
flush2:
    ret

; *******************idt.c
[global idt_load]
[extern idtp]

idt_load:
    lidt [idtp]
    ret

; *******************isrs.c
%macro m_isr 1
[global isr%1]
isr%1:
    cli
    ; int 8,10-14,17,30 have error code, 
    ; int fact int 9 exception happen 
    ; on outdataed techhnology, don't care it
    %if %1 != 17 && %1 != 30 && (%1 < 8 || %1 > 14)
        push byte 0 ; fake error code
    %endif
    push byte %1
    jmp isr_common_stub 

%endmacro 

%assign i 0
%rep 32
    m_isr i
    %assign i i + 1
%endrep

; 255 is a flag of unknown int
; so please note that we can't use it 
global isr_unknown 
isr_unknown:
    cli
    push byte 0
    push byte 255
    jmp isr_common_stub

; exists in isrs.c 
[extern fault_handler]

; a common ISR func, sava the context of CPU 
; call C func to process fault
; at last restore stack frame
isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov eax,esp
    push eax
    mov eax,fault_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

; *******************irq.c
%macro m_irq 1
[global irq%1]
irq%1:
    cli
    push byte 0
    push byte %1+32
    jmp irq_common_stub
%endmacro

%assign i 0
%rep 16 
    m_irq i   
%assign i i+1
%endrep

[extern irq_handler]

irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov eax,esp

    push eax
    mov eax,irq_handler
    call eax
    pop eax

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp,8
    iret


; *******************sched.c
; switch_to current => next
[global switch_to]
switch_to:
    ; retrieve arg 1: current
    mov eax, [esp+4]

    mov [eax+0], esp
    mov [eax+4], ebp
    mov [eax+8], ebx
    mov [eax+12], esi
    mov [eax+16], edi
    pushf
    pop ecx
    mov [eax+20], ecx

    ; retrieve arg 2; next
    mov eax, [esp+8]

    mov esp, [eax+0] 
    mov ebp, [eax+4] 
    mov ebx, [eax+8] 
    mov esi, [eax+12]
    mov edi, [eax+16]
    mov eax, [eax+20]
    push eax
    popf

    ret



