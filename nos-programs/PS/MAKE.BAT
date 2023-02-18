@echo off
if exist ps.com del ps.com
if exist ps del ps
tasm  ps.asm /l > errors.txt
if not exist ps.obj goto error
tlink ps.obj /3
e2c ps.exe ps.com
crc ps.com
if exist *.obj del *.obj
if exist *.bak del *.bak
if exist ps.exe del ps.exe
if exist errors.txt del errors.txt
ren ps.com ps
call ldel -q -s/dev/hda4 /bin/ps
call lwrite -q -s/dev/hda4 ./ps /bin/ps
goto ende
:error
type errors.txt
:ende