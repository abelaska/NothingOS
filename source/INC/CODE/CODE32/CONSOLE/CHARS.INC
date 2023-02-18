;#####################################################################################
;# zahajuje ESC kontrolni sekvenci
;#####################################################################################
Console_ESCAPE_Character_Procedure PROC
    Mov    Byte ptr ES:[Consoles_Control_Escape+EBX],1
   RetF
Console_ESCAPE_Character_Procedure ENDP
;#####################################################################################
;# Vrati se o jeden znak zpatky
;#####################################################################################
Console_Special_Backward PROC
       PUSHAD
        Mov   EAX, EBX
         Call  Get_Console_Addr
         Call  Console_GetXY
         Dec   EAX
         Jnz   @Console_Driver_WRITE_BackStep_OK1
         Movzx EAX,word ptr DS:[ESI.Console_Virt_Res_X]
         Dec   EBX
         Jnz   @Console_Driver_WRITE_BackStep_OK1
         Inc   EBX
         Jmp   @Console_Driver_WRITE_BackStep_Skip1
@Console_Driver_WRITE_BackStep_OK1:
         Call   Console_GotoXY
         Movzx  EAX,word ptr DS:[ESI.Console_Cur_Y]
         Dec    EAX
         Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
         Mul    ECX
         Movzx  ECX,word ptr DS:[ESI.Console_Cur_X]
         Dec    ECX
         Add    EAX,ECX
         Mov    EDI,EAX
         Movzx  EDX,Word ptr DS:[ESI.Console_Scr_Buf]
         Or     EDX,EDX
         Jz     @Console_Driver_WRITE_BackStep_Skip1
         Mov    DS, EDX
         Shl    EDI,1
         Mov    AX,0720h
         Mov    Word ptr DS:[EDI],AX
@Console_Driver_WRITE_BackStep_Skip1 :
       POPAD
       Call Console_Show_Extended
   RetF
Console_Special_Backward ENDP
;#####################################################################################
;# Horizontalni tabulator
;#####################################################################################
Console_Special_Tabulator_Horiz PROC
       PUSHAD
        Mov    EAX, EBX
        Call   Get_Console_Addr
        Call   Console_GetXY
        Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_X]
        Add    EAX,dword ptr ES:[Horizontal_Tab]
        Cmp    EAX,ECX
        Jna    @Console_Driver_WRITE_Tabulator_OK1
        Mov    EAX,ECX
   @Console_Driver_WRITE_Tabulator_OK1 :
        Call  Console_GotoXY
   @Console_Driver_WRITE_Tabulator_Skip1 :
       POPAD
       Call Console_Show_Cursor_Extended
   RetF
Console_Special_Tabulator_Horiz ENDP
;#####################################################################################
;# Vertikalni tabulator
;#####################################################################################
Console_Special_Tabulator_Vert PROC
       PUSHAD
        Mov    EAX, EBX
        Call   Get_Console_Addr
        Call   Console_GetXY
        Movzx  ECX,word ptr DS:[ESI.Console_Virt_Res_Y]
        Add    EBX,dword ptr ES:[Vertical_Tab]
        Cmp    EBX,ECX
        Jna    @Console_Driver_WRITE_VTabulator_OK1
        Mov    EBX,ECX
   @Console_Driver_WRITE_VTabulator_OK1 :
        Call  Console_GotoXY
       POPAD
       Call Console_Show_Cursor_Extended
   RetF
Console_Special_Tabulator_Vert ENDP
;#####################################################################################
;# Odradkovani
;#####################################################################################
Console_Special_New_Line PROC
       PUSHAD
        Mov   EAX, EBX
        Call  Get_Console_Addr
        Call  Console_GetXY
        Inc   EBX
        Mov   EAX, 1
        Call  Console_GotoXY
       POPAD
       Call Console_Show_Cursor_Extended
   RetF
Console_Special_New_Line ENDP
;#####################################################################################
;# Vstup : EBX - cislo konzoly
;#         ES  - Data descriptor
;#####################################################################################
Console_Show_Cursor_Extended PROC
      PUSH   DS ESI EAX
      Mov    EAX, EBX
      Call   Get_Console_Addr
      Cmp   EBX,Dword ptr ES:[Active_Console]
      Jne   @Console_Show_Cursor_Extended_Skip
      Mov   AX, Word ptr DS:[ESI.Console_Cur_Y]
      Mov   BX, Word ptr DS:[ESI.Console_View_Y]
      Add   BX, Word ptr DS:[ESI.Console_Res_Y]
      Cmp   BX,AX
      Jae   @Console_Show_Cursor_Extended_Skip1
      Sub   AX, Word ptr DS:[ESI.Console_Res_Y]
      Mov   Word ptr DS:[ESI.Console_View_Y],Ax
      Call  Console_Redraw
   @Console_Show_Cursor_Extended_Skip1 :
      Call  Console_Show_Cursor
   @Console_Show_Cursor_Extended_Skip :
      POP   EAX ESI DS
      Ret
Console_Show_Cursor_Extended ENDP
;#####################################################################################
;# Vstup : EBX - cislo konzoly
;#         ES  - Data descriptor
;#####################################################################################
Console_Show_Extended PROC
      PUSH   DS ESI EAX
      Mov    EAX, EBX
      Call   Get_Console_Addr
      Cmp   EBX,Dword ptr ES:[Active_Console]
      Jne   Short @Console_Show_Extended_Skip
      Call  Console_Redraw
   @Console_Show_Extended_Skip :
      POP   EAX ESI DS
      Ret
Console_Show_Extended ENDP