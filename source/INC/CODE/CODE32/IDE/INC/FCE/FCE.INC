;#####################################################################################
;# Implementace funkci ide.drv
;#####################################################################################
;# vsechny fce maji v eax pri volani nasledujici :
;# DDppXXXX
;# kde : pp   - partition (0 = cely disk, 1 = prvni part.,...)
;#       DD   - disk  (01 = hda,02 = hdb,...)
;#       XXXX - ide_driver
;# priklad : EAX = 01020020h => pracujeme s hda2
;#####################################################################################
include .\..\inc\code\code32\ide\inc\fce\standard.inc
include .\..\inc\code\code32\ide\inc\fce\extended.inc
