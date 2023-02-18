;###########################################################################
;# Obsluzna rutina pro IRQ1 = obsluha klavesnice
;###########################################################################
;# Version : 1.2-2-1
;#################################################################################
;# Poznamka : jeste nejni implementovane vypisovani na obrazovku, pri stisku
;# klavesy... pro budoucnost, vypisuje se jen tehdy, je-li bit ToScr v
;# Flags nastaven na 1.
;# Zatim se o vypisovani na obrazovku musi starat uzivatelsky program
;#################################################################################

Keyboard_IRQ_Routine :
        PushfD
        PUSHAD
        PUSH   ES DS
        CLI

        Xor    EAX,EAX
        In     Al,60h        ; do Al nacteme znak
        Cmp    Al,0EEh
        Je     @Keyboard_End ; klavesnice vratila ECHO
        Cmp    Al,0FAh
        Je     @Keyboard_End ; klavesnice vratila ACK
        Cmp    Al,0FEh
        Je     @Keyboard_End ; klavesnice vratila NAK

        Mov    DX, Data_Descriptor
        Mov    DS, DX
        Mov    ES, Word ptr DS:[Keyb_Buffers_Selector]
        Mov    EDX, Dword ptr DS:[KEYB_Buffer_Size]
        PUSH   EAX
         Movzx    EAX,Word ptr DS:[Active_Console]
         Mul      edx
         Mov      EDI,EAX
        POP    EAX
        Mov    DL,AL
        Mov    EBX,10h
        Mov    EAX,Char_Encode_driver
        Mov    CX,Word ptr ES:[EDI.Keyb_Buffer_Flags]
        ;Vstup  : DL - Scan Code
        ;         CX - Flagy
        ;Vystup : DL - ASCII code
        ;         DH - Extended ASCII code
        ;         CX - Flagy
        ;         EBX - 000000 - klavesa sla dolu
        ;               FFFFFF - klavesa sla nahoru

@Keyboard_Write :
        PUSH   ECX
         Call   Drv_Gate
        POP    EAX
        Jc     @Keyboard_End
        Mov    Word ptr ES:[EDI.Keyb_Buffer_Flags],CX
        Test   Dh,1
        Jz     @@Keyboard_IRQ_Routine_Not_Console_Switch
        Test   Dh,24h
        Jz     @@Keyboard_IRQ_Routine_Not_Console_Switch
        Cmp    Dl,11h
        Jb     @@Keyboard_IRQ_Routine_Not_Console_Switch
        Cmp    Dl,1Ch
        Ja     @@Keyboard_IRQ_Routine_Not_Console_Switch
        PUSH   ECX
         Movzx  EAX,Dl
         Sub    EAX,11h
         Shl    EAX,16
         Mov    AX,Consoles_Driver
         Mov    EBX,18h
         Call   Drv_Gate
        POP    ECX
;        Mov    EDX,Dword ptr DS:[KEYB_Buffer_Size]
        Movzx  EAX,Word ptr DS:[Active_Console]
        Mul    Dword ptr DS:[KEYB_Buffer_Size]
        Mov    EDI,EAX
        Mov    AX,Word ptr ES:[EDI.Keyb_Buffer_Flags]
        And    CX,0000000111111000b
        And    AX,1111111000000111b
        Or     AX,CX
        Mov    Word ptr ES:[EDI.Keyb_Buffer_Flags],AX
        Mov    Cl,Al
        Call   Keyb_SetFlags
        Jmp    @Keyboard_End
   @@Keyboard_IRQ_Routine_Not_Console_Switch:
        Or     EBX,EBX
        Jnz    @@Keyboard_IRQ_Routine_Up
        Test   CH,2
        Jz     @Keyboard_IRQ_Routine_Skip_Writing2Screen
        Jmp    Short @@Keyboard_IRQ_Routine_Write_Cont
   @@Keyboard_IRQ_Routine_Up:
        Test   CH,4
        Jz     @Keyboard_IRQ_Routine_Skip_Writing2Screen
   @@Keyboard_IRQ_Routine_Write_Cont :
       PUSHAD
        Test   DH,1
        Jz     @@Keyboard_IRQ_Routine_Write_Normal
        Mov    EBP,EDX
        Cmp    Dl,40h
        Je     @Keyboard_Write_OK
        Cmp    Dl,41h
        Je     @Keyboard_Write_OK
        Mov    EBP,EDX
        Xor    EDX,EDX
   @@Keyboard_IRQ_Routine_Write_Normal :
        Mov    ECX,Dword ptr Es:[EDI.Keyb_Buffer_End]        ; ECX = kde buffer konci
        Mov    ESI,Dword ptr Es:[EDI.Keyb_Buffer_Write_Pos]  ; ESI = kam se ma zapisovat
        Mov    EAX,Dword ptr Es:[EDI.Keyb_Buffer_Read_Pos]   ; EAX = odkud se cte ?
        Mov    EBX,Dword ptr ES:[EDI.Keyb_Buffer_Start]      ; EDI = kde zacina buffer
        Inc    ESI
        Cmp    EAX,ESI
        JE     @Keyboard_Write_Error
        Cmp    ESI,ECX
        Jna    @Keyboard_Write_Write
        Cmp    EBX,EAX
        Je     @Keyboard_Write_Error
        Mov    ESI,EBX
        Inc    ESI
    @Keyboard_Write_Write :
        Dec    ESI
        Mov    Byte ptr ES:[ESI+EDI],Dl
        Inc    ESI
        Mov    Dword ptr Es:[EDI.Keyb_Buffer_Write_Pos],ESI
        Or     Dl,Dl
        Jnz    Short @Keyboard_Write_OK
        Mov    EDX, EBP
        Jmp    @@Keyboard_IRQ_Routine_Write_Normal
    @Keyboard_Write_Error :
       POPAD
        Jmp    @Keyboard_End
    @Keyboard_Write_OK :
       POPAD
        Test   Ch,08h
        Jnz    @Keyboard_End
        And    Al,7
        And    Cl,7
        Or     Al,Cl
        Jz     @Keyboard_IRQ_Routine_Skip_Flags
        Call   Keyb_SetFlags
   @Keyboard_IRQ_Routine_Skip_Flags:
        Movzx  ECX,Word ptr ES:[EDI.Keyb_Buffer_Flags]
        Test   CH,10h
        Jz     @Keyboard_IRQ_Routine_Skip_Writing2Screen
        Test   DH,1
        Jnz    @Keyboard_IRQ_Routine_Skip_Writing2Screen
        Movzx  EAX,Word ptr DS:[Active_Console]
        Shl    EAX,16
        Mov    AX, Consoles_driver
        Mov    EBX, Drv_Std_WRITE
        Xor    DH,DH
        Test   CX,90h
        Jnz    @Keyboard_IRQ_Routine_Writing2Screen_N
        Cmp    Dl,'\'
        Je     @Keyboard_IRQ_Routine_Writing2Screen_Rave
        Cmp    Dl,27
        Jne    @Keyboard_IRQ_Routine_Writing2Screen_N
   @Keyboard_IRQ_Routine_Writing2Screen_Rave :
        Inc    DH
   @Keyboard_IRQ_Routine_Writing2Screen_N :
        Call   DRV_Gate
   @Keyboard_IRQ_Routine_Skip_Writing2Screen:
        Movzx    EDX,Word ptr DS:[Active_Console]
        Shl      EDX,2
        Mov      EBX,Dword ptr DS:[KEYB_Tasks_Sleeping+EDX]
        Or       EBX,EBX
        Jz       Short @Keyboard_End
        Call     Task_Wakeup_Task
@Keyboard_End :              ; Standardni konec rutiny preruseni
        In    Al,61H
        Mov   Ah,Al
        Or    Al,80h
        Out   61h,Al
        Xchg  Al,Ah
        Out   61h,Al
        Mov   Al,20h
        Out   20h,Al
        POP    DS ES
        POPAD
        PopfD
        Iret
