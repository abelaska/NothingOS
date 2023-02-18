;###########################################################################
;# Code segment
;###########################################################################
IDE_Code:
Sel_Data  dw      0           ; selector na datasegment daneho driveru
include .\..\inc\code\code32\IDE\inc\fce\fce.inc
include .\..\inc\code\code32\IDE\inc\code\init\autodet.inc
include .\..\inc\code\code32\IDE\inc\code\init\part.inc
include .\..\inc\code\code32\IDE\inc\code\int\intff.inc
include .\..\inc\code\code32\IDE\inc\code\int\intfe.inc
include .\..\inc\code\code32\IDE\inc\code\ata\ata.inc
IDE_CodeEnd:

IDE_CODE_Size equ (((IDE_CodeEnd - IDE_Code-1) Shr 12)+1)

