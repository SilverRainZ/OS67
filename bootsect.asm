
	ORG	0x7c00		
	JMP	entry

	DB	0x90
	DB	"OS67CDDS"  ; OEM name / 8 B
	DW	512		    ; 一个扇区512字节 1 WORD = 2 B
	DB	1		    ; 每个簇一个扇区
	DW	1	        ; 保留扇区数, 必须为 1	
	DB	2		    ; FAT表份数
	DW	224		    ; 根目录项数
	DW	2880		; RolSec16, 总扇区数
	DB	0xf0		; 介质种类: 移动介质
	DW	9		    ; FATSz16 分区表占用扇区数
	DW	18	        ; SecPerTrk, 磁盘用	
	DW	2	        ; 磁头数	
	DD	0		    ; HiddSec
	DD	2880		; 卡容量
	DB	0,0,0x29	;  DvcNum,NT保留,BootSig扩展引导标记
	DD	0xffffffff	; VolID 
	DB	"FLOPPYCDDS "   ; 卷标
	DB	"FAT12   "	    ; FilesysType 
; 可执行代码
   
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
	JE  fin 
	MOV	AH,0x0e		
	MOV	BX,15		
	INT	0x10		
	JMP	putloop
fin:
	JMP	fin		

msg:
	DB	"bootsect loaded."
	DB	0x0a		
	DB	0

	TIMES 510-($-$$) DB 0

	DB	0x55, 0xaa

