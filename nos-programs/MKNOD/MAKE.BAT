@echo off
if exist mknod.com del mknod.com
if exist mknod del mknod
tasm  mknod.asm /l > errors.txt
if not exist mknod.obj goto error
tlink mknod.obj /3
e2c mknod.exe mknod.com
crc mknod.com
if exist *.obj del *.obj
if exist *.bak del *.bak
if exist mknod.exe del mknod.exe
if exist errors.txt del errors.txt
ren mknod.com mknod
call ldel -q -s/dev/hda4 /bin/mknod
call lwrite -q -s/dev/hda4 ./mknod /bin/mknod
if exist mknod reboot
goto ende
:error
type errors.txt
:ende