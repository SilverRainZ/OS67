; os67's first process
align 4

[bits 32]
[section .text]
[global __init_start]
[global __init_end]

__init_start:
    nop     ; hava a rest~
    mov eax, 1
    int 0x80
    cmp eax, 0
    jz child
    jmp $
child:
    push (msg_arg0 - $$) + 0xc0000000
    mov eax, 7  ; exec
    int 0x80
    jmp $

msg_arg0:
    db "/cinit", 0

__init_end:
