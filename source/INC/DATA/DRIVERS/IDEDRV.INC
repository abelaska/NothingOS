;###########################################################################
;# IDE driver for NothingOS
;###########################################################################
IDE_DATA:
  Include         .\..\inc\code\code32\IDE\INC\DATA\IDEstruc.inc
  Include         .\..\inc\code\code32\IDE\INC\DATA\Data.inc
IDE_DATA_END:

IDE_DATA_SIZE equ (((IDE_DATA_END - IDE_DATA - 1) shr 12) +1)