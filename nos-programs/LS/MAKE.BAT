@echo off
if exist ls.com del ls.com
if exist ls del ls
tasm  ls.asm /l > errors.txt
if not exist ls.obj goto error
tlink ls.obj /3
e2c ls.exe ls.com
crc ls.com
if exist *.obj del *.obj
if exist *.bak del *.bak
if exist ls.exe del ls.exe
if exist errors.txt del errors.txt
ren ls.com ls
call ldel -q -s/dev/hda4 /bin/ls
call lwrite -q -s/dev/hda4 ./ls /bin/ls
rem reboot
goto ende
:error
type errors.txt
:ende