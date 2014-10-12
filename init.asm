    org 0x7c6d
    jmp disstr
msg:
    db "run to add."
    db 0x0a
    db 0
disstr:
    mov si,msg
putloop:
    mov al,[si]
    add si,1
    cmp al,0
    je back 
    mov ah,0x0e
    mov bx,15
    int 0x10
    jmp putloop
back:   
    
    

  
