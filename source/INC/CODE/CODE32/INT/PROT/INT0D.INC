;###########################################################################
;# Obsluzna rutina pro #GP - General Protection Fault
;###########################################################################
GP_Fault_Routine :

        Mov     Ax,Data_Descriptor
        Mov     Ds,Ax
        Mov     Al,Byte ptr DS:[console_initialized]
        Or      Al,Al
        Jz      @GP_NoCons
        Call    TASK_Get_Current_Task_Num
        CMP     EBX, 1
        Je      GP_Fault_Routine_kernel
        Call    TASK_Get_VFS_STDError_Inode
        Mov     EDI,Offset GP_Task
        Mov     ECX,GP_Task_size
        Push    DS
        Pop     ES
        Xor     EBX,EBX
       PUSHAD
        Call    VFS_Write
       POPAD
        POP     EBX
        Pop     EBX
        Pop     ECX
       PUSH     EAX EBX
       PUSH     EAX
        Mov     EAX,ECX
        Call    Hex2str_DD
        Push    DS
        Pop     ES
        Xor     EBX,EBX
        Mov     EDI,ESI
       POP     EAX
        Call    VFS_Write
        Mov     EDI,Offset Double_Trouble
        Mov     ECX,1
        Xor     EBX,EBX
        Call    VFS_Write
       POP      EAX
        Call    Hex2str_DD
        Push    DS
        Pop     ES
        Xor     EBX,EBX
        Mov     EDI,ESI
       POP      EAX
        Call    VFS_Write
        Call    Task_Kill_Me
        Jmp     $
GP_Fault_Routine_kernel:
        Pop     EBX
        Pop     EAX
        Mov     dword ptr ds:[Error_n2],EAX
        Pop     EBX
        mov     dword ptr ds:[Error_n1],EBX
        print   GP_cons
        Call    WaitKey
        Call    Restart

@GP_NoCons :
        Mov     AX,B8000_Descriptor
        Mov     ES,AX

        Xor     EDI,EDI
        Mov     ECX,80
        xor     eax,eax
        Rep     Stosw

        Xor     EDI,EDI
        Mov     ESI,Offset GP
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

