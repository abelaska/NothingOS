@echo off
if exist nosh.com del nosh.com
if exist nosh del nosh
tasm  nosh.asm /l > errors.txt
if not exist nosh.obj goto error
tlink nosh.obj /3
e2c nosh.exe nosh.com
crc nosh.com
if exist nosh del nosh
if exist *.obj del *.obj
if exist *.bak del *.bak
if exist nosh.exe del nosh.exe
if exist errors.txt del errors.txt
ren nosh.com nosh
call ldel -q -s/dev/hda4 /bin/nosh
call lwrite -q -s/dev/hda4 ./nosh /bin/nosh
rem Bootup
goto ende
:error
type errors.txt
:ende