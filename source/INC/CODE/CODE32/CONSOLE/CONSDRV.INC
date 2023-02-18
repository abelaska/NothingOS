;###################################################################################
;# Driver na konzoly
;####################################################################################
;# Vstupy : EAX  = ccccXXXX, kde
;#          cccc = cislo conzoly
;#          XXXX = Consoles_driver_id
;####################################################################################
Console_Driver_Code:
Console_Driver_Sel_Data  dw      0           ; selector na datasegment daneho driveru
;####################################################################################
;# Inicializacni procedura
;####################################################################################
Console_Driver_INIT PROC
      PUSH  DS
      PUSHAD
       Mov  DX,Data_Descriptor
       Mov  DS,DX
       Mov  EAX,(8*256)
       Call Alloc
       Jc   Short @Console_Driver_INIT_Error
       Mov  Byte ptr DS:[Consoles_Initializing_OK],0FFh
       Mov  Word ptr DS:[Char_Redirection_Descriptor],AX
       Mov  DS,AX
       Mov  EBX,Code_Descriptor
       Mov  EAX,Offset Console_Special_Backward
       Mov  Dword ptr DS:[8*8],EAX
       Mov  Dword ptr DS:[8*8+4],EBX
       Mov  EAX,Offset Console_Special_Tabulator_Horiz
       Mov  Dword ptr DS:[9*8],EAX
       Mov  Dword ptr DS:[9*8+4],EBX
       Mov  EAX,Offset Console_Special_Tabulator_Vert
       Mov  Dword ptr DS:[11*8],EAX
       Mov  Dword ptr DS:[11*8+4],EBX
       Mov  EAX,Offset Console_Special_New_Line
       Mov  Dword ptr DS:[10*8],EAX
       Mov  Dword ptr DS:[10*8+4],EBX
       Mov  EAX,Offset Console_ESCAPE_Character_Procedure
       Mov  Dword ptr DS:[27*8],EAX
       Mov  Dword ptr DS:[27*8+4],EBX
       Jmp  Short @Console_Driver_INIT_End
  @Console_Driver_INIT_Error:
       Mov  Byte ptr DS:[Consoles_Initializing_OK],00h
  @Console_Driver_INIT_End :
      POPAD
      POP  DS
       RetF
Console_Driver_INIT ENDP
;####################################################################################
;# DeInicializacni procedura
;####################################################################################
Console_Driver_DONE PROC
      PUSH  DS
      PUSHAD
       Mov   DX,Data_Descriptor
       Mov   DS,DX
       Movzx EAX,Word ptr DS:[Char_Redirection_Descriptor]
       Or    EAX,EAX
       Jz    @Console_Driver_DONE_Skip
       Call Dealloc
@Console_Driver_DONE_Skip :
      POPAD
      POP   DS
       RetF
Console_Driver_DONE ENDP
;####################################################################################
Console_Driver_READ PROC
       RetF
Console_Driver_READ ENDP
;####################################################################################
;# Procedura vypise na obrazovku znak DL
;####################################################################################
;# Vstup : DL - znak
;#         DH - 0 - korekturovat znak
;#              1 - nekorekturovat znak
;####################################################################################
Console_Driver_WRITE PROC
       PUSH   DS ES EBX EDX
        Mov   BX,Data_Descriptor
        Mov   ES,BX
        Cmp   Byte ptr ES:[Consoles_Initializing_OK],0
        Je    @Console_Driver_WRITENot_Special
        Mov   EBX,EAX
        Shr   EBX,16
        Shr   EAX,16
        Shl   EAX,6
        Or    DH,DH
        Jnz   @Console_Driver_WRITE_NoControl_NonCor
        PUSH  DS EAX EDI ECX
          Movzx  ECX,DL
          Shl    ECX,3
          Mov    DS,Word ptr ES:[Char_Redirection_Descriptor]
          Mov    EAX,Dword ptr DS:[ECX]
          Add    EAX,Dword ptr DS:[ECX+4]
          Or     EAX,EAX
          Jnz    @Console_Driver_Char_Redirection
        POP   EDI EDI EAX DS
          Jmp    @Console_Driver_WRITENot_Special
  @Console_Driver_Char_Redirection :
        POP   EDI EDI EAX
          Call   fword ptr DS:[ECX]
        POP   DS
          Jmp   @Console_Driver_WRITE_OK
  @Console_Driver_WRITENot_Special :
        Cmp   Byte ptr ES:[Consoles_Control_Escape+EBX],1
        Je    @Console_Driver_WRITE_ANSI
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX],0
        Jne   @Console_Driver_WRITE_Control_Seq
  @Console_Driver_WRITE_NoControl_Seq :
        Cmp   DL, Control_Seq_Start
        Je    @Console_Driver_WRITE_Control_Seq_Start
  @Console_Driver_WRITE_NoControl_NonCor :
        Mov   Al,DL
        Call  Console_PutChar_Ext
        Jc    @Console_Driver_WRITE_Error
        Cmp   EBX,Dword ptr ES:[Active_Console]
        Jne   Short @Console_Driver_WRITE_OK
        Call  Console_Redraw
  @Console_Driver_WRITE_OK:
       Xor    EAX,EAX
       POP    EDX EBX ES DS
       RetF
  @Console_Driver_WRITE_Error :
       Xor    EAX,EAX
       Dec    EAX
       POP    EDX EBX ES DS
       RetF
;###### Zahajujeme kontrolni seqvenci
@Console_Driver_WRITE_Control_Seq_Start :
        Mov    Byte ptr Byte ptr ES:[Consoles_Control_Seguence+EAX],DL
        Jmp    Short @Console_Driver_WRITE_OK
;###### Testujeme kontrolni seqvenci
@Console_Driver_WRITE_Control_Seq :
       PUSHAD
        Mov   ECX,64
@Console_Driver_WRITE_Control_Seq2 :
        Inc   EAX
        Mov   DH, Byte ptr ES:[Consoles_Control_Seguence+EAX]
        Or    DH, DH
        Jz    @Console_Driver_WRITE_Control_Seq_FindPos
        Dec   ECX
        Jnz   @Console_Driver_WRITE_Control_Seq2
        Jmp   @Console_Driver_WRITE_Control_Seq_Cmps
@Console_Driver_WRITE_Control_Seq_FindPos :
        Mov   Byte ptr ES:[Consoles_Control_Seguence+EAX],DL
@Console_Driver_WRITE_Control_Seq_Cmps :
       POPAD
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'n'
        Je    @Console_Driver_WRITE_New_Line
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'t'
        Je    @Console_Driver_WRITE_Tabulator
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'v'
        Je    @Console_Driver_WRITE_Tabulator_Vertical
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'b'
        Je    @Console_Driver_WRITE_BackStep
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'r'
        Je    @Console_Driver_WRITE_RightCarrier
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'\'
        Je    @Console_Driver_WRITE_BackSlash
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],39
        Je    @Console_Driver_WRITE_BackSlash
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'?'
        Je    @Console_Driver_WRITE_BackSlash
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+1],'"'
        Je    @Console_Driver_WRITE_BackSlash
        Call  Console_Driver_CmpStr_Oct
        Jnc   @Console_Driver_WRITE_Character
        Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX+4],0
        Jne   @Console_Driver_WRITE_ClearSequence
        Jmp   @Console_Driver_WRITE_OK
;########## Vypiseme znak
@Console_Driver_WRITE_Character :
       PUSHAD
        Mov   ECX,4
        Xor   EDX,EDX
@Console_Driver_WRITE_ClearSequence_Loop :
        Mov   Dword ptr ES:[Consoles_Control_Seguence+EAX],EDX
        Add   EAX,4
        Dec   ECX
        Jnz   @Console_Driver_WRITE_ClearSequence_Loop
       POPAD
        Mov   EAX, EBX
        Shl   EAX, 16
        Or    EAX, Consoles_driver
        Mov   EBX, Drv_Std_WRITE
        Call  Drv_Gate
        Jmp   @Console_Driver_WRITE_OK
;########## Odradkujeme
@Console_Driver_WRITE_New_Line :
       PUSHAD
        Mov   EAX, EBX
        Call  Get_Console_Addr
        Call  Console_GetXY
        Inc   EBX
        Mov   EAX, 1
        Call  Console_GotoXY
       POPAD
       Call  Console_Show_Cursor_Extended
        Jmp   @Console_Driver_WRITE_ClearSequence
;########## skocime zpatky na zacatek radku
@Console_Driver_WRITE_RightCarrier :
       PUSHAD
        Mov   EAX, EBX
        Call  Get_Console_Addr
        Call  Console_GetXY
        Mov   EAX, 1
        Call  Console_GotoXY
       POPAD
        Call  Console_Show_Cursor_Extended
        Jmp   @Console_Driver_WRITE_ClearSequence
;########## Horizontalni tabulator
@Console_Driver_WRITE_Tabulator :
       PUSHAD
        Mov    EAX, EBX
        Call   Get_Console_Addr
        Call   Console_GetXY
        Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
        Add    EAX,dword ptr ES:[Horizontal_Tab]
        Cmp    EAX,ECX
        Jna    @Console_Driver_WRITE_Tabulator_OK
        Mov    EAX,ECX
   @Console_Driver_WRITE_Tabulator_OK :
        Call  Console_GotoXY
       POPAD
        Call  Console_Show_Cursor_Extended
        Jmp   @Console_Driver_WRITE_ClearSequence
;########## Vertikalni tabulator
@Console_Driver_WRITE_Tabulator_Vertical :
       PUSHAD
        Mov    EAX, EBX
        Call   Get_Console_Addr
        Call   Console_GetXY
        Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_Y]
        Add    EBX,dword ptr ES:[Vertical_Tab]
        Cmp    EBX,ECX
        Jna    @Console_Driver_WRITE_VTabulator_OK
        Mov    EBX,ECX
   @Console_Driver_WRITE_VTabulator_OK :
        Call  Console_GotoXY
       POPAD
        Call  Console_Show_Cursor_Extended
        Jmp   @Console_Driver_WRITE_ClearSequence
;########## o Jednu pozici doleva
@Console_Driver_WRITE_BackStep :
       PUSHAD
        Mov   EAX, EBX
        Call  Get_Console_Addr
        Call  Console_GetXY
        Dec   EAX
        Jnz   Short @Console_Driver_WRITE_BackStep_OK
        Movzx EAX,word ptr DS:[ESI.Console_Virt_Res_X]
        Dec   EBX
        Jnz   Short @Console_Driver_WRITE_BackStep_OK
        Inc   EBX
        Jmp   Short @Console_Driver_WRITE_BackStep_Skip
@Console_Driver_WRITE_BackStep_OK:
        Call  Console_GotoXY
@Console_Driver_WRITE_BackStep_Skip :
       POPAD
        Call  Console_Show_Cursor_Extended
        Jmp   @Console_Driver_WRITE_ClearSequence
;########## vypiseme znak \,",',?
@Console_Driver_WRITE_BackSlash :
       PUSHAD
        Mov   Al,Byte ptr ES:[Consoles_Control_Seguence+EAX+1]
        Call  Console_PutChar_Ext
        Cmp   EBX,Dword ptr ES:[Active_Console]
        Jne   Short @Console_Driver_WRITE_BackSlash_Skip
        Call  Console_Redraw
@Console_Driver_WRITE_BackSlash_Skip :
       POPAD
        Jmp   @Console_Driver_WRITE_ClearSequence
;########## sekvence ANSI
@Console_Driver_WRITE_ANSI :
        PUSHAD
         Mov   ESI, EAX
         Add   ESI, Offset Consoles_Control_Seguence
         Mov   ECX, 64
@Console_Driver_WRITE_ANSI_Find_Free :
         Cmp   Byte ptr ES:[ESI],0
         Je    @Console_Driver_WRITE_ANSI_Find_Free_Found
         Inc   ESI
         Dec   ECX
         Jnz   @Console_Driver_WRITE_ANSI_Find_Free
        POPAD
         Mov   Byte ptr ES:[Consoles_Control_Escape+EBX],0
         Jmp   @Console_Driver_WRITE_ClearSequence
@Console_Driver_WRITE_ANSI_Find_Free_Found :
         Mov   Byte ptr ES:[ESI],DL
         Cmp   Byte ptr ES:[Consoles_Control_Seguence+EAX],'['
         Je    @Console_Driver_WRITE_ANSI_OK1
        POPAD
         Mov   Byte ptr ES:[Consoles_Control_Escape+EBX],0
         Jmp   @Console_Driver_WRITE_ClearSequence
@Console_Driver_WRITE_ANSI_OK1 :
         Mov   EDX,Offset Consoles_Control_Seguence
         Add   EDX,EAX
         Sub   ESI,EDX
         Jz    @Console_Driver_WRITE_ANSI_End
         Inc   EDX
         Mov   EDI, Offset CCS_ANSI_EndChars
         Mov   ECX, CCS_ANSI_EndChars_Num
         Xor   EBP,EBP
@Console_Driver_Find_Fce :
         PUSH  ECX EDI
          Mov   Al, Byte ptr ES:[EDX]
          Repnz  Scasb
          Jz   @Console_Driver_Find_Fce_Found
         POP   EDI ECX
          Inc   EBP
          Inc   EDX
          Dec   ESI
          Jnz   @Console_Driver_Find_Fce
          Jmp   @Console_Driver_WRITE_ANSI_End
@Console_Driver_Find_Fce_Found :
         Sub    ECX,CCS_ANSI_EndChars_Num
         Neg    ECX
         Dec    ECX
         Mov    ESI,EDX
         Sub    ESI,EBP
         Mov    EDX,EBP
         Dec    ESI
         ;ES:[ESI] - zacatek parametru sekvence
         ;EDX - kolik znaku ma sekveknce parametru sekvence
         ;EBX - cislo consoly, na ktere pracujeme
         ;ECX - cislo fce
         Shl    ECX,2
         Call   Dword ptr ES:[CCS_ANSI_FCE.ECX]
        POP    EDI ECX
        POPAD
         Mov   Byte ptr ES:[Consoles_Control_Escape+EBX],0
         Call  Console_Show_Cursor_Extended
         Jmp   @Console_Driver_WRITE_ClearSequence
@Console_Driver_WRITE_ANSI_End :
        POPAD
         Jmp   @Console_Driver_WRITE_OK
;########## vynulovani sekvence
@Console_Driver_WRITE_ClearSequence :
       PUSHAD
        Mov   ECX,4
        Xor   EDX,EDX
@Console_Driver_WRITE_ClearSequence_Loop1 :
        Mov   Dword ptr ES:[Consoles_Control_Seguence+EAX],EDX
        Add   EAX,4
        Dec   ECX
        Jnz   @Console_Driver_WRITE_ClearSequence_Loop1
       POPAD
        Jmp   @Console_Driver_WRITE_OK
Console_Driver_WRITE ENDP
;####################################################################################
;# Navrati addresu obsluzne rutiny pro danny znak
;####################################################################################
;# Vstup : Dl  - Znak
;# Vstup : ECX - Selector
;#         EDI - Offset
;####################################################################################
Console_Driver_Get_Char_Redir_Addr PROC
         PUSH  DS EDX
          Mov    ECX,Data_Descriptor
          Mov    DS,ECX
          Cmp    Byte ptr DS:[Consoles_Initializing_OK],0
          Je     Console_Driver_Get_Char_Redir_Addr_End
          Movzx  EDX,DL
          Shl    EDX,3
          Mov    DS,Word ptr DS:[Char_Redirection_Descriptor]
          Mov    ECX,Dword ptr DS:[EDX+4]
          Mov    EDI,Dword ptr DS:[EDX]
Console_Driver_Get_Char_Redir_Addr_End :
         POP   EDX DS
        RetF
Console_Driver_Get_Char_Redir_Addr ENDP
;####################################################################################
;# Nastaveni addresy obsluzne rutiny pro danny znak
;####################################################################################
;# Vstup : Dl  - Znak
;#         ECX - Selector
;#         EDI - Offset
;####################################################################################
Console_Driver_Set_Char_Redir_Addr PROC
         PUSH  DS EDX
          PUSH   Data_Descriptor
          POP    DS
          Cmp    Byte ptr DS:[Consoles_Initializing_OK],0
          Je     Console_Driver_Set_Char_Redir_Addr_End
          Movzx  EDX,DL
          Shl    EDX,3
          Mov    DS,Word ptr DS:[Char_Redirection_Descriptor]
          Mov    Dword ptr DS:[EDX+4],ECX
          Mov    Dword ptr DS:[EDX],EDI
Console_Driver_Set_Char_Redir_Addr_End :
         POP   EDX DS
        RetF
Console_Driver_Set_Char_Redir_Addr ENDP
;####################################################################################
;# Odscrollovani obrazovky na EDX pozici
;####################################################################################
;# Vstup : EDX - Od kolikateho radku scrolovat
;####################################################################################
Console_Driver_Scroll PROC
       PUSH   ES DS ESI
         Push  Data_Descriptor
         Pop   ES
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    Short @Console_Driver_Scroll_error
         PUSH  EAX
          Mov   EAX,EDX
          Call  Console_Scroll
         PUSH  EAX
         Cmp   EAX,Dword ptr ES:[Active_Console]
         Jne   Short @Console_Driver_Scroll_OK
         Call  Console_Redraw
    @Console_Driver_Scroll_OK :
         Xor   EAX,EAX
         Jmp   Short @Console_Driver_Scroll_End
    @Console_Driver_Scroll_error :
         Xor   EAX,EAX
         Inc   EAX
    @Console_Driver_Scroll_end :
       POP    ESI DS ES
       RetF
Console_Driver_Scroll ENDP
;####################################################################################
;# Scrollovani obrazovky o EDX (+/-)
;####################################################################################
;# Vstup : EDX - O kolik
;####################################################################################
Console_Driver_ScrollDelta PROC
       PUSH   ES DS ESI
         Push  Data_Descriptor
         Pop   ES
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    @Console_Driver_Scroll_Delta_error
         PUSH  EAX
          Mov   EAX,EDX
          Call  Console_ScrollDelta
         POP   EAX
         Cmp   EAX,Dword ptr ES:[Active_Console]
         Jne   Short @Console_Driver_Scroll_Delta_OK
         Call  Console_Redraw
    @Console_Driver_Scroll_Delta_OK :
         Xor   EAX,EAX
         Jmp   Short @Console_Driver_Scroll_Delta_End
    @Console_Driver_Scroll_Delta_error :
         Xor   EAX,EAX
         Inc   EAX
    @Console_Driver_Scroll_Delta_end :
       POP    ESI DS ES
       RetF
Console_Driver_ScrollDelta ENDP
;####################################################################################
;# Vymaze obrazovku
;####################################################################################
Console_Driver_Clear PROC
       PUSH   ES DS ESI
         Push  Data_Descriptor
         Pop   ES
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    Short @Console_Driver_Clear_error
         Call  Console_Clear
         Cmp   EAX,Dword ptr ES:[Active_Console]
         Jne   Short @Console_Driver_Clear_OK
         Call  Console_Redraw
    @Console_Driver_Clear_OK :
         Xor   EAX,EAX
         Jmp   Short @Console_Driver_Clear_End
    @Console_Driver_Clear_error :
         Xor   EAX,EAX
         Inc   EAX
    @Console_Driver_Clear_end :
       POP    ESI DS ES
       RetF
Console_Driver_Clear ENDP
;####################################################################################
;# Nastaveni atributu
;# Vstup : DL - Attribut
;####################################################################################
Console_Driver_SetAttrib PROC
       PUSH   DS ESI
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    Short @Console_Attr_Error
         Mov   Byte ptr DS:[ESI.Console_Color_Atr],DL
    @Console_Attr_Error :
       POP    ESI DS
       RetF
Console_Driver_SetAttrib ENDP
;####################################################################################
;# zobrazeni kursoru
;####################################################################################
Console_Driver_Show_Cursor PROC
       PUSH   ES DS ESI
         Push  Data_Descriptor
         Pop   ES
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    Short @Console_Driver_Show_Cursor_error
         Call  Console_Show_Cursor
         Cmp   EAX,Dword ptr ES:[Active_Console]
         Jne   Short @Console_Driver_Show_Cursor_OK
         Call  Console_Redraw
    @Console_Driver_Show_Cursor_OK :
         Xor   EAX,EAX
         Jmp   Short @Console_Driver_Show_Cursor_End
    @Console_Driver_Show_Cursor_error :
         Xor   EAX,EAX
         Inc   EAX
    @Console_Driver_Show_Cursor_end :
       POP    ESI DS ES
       RetF
Console_Driver_Show_Cursor ENDP
;####################################################################################
;# Schovani kursoru
;####################################################################################
Console_Driver_Hide_Cursor PROC
       PUSH   ES DS ESI
         Push  Data_Descriptor
         Pop   ES
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    Short @Console_Driver_Hide_Cursor_error
         Call  Console_Hide_Cursor
         Cmp   EAX,Dword ptr ES:[Active_Console]
         Jne   Short @Console_Driver_Hide_Cursor_OK
         Call  Console_Redraw
    @Console_Driver_Hide_Cursor_OK :
         Xor   EAX,EAX
         Jmp   Short @Console_Driver_Hide_Cursor_End
    @Console_Driver_Hide_Cursor_error :
         Xor   EAX,EAX
         Inc   EAX
    @Console_Driver_Hide_Cursor_end :
       POP    ESI DS ES
       RetF
Console_Driver_Hide_Cursor ENDP
;####################################################################################
;# Presune kurzor na presne dannou pozici
;# Vstup : ECX - X
;#         EDX - Y
;####################################################################################
Console_Driver_GotoXY PROC
       PUSH   ES DS ESI
         Push  Data_Descriptor
         Pop   ES
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    Short @Console_Driver_GotoXY_error
         PUSH  EAX
          Mov   EAX,ECX
          Mov   EAX,EDX
          Call  Console_GotoXY
         PUSH  EAX
          Jc   @Console_Driver_GotoXY_error
         Cmp   EAX,Dword ptr ES:[Active_Console]
         Jne   Short @Console_Driver_GotoXY_OK
         Call  Console_Show_Cursor_Extended
    @Console_Driver_GotoXY_OK :
         Xor   EAX,EAX
         Jmp   Short @Console_Driver_GotoXY_End
    @Console_Driver_GotoXY_error :
         Xor   EAX,EAX
         Inc   EAX
    @Console_Driver_GotoXY_end :
       POP    ESI DS ES
       RetF
Console_Driver_GotoXY ENDP
;####################################################################################
;# Navrati pozici kurzoru
;# Vystup : ECX - X
;#          EDX - Y
;####################################################################################
Console_Driver_GetXY PROC
       PUSH   DS ESI EBX
         Shr   EAX,16
         Call  Get_Console_Addr
         Jc    Short @Console_Driver_GetXY_error
         Call  Console_GetXY
         Mov   ECX,EAX
         Mov   EDX,EBX
    @Console_Driver_GetXY_OK :
         Xor   EAX,EAX
         Jmp   Short @Console_Driver_GetXY_End
    @Console_Driver_GetXY_error :
         Xor   EAX,EAX
         Inc   EAX
    @Console_Driver_GetXY_end :
       POP    EBX ESI DS
       RetF
Console_Driver_GetXY ENDP
;####################################################################################
;# Prida jeden radek
;####################################################################################
Console_Driver_AddLine PROC
        Shr   EAX,16
        Call  Console_AddLine_Ext
       RetF
Console_Driver_AddLine ENDP
;####################################################################################
;# Prepne na tuto konzolu
;####################################################################################
Console_Driver_Switch PROC
        Shr   EAX,16
        Call  Console_Switch
       RetF
Console_Driver_Switch ENDP
;####################################################################################
;# Navrati cislo aktivni konzoly
;# Vystyp  : EAX - Console
;####################################################################################
Console_Driver_GetActive_Console PROC
       PUSH  DS
        Mov  EAX,Data_Descriptor
        Mov  DS,EAX
        Movzx eax, word ptr ds:[Active_Console]
       POP   DS
       RetF
Console_Driver_GetActive_Console ENDP
;####################################################################################
;# Navrati informaci o tom, existuje-li danna konzola
;# EAX - 0000 - ANO Else Ne
;####################################################################################
Console_Driver_exist_Console PROC
        Shr    EAX,16
        Call   exist_Console
        Xor    EAX,EAX
        Jnc    @Console_Driver_exist_Console_End
        Dec    EAX
@Console_Driver_exist_Console_End :
       RetF
Console_Driver_exist_Console ENDP
Console_Driver_CodeEnd Equ $
Console_Driver_CODE_Size equ (((Console_Driver_CodeEnd - Console_Driver_Code-1) Shr 12)+1)
;####################################################################################
;# ANSI specification
;####################################################################################

;####################################################################################
;# Posune kurzor o n radku nahoru
;####################################################################################
CCS_ANSI_Cursor_Up PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        PUSH   EAX
         Call   CCS_Return_Num
         Jc     @CCS_ANSI_Cursor_Up_Error
         Sub    EBX, EAX
        POP    EAX
         Jns   Short @Console_Driver_CCS_ANSI_Cursor_Up_OK
         Mov   EBX,1
         Jmp   Short @Console_Driver_CCS_ANSI_Cursor_Up_OK2
  @Console_Driver_CCS_ANSI_Cursor_Up_OK :
         Or    EBX,EBX
         Jnz   Short @Console_Driver_CCS_ANSI_Cursor_Up_OK2
         Inc   EBX
  @Console_Driver_CCS_ANSI_Cursor_Up_OK2 :
        Call  Console_GotoXY
        RET
  @CCS_ANSI_Cursor_Up_Error :
        POP    EAX
        RET
CCS_ANSI_Cursor_Up ENDP
;####################################################################################
;# Posune kurzor o n radku dolu
;####################################################################################
CCS_ANSI_Cursor_Down PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        PUSH   EAX
         Call   CCS_Return_Num
         Jc     @Console_Driver_CCS_ANSI_Cursor_Down_Err
         Add    EBX, EAX
        POP    EAX
        Mov    ECX, Dword ptr DS:[ESI.Console_Virt_Res_Y]
        Cmp    EBX, ECX
        Jna    @Console_Driver_CCS_ANSI_Cursor_Down_OK
        Mov    EBX, ECX
  @Console_Driver_CCS_ANSI_Cursor_Down_OK :
        Call  Console_GotoXY
        RET
  @Console_Driver_CCS_ANSI_Cursor_Down_Err :
        POP    EAX
        RET
CCS_ANSI_Cursor_Down ENDP
;####################################################################################
;# Posune kurzor o n znaku dopredu
;####################################################################################
CCS_ANSI_Cursor_Forward PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        PUSH   EAX
         Call   CCS_Return_Num
         Mov    ECX, EAX
         Jc     @Console_Driver_CCS_ANSI_Cursor_Forward_Err
        POP    EAX
        Add    EAX, ECX
        Mov    ECX, Dword ptr DS:[ESI.Console_Virt_Res_X]
        Cmp    EAX, ECX
        Jna    @Console_Driver_CCS_ANSI_Cursor_Forward_OK
        Mov    EAX, ECX
  @Console_Driver_CCS_ANSI_Cursor_Forward_OK :
        Call  Console_GotoXY
        RET
  @Console_Driver_CCS_ANSI_Cursor_Forward_Err :
        POP    EAX
        RET
CCS_ANSI_Cursor_Forward ENDP
;####################################################################################
;# Posune kurzor o n znaku dozadu
;####################################################################################
CCS_ANSI_Cursor_Backward PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        PUSH   EAX
         Call   CCS_Return_Num
         Jc     @Console_Driver_CCS_ANSI_Cursor_BeckWard_Err
         Mov    ECX, EAX
        POP    EAX
        Sub    EAX, ECX
        Js     Short @Console_Driver_CCS_ANSI_Cursor_BeckWard_1
        Or     EAX, EAX
        Jnz    @Console_Driver_CCS_ANSI_Cursor_BeckWard_OK
  @Console_Driver_CCS_ANSI_Cursor_BeckWard_1 :
        Mov    EAX, 1
  @Console_Driver_CCS_ANSI_Cursor_BeckWard_OK :
        Call  Console_GotoXY
        RET
  @Console_Driver_CCS_ANSI_Cursor_BeckWard_Err :
        POP    EAX
        RET
CCS_ANSI_Cursor_Backward ENDP
;####################################################################################
;# Procedura nastavuje kurzor na hodnoty zadane v retezci
;####################################################################################
CCS_ANSI_Cursor_position PROC
        Mov     EAX, EBX
        Mov     EDI,ESI
        Call    Get_Console_Addr
        Mov     Al,';'
        Call    CCS_Return_Char_Pos
        Jnc     @CCS_ANSI_Cursor_Position_OK1
        Mov     Al,','
        Call    CCS_Return_Char_Pos
        Jnc     @CCS_ANSI_Cursor_Position_OK1
        Xor     EAX,EAX
        Inc     EAX
        Mov     EBX,EAX
        Jmp     @CCS_ANSI_Cursor_Position_YOK2
   @CCS_ANSI_Cursor_Position_OK1 :
        PUSH   EDX
         Mov    EDX,EAX
         Call   CCS_Return_Num
         Jc     @CCS_ANSI_Cursor_Position_Err
         Mov    EBX,EAX
         Inc    EDX
         Add    EDI,EDX
         Mov    ECX,EDX
        POP    EDX
         Sub    EDX,ECX
        PUSH    EBX
         Call   CCS_Return_Num
         Jc     @CCS_ANSI_Cursor_Position_Err
        POP     EBX
        Mov    ECX, Dword ptr DS:[ESI.Console_Virt_Res_X]
        Cmp    EAX, ECX
        Jna    Short @CCS_ANSI_Cursor_Position_XOK
        Mov    EAX, ECX
   @CCS_ANSI_Cursor_Position_XOK :
        Mov    ECX, Dword ptr DS:[ESI.Console_Virt_Res_Y]
        Cmp    EBX, ECX
        Jna    Short @CCS_ANSI_Cursor_Position_YOK
        Mov    EBX, ECX
   @CCS_ANSI_Cursor_Position_YOK:
        Or     EAX,EAX
        Jnz    @CCS_ANSI_Cursor_Position_XOK2
        Inc    EAX
   @CCS_ANSI_Cursor_Position_XOK2:
        Or     EBX,EBX
        Jnz    @CCS_ANSI_Cursor_Position_YOK2
        Inc    EBX
   @CCS_ANSI_Cursor_Position_YOK2:
       ;EAX = X position
       ;EBX = Y position
        Call    Console_GotoXY
   @CCS_ANSI_Cursor_Position_End :
        RET
   @CCS_ANSI_Cursor_Position_Err :
        POP     EBX
        RET
CCS_ANSI_Cursor_position ENDP
;####################################################################################
;# Ulozi pozici kurzoru pro dannou konzolu
;####################################################################################
CCS_ANSI_Save_cursor_position PROC
         Mov    EAX, EBX
         PUSH   EAX
          Call   Get_Console_Addr
          Call   Console_GetXY
         POP    EDX
         Shl    EDX,3
         Mov    Dword ptr ES:[Consoles_Cursor_Saved+EDX],EAX
         Add    EDX,4
         Mov    Dword ptr ES:[Consoles_Cursor_Saved+EDX],EBX
        RET
CCS_ANSI_Save_cursor_position ENDP
;####################################################################################
;# Obnovi pozici kurzoru z ulozenych hodnot
;####################################################################################
CCS_ANSI_Restore_cursor_position PROC
         Mov    EAX, EBX
         PUSH   EAX
          Call   Get_Console_Addr
         POP    EDX
         Shl    EDX,3
         Mov    EAX,Dword ptr ES:[Consoles_Cursor_Saved+EDX]
         Add    EDX,4
         Mov    EBX,Dword ptr ES:[Consoles_Cursor_Saved+EDX]
         Call   Console_GotoXY
        RET
CCS_ANSI_Restore_cursor_position ENDP
;####################################################################################
;# Vymaze cast displaye
;# Cislo v retezci udava - 0 - od pozice kurzoru do konce
;#                         1 - od zacatku do pozice kurzoru
;#                         2 - cela obrazovka
;####################################################################################
CCS_ANSI_Erase_display PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   CCS_Return_Num
        Or     EAX,EAX
        Jnz    CCS_ANSI_Erase_display_Next1
        ;Mazeme od kurzoru do konce obrazovky
        Movzx  EDX,Word ptr DS:[ESI.Console_Scr_Buf]
        Or     EDX,EDX
        Jz     CCS_ANSI_Erase_display_Next3
        PUSH   ES
         Mov    ES,DX
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_Y]
         Dec    EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Mul    ECX
         Movzx  ECX,word ptr DS:[ESI.Console_Cur_X]
         Dec    ECX
         Add    EAX,ECX
         Mov    EDI,EAX
         Movzx  EAX,word ptr DS:[ESI.Console_Virt_Res_X]
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_Y]
         Mul    ECX
         Mov    ECX,EAX
         Sub    ECX,EDI
         Shl    EDI,1
         Mov    AX,0720h
         Rep    Stosw
        POP    ES
         Jmp    CCS_ANSI_Erase_display_Refresh
  CCS_ANSI_Erase_display_Next1 :
        Dec    EAX
        Jnz    Short CCS_ANSI_Erase_display_Next2
        Movzx  EDX,Word ptr DS:[ESI.Console_Scr_Buf]
        Or     EDX,EDX
        Jz     CCS_ANSI_Erase_display_Next3
        ;Mazeme od zacatku obrazovky do kurzoru
        PUSH   ES
         Mov    ES,DX
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_Y]
         Dec    EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Mul    ECX
         Movzx  ECX,word ptr DS:[ESI.Console_Cur_X]
         Dec    ECX
         Add    ECX,EAX
         Xor    EDI,EDI
         Mov    AX,0720h
         Rep    Stosw
        POP    ES
         Jmp    CCS_ANSI_Erase_display_Refresh
  CCS_ANSI_Erase_display_Next2 :
        Dec    EAX
        Jnz    Short CCS_ANSI_Erase_display_Next3
        ;Mazeme celou obrazovku
         Call  Console_Clear
  CCS_ANSI_Erase_display_Refresh :
        Cmp    EBX,Dword ptr ES:[Active_Console]
        Jne    Short CCS_ANSI_Erase_display_Next3
        Call   Console_Redraw
  CCS_ANSI_Erase_display_Next3 :
        RET
CCS_ANSI_Erase_display ENDP
;####################################################################################
;# Maze cast radku
;####################################################################################
CCS_ANSI_Erase_to_eol PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   CCS_Return_Num
        Or     EAX,EAX
        Jnz    CCS_ANSI_Erase_EOP_Next1
        ;Mazeme od kurzoru do konce radky
        Movzx  EDX,Word ptr DS:[ESI.Console_Scr_Buf]
        Or     EDX,EDX
        Jz     CCS_ANSI_Erase_EOP_Next3
        PUSH   ES
         Mov    ES,DX
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_Y]
         Dec    EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Mul    ECX
         Movzx  ECX,word ptr DS:[ESI.Console_Cur_X]
         Dec    ECX
         Add    EAX,ECX
         Mov    EDI,EAX
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_X]
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Sub    ECX,EAX
         Shl    EDI,1
         Mov    AX,0720h
         Rep    Stosw
        POP    ES
         Jmp    CCS_ANSI_Erase_EOP_Refresh
  CCS_ANSI_Erase_EOP_Next1 :
        Dec    EAX
        Jnz    Short CCS_ANSI_Erase_EOP_Next2
        Movzx  EDX,Word ptr DS:[ESI.Console_Scr_Buf]
        Or     EDX,EDX
        Jz     CCS_ANSI_Erase_EOP_Next3
        ;Mazeme od zacatku line do kurzoru
        PUSH   ES
         Mov    ES,DX
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_Y]
         Dec    EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Mul    ECX
         Mov    EDI,EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Cur_X]
         Shl    EDI,1
         Mov    AX,0720h
         Rep    Stosw
        POP    ES
         Jmp    CCS_ANSI_Erase_EOP_Refresh
  CCS_ANSI_Erase_EOP_Next2 :
        Dec    EAX
        Jnz    Short CCS_ANSI_Erase_EOP_Next3
        ;Mazeme celou line
        Movzx  EDX,Word ptr DS:[ESI.Console_Scr_Buf]
        Or     EDX,EDX
        Jz     CCS_ANSI_Erase_EOP_Next3
        PUSH   ES
         Mov    ES,DX
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_Y]
         Dec    EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Mul    ECX
         Mov    EDI,EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Shl    EDI,1
         Mov    AX,0720h
         Rep    Stosw
        POP    ES
  CCS_ANSI_Erase_EOP_Refresh :
        Cmp    EBX,Dword ptr ES:[Active_Console]
        Jne    Short CCS_ANSI_Erase_EOP_Next3
        Call   Console_Redraw
  CCS_ANSI_Erase_EOP_Next3 :
        RET
CCS_ANSI_Erase_to_eol  ENDP
;####################################################################################
;# Presune kurzor absolutne na znak n
;####################################################################################
CCS_ANSI_Move_Cursor_2CharPos PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        Call   CCS_Return_Num
        Jc     @Console_Driver_CCS_ANSI_Cursor_2CharPos_Err
        Or     EAX,EAX
        Jnz    @Console_Driver_CCS_ANSI_Cursor_2CharPos_OK1
        Inc    EAX
        Jmp    @Console_Driver_CCS_ANSI_Cursor_2CharPos_OK
  @Console_Driver_CCS_ANSI_Cursor_2CharPos_OK1 :
        Movzx  ECX, word ptr ES:[ESI.Console_Virt_Res_X]
        Cmp    EAX, ECX
        Jnb    @Console_Driver_CCS_ANSI_Cursor_2CharPos_OK
        Mov    EAX, ECX
  @Console_Driver_CCS_ANSI_Cursor_2CharPos_OK :
        Call  Console_GotoXY
  @Console_Driver_CCS_ANSI_Cursor_2CharPos_Err :
        RET
CCS_ANSI_Move_Cursor_2CharPos ENDP
;####################################################################################
;# Presune kurzor na line n
;####################################################################################
CCS_ANSI_Move_Cursor_2Line PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        PUSH   EAX
         Call   CCS_Return_Num
         Mov    EBX,EAX
        POP    EAX
        Jc     @Console_Driver_CCS_ANSI_Cursor_2LinePos_Err
        Or     EBX,EBX
        Jnz    @Console_Driver_CCS_ANSI_Cursor_2LinePos_OK1
        Inc    EBX
        Jmp    @Console_Driver_CCS_ANSI_Cursor_2LinePos_OK
  @Console_Driver_CCS_ANSI_Cursor_2LinePos_OK1 :
        Movzx  ECX, word ptr ES:[ESI.Console_Virt_Res_Y]
        Cmp    EBX, ECX
        Jnb    @Console_Driver_CCS_ANSI_Cursor_2LinePos_OK
        Mov    EBX, ECX
  @Console_Driver_CCS_ANSI_Cursor_2LinePos_OK :
        Call  Console_GotoXY
  @Console_Driver_CCS_ANSI_Cursor_2LinePos_Err :
        RET
CCS_ANSI_Move_Cursor_2Line ENDP
;####################################################################################
;# Posune kurzor o n line nahoru, prvni znak
;####################################################################################
CCS_ANSI_Cursor_Up_StartLine PROC
         Mov    EAX, EBX
         Mov    EDI,ESI
         Call   Get_Console_Addr
         Call   Console_GetXY
         Call   CCS_Return_Num
         Jc     @CCS_ANSI_Cursor_Up_Start_Line_Error
         Sub    EBX, EAX
         Jns    Short @Console_Driver_CCS_ANSI_Cursor_Up_Start_Line_OK
         Mov    EBX,1
         Jmp    Short @Console_Driver_CCS_ANSI_Cursor_Up_Start_Line_OK2
  @Console_Driver_CCS_ANSI_Cursor_Up_Start_Line_OK :
         Or     EBX,EBX
         Jnz    Short @Console_Driver_CCS_ANSI_Cursor_Up_Start_Line_OK2
         Inc    EBX
  @Console_Driver_CCS_ANSI_Cursor_Up_Start_Line_OK2 :
        Xor     EAX,EAX
        Inc     EAX
        Call   Console_GotoXY
  @CCS_ANSI_Cursor_Up_Start_Line_Error :
        RET
CCS_ANSI_Cursor_Up_StartLine ENDP
;####################################################################################
;# Posune kurzor o n line dolu, prvni znak
;####################################################################################
CCS_ANSI_Cursor_Down_StartLine PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        Call   CCS_Return_Num
        Jc     @Console_Driver_CCS_ANSI_Cursor_Down_StartLine_Err
        Add    EBX, EAX
        Mov    ECX, Dword ptr DS:[ESI.Console_Virt_Res_Y]
        Cmp    EBX, ECX
        Jna    @Console_Driver_CCS_ANSI_Cursor_Down_StartLine_OK
        Mov    EBX, ECX
  @Console_Driver_CCS_ANSI_Cursor_Down_StartLine_OK :
        Xor    EAX,EAX
        Inc    EAX
        Call  Console_GotoXY
  @Console_Driver_CCS_ANSI_Cursor_Down_StartLine_Err :
        RET
CCS_ANSI_Cursor_Down_StartLine ENDP
;####################################################################################
;# Vrati se o n tabulatoru zpet
;####################################################################################
CCS_ANSI_Cursor_Back_Tabs PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   Console_GetXY
        PUSH   EAX
         Call   CCS_Return_Num
         Jc     @Console_Driver_CCS_ANSI_Cursor_BackTabs_Err
         Mov    ECX, Dword ptr ES:[Horizontal_Tab]
         Mul    ECX
         Mov    ECX, EAX
        POP    EAX
        Sub    EAX, ECX
        Js     Short @Console_Driver_CCS_ANSI_Cursor_BackTabs_1
        Or     EAX, EAX
        Jnz    @Console_Driver_CCS_ANSI_Cursor_BackTabs_OK
  @Console_Driver_CCS_ANSI_Cursor_BackTabs_1 :
        Mov    EAX, 1
  @Console_Driver_CCS_ANSI_Cursor_BackTabs_OK :
        Call  Console_GotoXY
        RET
  @Console_Driver_CCS_ANSI_Cursor_BackTabs_Err :
        POP    EAX
        RET
CCS_ANSI_Cursor_Back_Tabs ENDP
;####################################################################################
;# Vymaze n znaku od pozice kurzoru
;####################################################################################
CCS_ANSI_Erase_Characters PROC
        Mov    EAX, EBX
        Mov    EDI,ESI
        Call   Get_Console_Addr
        Call   CCS_Return_Num
        Jc     CCS_ANSI_Erase_Chars_Next3
        Mov    EBP,EAX
        ;Mazeme od kurzoru do konce radky
        Movzx  EDX,Word ptr DS:[ESI.Console_Scr_Buf]
        Or     EDX,EDX
        Jz     CCS_ANSI_Erase_Chars_Next3
        PUSH   ES
         Mov    ES,DX
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_Y]
         Dec    EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Mul    ECX
         Movzx  ECX,word ptr DS:[ESI.Console_Cur_X]
         Dec    ECX
         Add    EAX,ECX
         Mov    EDI,EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_X]
         Sub    ECX,EAX
         Mov    EAX,ECX
         Sub    EAX,EBP
         Jns    CCS_ANSI_Erase_Chars_Next1
         Mov    EBP,ECX
   CCS_ANSI_Erase_Chars_Next1 :
         Mov    ECX,EBP
         Shl    EDI,1
         Mov    AX,0720h
         Rep    Stosw
        POP    ES
  CCS_ANSI_Erase_Chars_Refresh :
        Cmp    EBX,Dword ptr ES:[Active_Console]
        Jne    Short CCS_ANSI_Erase_Chars_Next3
        Call   Console_Redraw
  CCS_ANSI_Erase_Chars_Next3 :
        RET
CCS_ANSI_Erase_Characters ENDP
;####################################################################################
;# Nastavuje barvu pisma
;####################################################################################
CCS_ANSI_Set_Graphics_Rendition PROC
        Mov     EAX, EBX
        Mov     EDI,ESI
        Call    Get_Console_Addr
@CCS_ANSI_Set_Graphics_Rendition_Main :
        Mov     Al,';'
        Call    CCS_Return_Char_Pos
        Jc      @CCS_ANSI_GR_Rend_OK1
        PUSH    EDX EAX
         Mov     EDX,EAX
         Call    CCS_Return_Num
         Jc      @CCS_ANSI_GR_Rend_Err
         Call    CCS_ANSI_Set_Color
        POP     EAX  EDX
        Inc     EAX
        Sub     EDX, EAX
        Add     EDI, EAX
        Mov     Byte ptr ES:[EDI],'['
        Jmp     @CCS_ANSI_Set_Graphics_Rendition_Main
@CCS_ANSI_GR_Rend_OK1 :
        Call    CCS_Return_Num
        Jc      @CCS_ANSI_GR_Rend_Err2
        Call    CCS_ANSI_Set_Color
        Jmp     @CCS_ANSI_GR_Rend_Err2
@CCS_ANSI_GR_Rend_Err  :
        POP    EAX EDX
@CCS_ANSI_GR_Rend_Err2 :
        RET
CCS_ANSI_Set_Graphics_Rendition ENDP
;####################################################################################
;# Nastavi barvu textu
;####################################################################################
CCS_ANSI_Set_Color PROC
         Or    AL,AL
         Jnz   @CCS_ANSI_Set_Color_No_Reset
         Mov   Al, Byte ptr ES:[Reset_Col]
         Mov   Byte ptr DS:[ESI.Console_Color_Atr],Al
         Jmp   @CCS_ANSI_Set_Color_Error
@CCS_ANSI_Set_Color_No_Reset:
         Cmp   Al,2
         Jne   @CCS_ANSI_Set_Color_No_Dim
         Mov   Al, Byte ptr ES:[Dim_Col]
         And   Byte ptr DS:[ESI.Console_Color_Atr],Al
         Jmp   @CCS_ANSI_Set_Color_Error
@CCS_ANSI_Set_Color_No_Dim :
         Cmp   Al,5
         Jne   @CCS_ANSI_Set_Color_No_Blink
         Mov   Al, Byte ptr ES:[Blink_Col]
         Or    Byte ptr DS:[ESI.Console_Color_Atr],Al
         Jmp   @CCS_ANSI_Set_Color_Error
@CCS_ANSI_Set_Color_No_Blink:
         Sub   Al,30
         Js    @CCS_ANSI_Set_Color_Error
         Cmp   Al,7
         Ja    @CCS_ANSI_Set_Color_No_Foreground
         And   EAX,0FFh
         Mov   Al, Byte ptr ES:[Colors_Redirection_30+EAX]
         Or    Byte ptr DS:[ESI.Console_Color_Atr],Al
         Jmp   @CCS_ANSI_Set_Color_Error
@CCS_ANSI_Set_Color_No_Foreground :
         Sub   Al,10
         Js    @CCS_ANSI_Set_Color_Error
         Cmp   Al,7
         Ja    @CCS_ANSI_Set_Color_Error
         And   EAX,0FFh
         Mov   Al, Byte ptr ES:[Colors_Redirection_40+EAX]
         Or    Byte ptr DS:[ESI.Console_Color_Atr],Al
         Jmp   @CCS_ANSI_Set_Color_Error
@CCS_ANSI_Set_Color_Error:
        RET
CCS_ANSI_Set_Color ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Inserts_blank_lines PROC
        RET
CCS_ANSI_Inserts_blank_lines ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Deletes_lines PROC
        RET
CCS_ANSI_Deletes_lines ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Inserts_blank_chars PROC
        RET
CCS_ANSI_Inserts_blank_chars ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Scroll_Up PROC
        RET
CCS_ANSI_Scroll_Up ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Scroll_Down PROC
        RET
CCS_ANSI_Scroll_Down ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Output_char_translate PROC
        RET
CCS_ANSI_Output_char_translate ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Keyboard_Key_Reassignment PROC
        RET
CCS_ANSI_Keyboard_Key_Reassignment ENDP
;###################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Set_mode PROC
        RET
CCS_ANSI_Set_mode ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Reset_Mode PROC
        RET
CCS_ANSI_Reset_Mode ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Cursor_position_Report PROC
        RET
CCS_ANSI_Cursor_position_Report ENDP
;####################################################################################
;# not implement yet
;####################################################################################
CCS_ANSI_Device_status_report PROC
        RET
CCS_ANSI_Device_status_report ENDP




;####################################################################################
;# Procedura navraci cislo, ktere je ulozeno v retezci
;####################################################################################
;# Vstup  : ES:[EDI] - zacatek parametru sekvence
;#          EDX - kolik znaku ma mit cislo
;# Vystup : If C then Error Else EAX - cislo
;####################################################################################
CCS_Return_Num PROC
       PUSH ECX EDX EBP
         STC
         Or    EDX,EDX
         Jz    @CCS_Return_NumCount_End
         Mov   ECX, EDX
         Xor   EBP, EBP
         Mov   EAX, 1
  @CCS_Return_NumCount :
         PUSH  EAX
          Movzx EDX, Byte ptr ES:[EDI+ECX]
          Sub   Dl,'0'
          Js    @CCS_Return_NumCount_Error
          Cmp   Dl,9
          Ja    @CCS_Return_NumCount_Error
          Mul   EDX
          Add   EBP, EAX
         POP   EAX
          Mov   EDX,10
          Mul   EDX
          Dec   ECX
          Jnz   @CCS_Return_NumCount
          Mov   EAX, EBP
          CLC
  @CCS_Return_NumCount_End :
          POP  EBP EDX ECX
        RET
  @CCS_Return_NumCount_Error :
         POP    EAX
         Xor    EAX,EAX
         STC
         Jmp    Short @CCS_Return_NumCount_End
CCS_Return_Num ENDP
;####################################################################################
;# Procedura navraci Offset v retezci, kde nalezl znak
;####################################################################################
;# Vstup  : ES:[EDI] - zacatek parametru sekvence
;#          EDX      - delka retezce
;#          Al       - Hledany znak
;# Vystup : If C then Error Else EAX - cislo
;####################################################################################
CCS_Return_Char_Pos PROC
    PUSH   EBP EDX EDI
     OR    EDX,EDX
     Jz    Short CCS_Return_Char_Pos_Loop_Err
     XOR   EBP,EBP
  CCS_Return_Char_Pos_Loop :
     Cmp   Byte ptr ES:[EDI],Al
     Je    CCS_Return_Char_Pos_Loop_End
     Inc   EDI
     Inc   EBP
     Dec   EDX
     Jnz   CCS_Return_Char_Pos_Loop
  CCS_Return_Char_Pos_Loop_Err :
     STC
    POP    EDI EDX EBP
       RET
  CCS_Return_Char_Pos_Loop_End :
     CLC
     Mov   EAX, EBP
     Dec   EAX
    POP    EDI EDX EBP
       RET
CCS_Return_Char_Pos ENDP
;############################################################################
;# Kontroluje retezec, je-li to validni OCT cislo, a ano-li, pak navrati
;# hodnotu cisla
;############################################################################
Console_Driver_CmpStr_Oct PROC
          PUSH EDI ECX EAX EBP
        MOV      EDI,EAX
        Inc      EDI
        Add      EDI,Offset Consoles_Control_Seguence
        Mov      ECX,3
  @Console_Driver_CmpStr_OctCheck :
        Mov      Dl, Byte ptr ES:[EDI]
        Sub      Dl,'0'
        Js       @CCS_Return_NumCount_Error_OCT
        Cmp      Dl,7
        Ja       @CCS_Return_NumCount_Error_OCT
        Inc      EDI
        Dec      ECX
        Jnz      @Console_Driver_CmpStr_OctCheck
        Dec      EDI
        Mov      ECX, 3
        Xor      EBP, EBP
        Mov      EAX, 1
  @CCS_Return_NumCount_OCT :
        PUSH  EAX
         Movzx   EDX, Byte ptr ES:[EDI]
         Sub     EDX, '0'
         Mul     EDX
         Add     EBP, EAX
        POP     EAX
         Mov     EDX,8
         Mul     EDX
         Dec     EDI
         Dec     ECX
         Jnz     @CCS_Return_NumCount_OCT
         Mov     EDX, EBP
         Cmp     EDX,0FFh
         Ja      @CCS_Return_NumCount_Error_OCT
          CLC
  @CCS_Return_NumCount_End_OCT :
          POP  EBP EAX ECX EDI
        RET
  @CCS_Return_NumCount_Error_OCT :
          Xor    EAX,EAX
          STC
         Jmp    Short @CCS_Return_NumCount_End_OCT
Console_Driver_CmpStr_Oct ENDP


