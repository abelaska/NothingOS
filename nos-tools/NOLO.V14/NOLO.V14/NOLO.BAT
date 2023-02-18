@ECHO OFF
if exist *.com del *.com
tasm nolo /l
if not exist nolo.obj goto ende
tlink nolo /3
e2c nolo.exe nolo.com
if exist *.obj del *.obj
if exist *.map del *.map
if exist *.bak del *.bak
if exist nolo.inc del nolo.inc
if exist nolo.exe del nolo.exe
rem dbfile.exe nolo.com nolo.inc
rem del nolo.com
:ende