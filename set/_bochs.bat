@echo off
if "%1"=="dbg" goto dbg
    bochs -q -f set/bochsrc.bxrc 
exit
:dbg
    bochsdbg -q -f set/DBG_bochsrc.bxrc
exit

