;boot sector FAT12
%INCLUDE "pm.inc"
[BITS 16]
org    0x7c00        

BS_jmpBoot      jmp    entry
                db    0x90
BS_OEMName      db    "OS67CDDS"        ; OEM name / 8 B
BPB_BytsPerSec  dw    512               ; 一个扇区512字节 
BPB_SecPerClus  db    1                 ; 每个簇一个扇区
BPB_RsvdSecCnt  dw    1                 ; 保留扇区数, 必须为1
BPB_NumFATs     db    2                 ; FAT表份数
BPB_RootEntCnt  dw    224               ; 根目录项数
BPB_TotSec16    dw    2880              ; RolSec16, 总扇区数
BPB_Media       db    0xf0              ; 介质种类: 移动介质
BPB_FATSz16     dw    9                 ; FATSz16 分区表占用扇区数
BPB_SecPerTrk   dw    18                ; SecPerTrk, 磁盘 
BPB_NumHeads    dw    2                 ; 磁头数    
BPB_HiddSec     dd    0                 ; HiddSec
BPB_TotSec32    dd    2880              ; 卡容量
BS_DrvNum       db    0                 ; DvcNum
BS_Reserved1    db    0                 ; NT保留    
BS_BootSig      db    0x29              ; BootSig扩展引导标记
BS_VolD         dd    0xffffffff        ; VolID 
BS_VolLab       db    "FLOPPYCDDS "     ; 卷标
BS_FileSysType  db    "FAT12   "        ; FilesysType

times 18 db 0

GDT:
DESC_NULL:        Descriptor 0,       0,                  0             ; null
DESC_CODE32_R0:   Descriptor 0x8000,  0x8000000-0x8000,   DA_C+DA_32    ; uncomfirm 
DESC_DATA_R0:     Descriptor 0,       0x7ffffff,          DA_DRW+DA_32  ; 4G seg 
DESC_VIDEO_R0:    Descriptor 0xb8000, 0xffff,             DA_DRW+DA_32  ; vram 
DESC_STACK_R0:    Descriptor 0,       0x7ffffff,          DA_DRWA+DA_32
; mem size is 128M

GdtLen    equ    $ - GDT    ; GDT len
GdtPtr    dw    GdtLen - 1  ; GDT limit
dd    0                     ; GDT Base

; GDT Selector 
Selec_Code32_R0 equ     DESC_CODE32_R0 - DESC_NULL
Selec_Data_R0   equ     DESC_DATA_R0   - DESC_NULL 
Selec_Video_R0  equ     DESC_VIDEO_R0  - DESC_NULL
Selec_Stack_R0  equ     DESC_STACK_R0  - DESC_NULL

temp_print16:
loop:
    lodsb   ; ds:si -> al
    or al,al
    jz done 
    mov    ah,0x0e        
    mov    bx,15        
    int    0x10        
    jmp    loop
done:
    ret

;============================================================
entry:
    mov    ax,0        
    mov    ss,ax
    mov    sp,0x7c00
    mov    ds,ax
    mov    es,ax   ; bios interrupt expects ds
   ; mov ah,0x01
   ; mov ch,00010000b    ; bit 5 = 1 bit 6 = 0
   ; mov cl,0x0
   ; int 0x10    ; hide cursor
    
   ; mov ah, 0x06
   ; mov al,0
   ; mov ch,0
   ; mov cl,0
   ; mov dh,0
   ; mov dl,0
   ; int 0x10    ; cls
   ; mov    si,msg
   ; call temp_print16
    mov ah, 0x0
    mov al, 0x03 ; 80*25 16 color
    int 0x10

    mov si, msg_boot
    call temp_print16

getmsg:
    mov ax,0x9000
    mov ds,ax
    mov ah,0x03
    xor bh,bh
    int 0x10
    mov [0],dx  ; cursor pos save in 0x9000 
 
    mov ah,0x88
    int 0x15
    mov [2],ax  ; get mem size(extrened mem kb)

    mov ah,0x0f
    int 0x10
    mov [4],bx  ; bh = display page
    mov [6],ax  ; al = video mode, ah = window width
       
    mov ah,0x12
    mov bl,0x10
    int 0x10
    mov [8],ax  ; 0x90008 do u know what linus's meaning?
    mov [10],bx ; 0x9000a  安装的显示内存 0x900b  显示状态
    mov [12],cx ; 0x9000c  显示卡的特性参数
    
    mov ax,0
    mov ds,ax   ; revert
; get message end

    
loadloader: ;  read 4 sector to load loader.bin
    mov bx,0  ; loader.bin 's addr in mem
    mov ax,0x0800 ; loader's addr
    mov es,ax
    mov ch,0
    mov dh,1
    mov cl,16
; loader.bin 在软gg盘的第34个扇区,0x4200,换算为c0-h1-s16

readloop:
    mov    si,0    ; err counter 
retry:
    mov  ah,0x02 ; read 
    mov  al,4    ; read 4 sector 
    mov  dl,0x00 ; driver a:
    int  0x13
    jnc  succ 
    add  si,1
    cmp  si,5    
    jae  error
    mov  ah,0x00
    mov  dl,0x00    ; driver a
    int  0x13    ; reset
    jmp  retry    

error:        
    mov  si,msg_err
    call temp_print16
    jmp $
succ:    
    mov  si,msg_succ
    call temp_print16
;=============================kernel loaded================================
   ; FillDesc cs, SEG_CODE32_R0, DESC_CODE32_R0
   ; FillDesc ds, SEG_DATA_R0,   DESC_DATA_R0
   ; FillDesc ds, SEG_STACK_R0,  DESC_STACK_R0
   ; FillDesc ds, SEG_STACK_R3,  DESC_STACK_R3

    xor  eax, eax
    mov  ax, ds
    shl  eax, 4
    add  eax, GDT        ; eax <- gdt base 
    mov  dword [GdtPtr + 2], eax    ; [GdtPtr + 2] <- gdt base 

    lgdt [GdtPtr]
    cli

    mov  si,msg_gdt
    call temp_print16

    ; turn on A20 line
    in  al, 0x92   
    or  al, 00000010b
    out  0x92, al

    mov  si,msg_a20
    call temp_print16


    ; shift to protectmode  
    mov    eax, cr0
    or eax, 1
    mov    cr0, eax

    jmp    dword Selec_Code32_R0:0    ; special, clear pipe-line and jump 

msg_boot:
    db  "Bootsector loaded...",13,10,0
msg_err:
    db  "Loader load error!",13,10,0
msg_succ:
    db  "Loader load success...",13,10,0
msg_gdt:
    db  "Temp GDT loaded...",13,10,0
msg_a20:
    db  "A20 line on...",13,10,0
msg_temp:
    db 0,0,0

times 510 - ($-$$) db 0
db  0x55, 0xaa

