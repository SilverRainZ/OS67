; os67's first process
align 4

[bits 32]
[section .text]

[global __init_start]
__init_start:
    nop     ; hava a rest~
    push 233
    push (msg_arg1 - $$) + 0xc0000000
    mov eax, 1
    int 0x80
    cmp eax, 0  ; fork test
    jz child
    jmp $
child:
    mov eax, 1  ; fork again
    int 0x80
    cmp eax, 0
    jz child2
    jmp $
child2:
    push 1000
    mov eax, 13
    int 0x80
    jmp $

msg_arg1:
    db "/init", 0

[global __init_end]
__init_end:
