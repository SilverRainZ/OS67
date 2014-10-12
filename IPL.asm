
	ORG	0x7c00		
	JMP	entry

	DB	0x90
	DB	"OS67"	
	DW	512		
	DB	1		
	DW	1		
	DB	2		
	DW	224		
	DW	2880		
	DB	0xf0		
	DW	9		
	DW	18		
	DW	2		
	DD	0		
	DD	2880		
	DB	0,0,0x29	
	DD	0xffffffff	
	DB	"HELLO-OS   "	
	DB	"FAT12   "	
	TIMES 18 DB 0		



entry:
	MOV	AX,0		
	MOV	SS,AX
	MOV	SP,0x7c00
	MOV	DS,AX
	MOV	ES,AX

	MOV	SI,msg
putloop:
	MOV	AL,[SI]
	ADD	SI,1		
	CMP	AL,0
	JE  add	
	MOV	AH,0x0e		
	MOV	BX,15		
	INT	0x10		
	JMP	putloop
add:
    INCBIN "add.bin"
fin:
	HLT			
	JMP	fin		

msg:
	DB	"OS64"
	DB	0x0a		
	DB	0

	TIMES 510-($-$$) DB 0

	DB	0x55, 0xaa

