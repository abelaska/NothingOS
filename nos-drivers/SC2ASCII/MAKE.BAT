@echo off
if exist sc2ascii.com del sc2ascii.com
tasm  sc2ascii.asm /l > errors.txt
if not exist sc2ascii.obj goto error
tlink sc2ascii.obj /3
e2c sc2ascii.exe sc2ascii.com
if exist sc2ascii.drv del sc2ascii.drv
if exist *.obj del *.obj
if exist *.map del *.map
if exist *.bak del *.bak
if exist sc2ascii.exe del sc2ascii.exe
if exist sc2ascii.lst del sc2ascii.lst
if exist errors.txt del errors.txt
ren sc2ascii.com sc2ascii.drv
call ldel -q -s/dev/hda4 /drv/sc2ascii.drv
call lwrite -q -s/dev/hda4 ./sc2ascii.drv /drv/sc2ascii.drv
goto ende
:error
type errors.txt
:ende