;########################################################################
;# Include soubor pro obsluzne rutiny
;#################################################################################

Include .\..\inc\code\code32\INT\Prot\Int00.inc
Include .\..\inc\code\code32\INT\Prot\Int06.inc
Include .\..\inc\code\code32\INT\Prot\Int07.inc
Include .\..\inc\code\code32\INT\Prot\Int08.inc
Include .\..\inc\code\code32\INT\Prot\Int09.inc
Include .\..\inc\code\code32\INT\Prot\Int0A.inc
Include .\..\inc\code\code32\INT\Prot\Int0B.inc
Include .\..\inc\code\code32\INT\Prot\Int0C.inc
Include .\..\inc\code\code32\INT\Prot\Int0d.inc

Include .\..\inc\code\code32\INT\B1\IntB1.inc   ; Main kernel interrupt
Include .\..\inc\code\code32\TSS\int-tsk.inc    ; Procedura, ktera nastavi IDT na spravne hodnoty :)
Include .\..\inc\code\code32\INT\intinit.inc    ; Procedura, ktera nastavi IDT na spravne hodnoty :)

;# prozatimni procedura pro navrat IRQcek
IntReturn :
        pushad
        pushfd
        mov  al,20h
        out  0a0h,al
        out  20h,al
        popfd
        popad
        Iret


