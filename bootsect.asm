;boot sector
[BITS 16]
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

temp_print16:
loop:
    LODSB   ; DS:SI -> AL
    OR AL,AL
	JZ done 
	MOV	AH,0x0e		
	MOV	BX,15		
	INT	0x10		
	JMP	loop
done:
    RET

; executable code 
entry:
	MOV	AX,0		
	MOV	SS,AX
	MOV	SP,0x7c00
	MOV	DS,AX
	MOV	ES,AX   ; BIOS interrupt expects DS

	MOV	SI,msg
    CALL temp_print16

loadloader: ;  read 4 sector to load loader.bin
    MOV BX,0  ; loaded.bin 's addr in mem
    MOV AX,0x0800
    MOV ES,AX
    MOV CH,0
    MOV DH,1
    MOV CL,15

readloop:
	MOV	SI,0	; err counter 
retry:
	MOV	AH,0x02 ; read 
	MOV	AL,4	; sector number  
	MOV	DL,0x00 ; Driver A:
	INT	0x13
    JNC succ 
	ADD	SI,1
	CMP	SI,5	
	JAE	error
	MOV	AH,0x00
	MOV	DL,0x00	; Driver A
	INT	0x13	; reset
	JMP	retry	


error:		
	MOV	SI,msg_err
    CALL temp_print16
    JMP $
succ:	
	MOV	SI,msg_succ
    CALL temp_print16
    JMP 0x8000 
msg:
    DB	"bootsect loaded."
    DB  13,10
    DB	0
msg_err:
    DB	"loader load error."
    DB  13,10
    DB	0
msg_succ:
    DB	"loader load success."
    DB  13,10
    DB	0

TIMES 510 - ($-$$) DB 0
DB	0x55, 0xaa

