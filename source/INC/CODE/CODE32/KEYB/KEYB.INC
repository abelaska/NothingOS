;###########################################################################
;# Keyboard virtual driver
;###########################################################################

;###########################################################################
;# Inicializacni procedura
;###########################################################################
Keyb_Init PROC
        PUSHFD
        PUSHAD
        PUSH   DS ES
         Mov   DX, Data_Descriptor
         Mov   DS, DX
         Mov   EAX, Dword ptr DS:[KEYB_Buffer_Size]
         Mov   EDX, EAX
         PUSH  EDX
          Mov   ECX, Max_Console_Count
          Mul   ECX
          Call  Kalloc
          Mov   Word ptr DS:[Keyb_Buffers_Selector],AX
          Mov   ES, AX
          Xor   EDI,EDI
          Mov   EAX,20
         POP    EDX
 @Keyb_Init_Initialize_Buffers :
         Mov   Dword ptr ES:[EDI.Keyb_Buffer_Start],EAX
         Mov   Dword ptr ES:[EDI.Keyb_Buffer_End],EDX
         Mov   word ptr  ES:[EDI.Keyb_Buffer_Flags],KEYB_Initialize_Flags
         Mov   word ptr  ES:[EDI.Keyb_Buffer_Speed],KEYB_Initialize_Speed
         Mov   Dword ptr ES:[EDI.Keyb_Buffer_Write_Pos],EAX
         Mov   Dword ptr ES:[EDI.Keyb_Buffer_Read_Pos],EAX
         Add   EDI,EDX
         Dec   ECX
         Jnz   @Keyb_Init_Initialize_Buffers
         Movzx EAX, Word ptr DS:[Active_Console]
         Mul   EDX
         Mov   EDI,EAX
         Mov   CX,Word ptr Es:[EDI.Keyb_Buffer_Flags]
         And   Cl,7
         Call  Keyb_SetFlags
         Mov   CX,Word ptr  ES:[EDI.Keyb_Buffer_Speed]
         Call  Keyb_SetAutoRepeat
         Mov   Ax,IDT_Descriptor
         Mov   Es,Ax
         CLI
         SetInterrupt 0F1H CS Keyboard_IRQ_Routine,Int_atr1
         In    Al,21h
         And   Al,11111101b
         Out   21h,Al
        POP    ES DS
        POPAD
        POPFD
        RetF
Keyb_Init ENDP
;###########################################################################
;# Deinicializacni procedure
;###########################################################################
KEYB_DONE PROC
     PUSHAD
     PUSH DS
        Mov    DX, Data_Descriptor
        Mov    DS, DX
        Movzx  EAX,Word ptr DS:[Keyb_Buffers_Selector]
        Call   Dealloc
     POP  DS
     POPAD
        RetF
KEYB_DONE ENDP
;###########################################################################
;# Standardni cteni
;# Vstup  : Neni
;# Vystup : eDX - ASCII znak
;###########################################################################
KEYB_READ PROC
       PUSH    EDI ESI EAX ECX EBX ES DS
        Mov    BX,  Data_Descriptor
        Mov    DS,  BX
        Shr    EAX,16
        Mov    ES,  Word ptr DS:[Keyb_Buffers_Selector]
        Cmp    EAX, Max_Console_Count
        Ja     @Keyb_Read_End
        PUSH   EAX
         Mul    Dword ptr DS:[KEYB_Buffer_Size]
         Mov    EDI,EAX
@Keyb_Read_After_Sleeping :
         Mov    ESI,Dword ptr Es:[EDI.Keyb_Buffer_Read_Pos]   ; ESI = odkud se cte
         Cmp    ESI,Dword ptr Es:[EDI.Keyb_Buffer_Write_Pos]
         Je     @Keyb_Read_Sleep_Task
         Cmp    ESI,Dword ptr ES:[EDI.Keyb_Buffer_End]
         Jne    @Keyb_Read_Not_End_Of_Buffer
         Mov    ESI,Dword ptr Es:[EDI.Keyb_Buffer_Start]
         Cmp    ESI,Dword ptr Es:[EDI.Keyb_Buffer_Write_Pos]
         Je     @Keyb_Read_Sleep_Task
    @Keyb_Read_Not_End_Of_Buffer :
         Movzx  EDX, Byte ptr ES:[ESI+EDI]
         Inc    ESI
         Mov    Dword ptr Es:[EDI.Keyb_Buffer_Read_Pos],ESI
        Jmp    @Keyb_Read_End
    @Keyb_Read_sleep_Task :
         Call   TASK_Get_Current_Task_Num
         POP   EDX
         PUSH  EDX
         Shl    EDX,2
         Mov    Dword ptr DS:[KEYB_Tasks_Sleeping+EDX],EBX
         Call   TASK_Sleep_Me
         Mov    Dword ptr DS:[KEYB_Tasks_Sleeping+EDX],0
        Jmp    @Keyb_Read_After_Sleeping
    @Keyb_Read_End :
        POP    EAX
       POP     DS ES EBX ECX EAX ESI EDI
        RetF
KEYB_READ ENDP
;###########################################################################
;# Procedura navraci stav klavesnice
;###########################################################################
;# Vstup   : neni
;# Vystup  : Al - Stav
;# Destroy : nic
;###########################################################################
Keyb_ReadStatus Proc
         In    Al,64h
         RET
Keyb_ReadStatus EndP
;###########################################################################
;# Procedura ceka, dokud neni klavesnice pripravena prijmout prikaz
;###########################################################################
;# Vstup   : neni
;# Vystup  : neni
;# Destroy : AL
;###########################################################################
Keyb_WaitReady  Proc
         Push  ECX
          Mov   ECX,100000h
@Keyb_Wait0001:
          In    Al,64h
          Test  Al,2
          Loopnz @Keyb_Wait0001
          Call  Wait_Loop_010ms
         POP   ECX
         RET
Keyb_WaitReady  EndP
;###########################################################################
;# Procedura ECHO
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = 0EEH - OK
;# Destroy : AL
;###########################################################################
Keyb_Echo   Proc
         Mov    Al,0eeh
         Out    60h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_Echo  EndP
;###########################################################################
;# Procedura nastavuje LED diody na klavesnici
;###########################################################################
;# Vstup   : CL = Flags ... Bit 0 = SrollLock
;#                          Bit 1 = NumLock
;#                          Bit 2 = CapsLock
;# Vystup  : AL = ACK/NACK
;# Destroy : AL
;###########################################################################
Keyb_SetFlags    Proc
         And    Cl,111b
         Mov    Al,0EDh
         Out    60h,Al
         Call   Keyb_WaitReady
         Mov    Al,Cl
         Out    60h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_SetFlags   EndP
;###########################################################################
;# Procedura nastavuje rychlost AutoOpakovani klavesnice
;###########################################################################
;# Vstup   : CL = Zpozdeni pred prcnim opakovanim
;#                          00b = 250ms
;#                          01b = 500ms
;#                          10b = 750ms
;#                          11b = 1000ms
;#           CH = Rychlost opakovani
;#                          00h = 30 znaku/sek
;#                           .
;#                           .
;#                           .
;#                          1fh = 02 znaku/sek
;# Vystup  : AL = ACK/NACK
;# Destroy : AL
;###########################################################################
Keyb_SetAutoRepeat Proc
         And    Cl,3
         Shl    Cl,5
         And    Ch,1Fh
         Or     Cl,Ch
         Mov    Al,0F3h
         Out    60h,Al
         Call   Keyb_WaitReady
         Mov    Al,Cl
         Out    60h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_SetAutoRepeat EndP
;###########################################################################
;# Procedura povoli Keyboard, vymaze vystupni buffer a zacne cist
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = ACK/NACK
;# Destroy : AL
;###########################################################################
Keyb_EnableKeyboard Proc
         Mov    Al,0F4h
         Out    60h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_EnableKeyboard EndP
;###########################################################################
;# Procedura povoli Keyboard, resetuje nastaveni a zacne cist
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = ACK/NACK
;# Destroy : AL
;###########################################################################
Keyb_EnableKeyboard_Reset Proc
         Mov    Al,0F6h
         Out    60h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_EnableKeyboard_Reset Endp
;###########################################################################
;# Procedura zakaze Keyboard, resetuje nastaveni a prestane cist
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = ACK/NACK
;# Destroy : AL
;###########################################################################
Keyb_DisableKeyboard Proc
         Mov    Al,0F5h
         Out    60h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_DisableKeyboard EndP
;###########################################################################
;# Procedura resetuje Keyboard, nastavi nastaveni na firemni status
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = ACK/NACK
;# Destroy : AL
;###########################################################################
Keyb_Reset Proc
         Mov    Al,0FFh
         Out    60h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_Reset EndP
;###########################################################################
;# Procedura navraci systemovy byte klavesnice
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = systemovy byte
;#                  Bit 0 = Povoleni preruseni pri plnnem vystupnim bufferu
;#                  Bit 1 = ???
;#                  Bit 2 = Systemova Vlajka F0
;#                  Bit 3 = ???
;#                  Bit 4 = Klavesnice zakazana (=1)
;#                  Bit 5 = ???
;#                  Bit 6 = ScanCody tak jak jsou (0)
;#                        = Prevod na scancody PC (1)
;#                  Bit 7 = Reserved, must be 0
;# Destroy : AL
;###########################################################################
Keyb_GetSystemByte Proc
         Mov    Al,020h
         Out    64h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_GetSystemByte EndP
;###########################################################################
;# Procedura diagnostikuje klavesnici
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = 55H, if OK
;# Destroy : AL
;###########################################################################
Keyb_Diagnostic Proc
         Mov    Al,0AAh
         Out    64h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_Diagnostic  Endp
;###########################################################################
;# Procedura diagnostikuje interface klavesnice
;###########################################################################
;# Vstup   : neni
;# Vystup  : AL = info
;#                 0h = OK
;#                 1h = hodiny zaseknute v 0
;#                 2h = hodiny zaseknute v 1
;#                 3h = data zaseknuta v 0
;#                 4h = data zaseknuta v 1
;# Destroy : AL
;###########################################################################
Keyb_InterfaceDiagnostic Proc
         Mov    Al,0ABh
         Out    64h,Al
         Call   Keyb_WaitReady
         In     Al,60h
         RET
Keyb_InterfaceDiagnostic EndP
;###########################################################################
;#
;###########################################################################
Keyb_SetFlags_vd PROC
         RetF
Keyb_SetFlags_vd ENDP
;###########################################################################
;#
;###########################################################################
Keyb_SetAutoRepeat_vd PROC
         RetF
Keyb_SetAutoRepeat_vd ENDP
;###########################################################################
;#
;###########################################################################
Keyb_EnableKeyboard_vd PROC
         RetF
Keyb_EnableKeyboard_vd ENDP
;###########################################################################
;#
;###########################################################################
Keyb_DisableKeyboard_vd PROC
         RetF
Keyb_DisableKeyboard_vd ENDP
;###########################################################################
Include   .\..\inc\code\code32\Keyb\Keyb-int.inc
