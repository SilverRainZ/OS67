; a loader to load kernel & others
ORG 0x10000
JMP loader
msg:
    DB "loader loaded."

len EQU ($ - msg) 

loader:
    MOV	AX,0
    MOV	ES,AX
    MOV	AX,msg
    MOV	BP,AX	    ; ES:BP addr
    MOV	CX,len 
    MOV	AX,0x1301	;AH = 0x13，AL = 0x01
    MOV	BX,0x000c	;Page Number BH = 0 黑底红字 BL = 0x0c
    MOV	DL,0
    INT	0x10

