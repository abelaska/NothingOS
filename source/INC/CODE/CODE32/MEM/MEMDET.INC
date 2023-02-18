;##############################################################################
;# Memory Size Detection Code
;##############################################################################

;##############################################################################
;# Procedura zjisti velikost RAM pameti pomoci RAW pristupu :)
;##############################################################################
Memory_Size_Detection PROC
       PUSHAD
       PUSH    ES DS
        Mov     EAX,GDT_Descriptor
        Mov     ES,EAX
        Mov     EAX, (1024*1024) ;zacneme nad 1MB
        Create_Temp_Descriptor EAX
        Mov     Word ptr ES:[EDX],0FFFFh
        Mov     Al, Byte ptr ES:[EDX+6]
        Or      Al,0Fh
        Mov     Byte ptr ES:[EDX+6], Al
        Mov     ES,EDX
        Mov     EAX,Data_Descriptor
        Mov     DS,EAX
        Xor     EDI,EDI
        Inc     EDI
        Mov     EBX,((1024*1024) Shr 12)
        Mov     Al,0B1h
        Mov     ECX,ES
        LSL     ECX,ECX
        Shr     ECX,12
@Memory_Size_Detection_Loop:
        Xor     AH,AH
        Mov     Byte ptr ES:[EDI],AL
        Mov     AH,Byte ptr ES:[EDI]
        Cmp     Ah,AL
        Jne     @Memory_Size_Detection_Loop_END
        Add     EDI,4096
        Inc     EBX
        Loop    @Memory_Size_Detection_Loop
@Memory_Size_Detection_Loop_END :
        Mov     Dword ptr DS:[RAM_Size],EBX
        Shr     EBX,8
        Mov     Dword ptr DS:[RAM_Size_MB],EBX
        Dispose_Temp_Descriptor ES
       POP     DS ES
       POPAD
        RET
Memory_Size_Detection ENDP



