; a loader to load kernel & others
ORG 0x7c71
JMP loader
msg:
    DB 0x0a
    DB "loader loaded."

len EQU ($ - msg) 

loader:
    MOV	AX,0
    MOV	ES,AX
    MOV	AX,msg
    MOV	BP,AX	    ; ES:BP addr
    MOV	CX,0x000F
    MOV	AX,0x1301	;AH = 0x13，AL = 0x01
    MOV	BX,0x000c	;Page Number BH = 0 黑底红字 BL = 0x0c
    MOV	DL,0
    INT	0x10
    JMP $

