; represent a floppy
INCBIN "bootsect.bin"
; include a file loader
; resb 0x200  ; fat1 0x200 here
db 0xf0,0xff,0xff   ; reserve fat entry 0 1
db 0x03,0x40,0x00,0x05,0x60,0x00,0x07,0x80,0x00,0x09,0xa0,0x00,0xff,0x0f        

times 0x1400 - ($-$$) db 0 ; fat2 same to fat1
db 0xf0,0xff,0xff  
db 0x03,0x40,0x00,0x05,0x60,0x00,0x07,0x80,0x00,0x09,0xa0,0x00,0xff,0x0f        
                        ; name      offset  len     describe
times 0x2600 - ($-$$) db 0  ; root dir
db "loader  "           ; dir_name  0       0x0b    name&extname 
db "bin"                
db 0x20                 ; dir_attr  0xb     1       attribute
times 10 db 0           ; resrrve   0xc`    10
dw 0x0000               ; dir_wrttime 0x16  2       last time to write
dw 0x0000               ; dir_wrtdate 0x18  2   
dw 0x0002               ; dir_fstclus 0x1a  2       cluster number of the file
dd 0x00001000           ; dir_fileszie 0x1c 4       4kib

times (((224 * 32) + (512 - 1))/512 + 19) * 512 - ($-$$) \
db 0 ; 0x4200 - ($ - $$)
INCBIN "kernel"

times 80*18*2*512 - ($-$$) db 0
; current section - current line
; 80 sector * 18 cylinder * 2 head * 512 byte
