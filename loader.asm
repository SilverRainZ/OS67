ORG 0x8000
JMP loader
msg:
ABC    DB 33 
    DB 13,10,10
loader:
    MOV AX,0
    MOV DS,AX
    MOV SS,AX
    MOV SP,0x9c00
    MOV AX,0xb800
    MOV GS,AX
    MOV	EDI, (80 * 1 + 0) * 2	; 屏幕第 2 行, 第 1 列。
    MOV	AH, 0x0c			; 0000: 黑底    1100: 红字
    MOV AL,[msg]
    MOV [GS:DI],AX
    
    MOV SI,msg
    MOV AX,[ES:SI]
    MOV AH,0x0e
    MOV BX,0
    INT 0x10
    JMP $
