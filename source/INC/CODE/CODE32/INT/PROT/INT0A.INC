;###########################################################################
;# Obsluzna rutina pro #TS - General Protection Fault
;###########################################################################
TS_Fault_Routine :

        Mov     Ax,Data_Descriptor
        Mov     Ds,Ax
        Mov     Al,Byte ptr DS:[console_initialized]
        Or      Al,Al
        Jz      @TS_NoCons

        Pop     EBX
        Pop     EAX
        Mov     dword ptr ds:[Error_n2],EAX
        Pop     EBX
        mov     dword ptr ds:[Error_n1],EBX
        print   TS_cons
        Call    WaitKey
        Call    Restart



@TS_NoCons :
        Mov     AX,B8000_Descriptor
        Mov     ES,AX

        Xor     EDI,EDI
        Mov     ECX,80
        xor     eax,eax
        Rep     Stosw

        Xor     EDI,EDI
        Mov     ESI,Offset TS
        Mov     Ah,15
        Call    Write_pchar_Col

        Pop     EBX
        Pop     EBX
        Pop     EAX
        Call    Hex2Str_DD
        Mov     Ah,15
        Call    Write_text_Col
        Mov     Al,':'
        Stosw
        Mov     EAX,EBX
        Call    Hex2Str_dd
        Mov     Ah,15
        Call    Write_text_Col

        Call    WaitKey
        Call    Restart

