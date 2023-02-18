;####################################################################################
;# Hlavni Include soubor pro dataseg kernelu
;####################################################################################
             Include .\..\inc\data\error.inc
             Include .\..\inc\data\const.inc
             Include .\..\inc\data\struc.inc
             Include .\..\inc\data\var.inc
             Include .\..\inc\data\pages.inc
             Include .\..\inc\data\waitdata.inc
             Include .\..\inc\data\libdata.inc
             Include .\..\inc\data\partdata.inc
             Include .\..\inc\data\drvdata.inc
             Include .\..\include\ext2.inc
             Include .\..\inc\data\memdata.inc
             Include .\..\inc\data\exec.inc
             Include .\..\inc\data\b1_int.inc
             Include .\..\inc\data\vfsdata.inc
             Include .\..\inc\data\fsdata.inc
             Include .\..\inc\data\tskdata.inc
             Include .\..\inc\data\swap.inc
             Include .\..\inc\data\cmosdata.inc
             Include .\..\inc\data\inittxt.inc
             Include .\..\inc\data\paldata.inc
             Include .\..\inc\data\consdata.inc
             Include .\..\inc\data\drivers\execdrv.inc
             Include .\..\inc\data\drivers\idedrv.inc
             Include .\..\inc\data\drivers\consdrv.inc
             Include .\..\inc\data\drivers\Keybdrv.inc
             Include .\..\inc\data\drivers\ext2drv.inc
             Include .\..\inc\data\init.inc             
             Include .\..\inc\data\vlibs.inc

JMP2Protect  dd  offset Start_Code32
             dw  Code_Descriptor

Temp_Pointer  dw 65535
              dd GDT_Base
;####################################################################################
;# Adds
;####################################################################################
;# T2 = waitdata.inc
;# T3 = libdata.inc