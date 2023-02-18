;############################################################################
;# FPU :)
;############################################################################

FPU_Initializing PROC
        Mov    EAX,CR0
        And    Al, 11110001b
        Or     Al, 00110000b
        Mov    CR0,EAX
        Mov    CX,CS
        Mov    EAX,Offset FPU_Device_Not_Avaible
        Mov    EDX,7
        Call   Set_Int_Routine
        Print  FPU_Init_TXT
        RET
FPU_Initializing ENDP

Include   .\..\inc\code\code32\FPU\Fpu-int.inc