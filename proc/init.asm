; os67's first process
align 4

[bits 32]
[section .text]
[global __init_start]
[global __init_end]

__init_start:
    nop     ; hava a rest~
    mov eax, 1
    int 0x80    ; a free process is necessary
    cmp eax, 0
    jz child
    jmp $

child:
    push (msg_argv - $$) + 0xc0000000   ; arguments
    push (msg_init - $$) + 0xc0000000   ; path
    push 0  ; eip
    mov eax, 7  ; exec
    int 0x80
    jmp $

msg_init:
    db "/bin/cinit", 0
; char **argv = {"/bin/cinit", 0}
msg_argv:
    dd (msg_init - $$) + 0xc0000000
    dd 0

__init_end:
