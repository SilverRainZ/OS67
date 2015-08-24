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
    jmp $

msg_arg1:
    db "/init", 0

[global __init_end]
__init_end:
