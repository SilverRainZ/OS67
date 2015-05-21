; represent a floppy
; ###boot sector
INCBIN "bootsect.bin"
times 80*18*2*512 - ($-$$) db 0
; 80 sector * 18 cylinder * 2 head * 512 byte
