INCBIN "bootsect.bin"
; include a file loader
; RESB 0x200  ; FAT1
DB 0xF0,0xFF,0xFF   ; reserve FAT Entry 0 1
DB 0xFF,0xF         ; file loader occupy a culster
RESB 0x1400 - ($-$$) ; FAT2 same to FAT1
DB 0xF0,0xFF,0xFF  
DB 0xFF,0xF       
                        ; name      offset  len     describe
RESB 0x2600 - ($-$$)    ; root dir
DB "loader  "           ; DIR_Name  0       0x0b    name&extname 
DB "bin"                
DB 0x20                 ; DIR_Attr  0xB     1       Attribute
RESB 10                 ; Resrrve   0xC`    10
DW 0x0000; DIR_WrtTime 0x16  2       last time to write
DW 0x0000               ; DIR_WrtDate 0x18  2   
DW 0x0002               ; DIR_FstClus 0x1A  2       Cluster number of the file
DD 0x00000014           ; DIR_FileSzie 0x1C 4   variable 

RESB (((224 * 32) + (512 - 1))/512 + 19) * 512 - ($-$$) ; 0x4200 - ($ - $$)
INCBIN "loader.bin"

RESB 80*18*2*512 - ($-$$)    

; current section - current line
; 80 sector * 18 cylinder * 2 head * 512 byte
