
	ORG	0x7c00		

BS_jmpBoot      JMP	entry
                DB	0x90
BS_OEMName      DB	"OS67CDDS"  ; OEM name / 8 B
BPB_BytsPerSec  DW	512		    ; 一个扇区512字节 
BPB_SecPerClus  DB	1		    ; 每个簇一个扇区
BPB_RsvdSecCnt  DW	1	        ; 保留扇区数, 必须为1
BPB_NumFATs     DB	2		    ; FAT表份数
BPB_RootEntCnt  DW	224		    ; 根目录项数
BPB_TotSec16    DW	2880		; RolSec16, 总扇区数
BPB_Media       DB	0xf0		; 介质种类: 移动介质
BPB_FATSz16     DW	9		    ; FATSz16 分区表占用扇区数
BPB_SecPerTrk   DW	18	        ; SecPerTrk, 磁盘 
BPB_NumHeads    DW	2	        ; 磁头数	
BPB_HiddSec     DD	0		    ; HiddSec
BPB_TotSec32    DD	2880		; 卡容量
BS_DrvNum       DB	0           ; DvcNum
BS_Reserved1    DB  0           ; NT保留    
BS_BootSig      DB  0x29        ; BootSig扩展引导标记
BS_VolD         DD	0xffffffff	; VolID 
BS_VolLab       DB	"FLOPPYCDDS "   ; 卷标
BS_FileSysType  DB	"FAT12   "	    ; FilesysType
TIMES 18 DB 0

; executable code 
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

