;############################################################################
;# Prace s LDT descriptory
;############################################################################

;############################################################################
;# Procedura vytvari descriptor v dane tabulce, Datovy Descriptor,
;# G=1,D/B=1, CPL3
;############################################################################
;# Vstup  : ECX  - Size ve strankach
;#          EAX  - Virtualni Adresa
;#          FS   - Datovy alias na LDT
;############################################################################
;# Nici :   EDX
;############################################################################
TSKMM_CreateDescriptor PROC
         PUSH  ECX EBX
          Dec   ECX
         PUSH  ECX
          Xor   EDX,EDX
          Mov   ECX,8192
@MM_LookForDescriptor :
          Mov   EBX,Dword ptr FS:[EDX]
          Add   EBX,Dword ptr FS:[EDX+4]
          Jz    Short @MM_FoundDescriptor
          Add   EDX,8
          Dec   ECX
          Jnz   Short @MM_LookForDescriptor
          STC
         POP   ECX
         POP   EBX ECX DS
         RET
@MM_FoundDescriptor :
          POP   ECX
          Mov   Word ptr  FS:[EDX+2],AX
          ROR   EAX,16
          Mov   Byte ptr  FS:[EDX+4],AL
          Mov   Byte ptr  FS:[EDX+7],AH
          ROR   EAX,16
          Mov   Word  ptr FS:[EDX+0],CX
          Shr   ECX,16
          And   Ch,0Fh
          Or    CH,Data_Atr2
          Mov   Byte ptr FS:[EDX+5], Data_Atr1_CPL3
          Mov   Byte ptr FS:[EDX+6], CH
          CLC
          Or    Dl,7
         POP   EBX ECX
         RET
TSKMM_CreateDescriptor ENDP
;############################################################################
;# Procedura vytvari descriptor v dane tabulce, Datovy Descriptor,
;# G=1,D/B=1, CPL3
;############################################################################
;# Vstup  : ECX  - Size ve strankach
;#          EAX  - Virtualni Adresa
;############################################################################
;# Nici :   EDX
;############################################################################
TSKMM_CreateDescriptor_LDT PROC
    PUSH  ECX EBX ES FS
     SLDT  DX
     Call  Memory_Manager_Create_Alias
     Jc    @TSKMM_CreateDescriptor_LDT_LDT_Error
     Mov   FS,EDX
     Call  TSKMM_CreateDescriptor
     Mov   BX,4h
     Mov   ES,BX
     Xor   EBX,EBX
     Mov   ECX,(8192-1)
@TSKMM_CreateDescriptor_LDT_FindFree :
     Add   EBX, TMA_Size
     Cmp   Dword ptr ES:[EBX.TMA_Block_Size],0
     Je    Short @TSKMM_CreateDescriptor_LDT_FindFree_Found
     Dec   ECX
     Jnz   Short @TSKMM_CreateDescriptor_LDT_FindFree
     Jmp   Short @TSKMM_CreateDescriptor_LDT_FindFree_End
@TSKMM_CreateDescriptor_LDT_FindFree_Found :
     Mov   Word ptr ES:[EBX.TMA_Selector],DX
@TSKMM_CreateDescriptor_LDT_FindFree_End :
     Mov   EBX,EDX
     Mov   EDX,FS
     POP   FS
     PUSHFD
      Call  Memory_Manager_Dispose_Descriptor
     POPFD
     Mov   EDX, EBX
     POP   ES EBX ECX
     Ret
@TSKMM_CreateDescriptor_LDT_LDT_Error :
     POP  FS ES EBX ECX
     Ret
TSKMM_CreateDescriptor_LDT ENDP
;############################################################################
;# Procedura meni datovy descriptor na codovy v LDT
;############################################################################
;#  Vstup  : EDX  - Selector na descriptor pro prevod
;############################################################################
TSKMM_Memory_Manager_Data2Code_Descr_LDT PROC
    PUSH  EBX FS
     Mov   EBX,EDX
     SLDT  DX
     Call  Memory_Manager_Create_Alias
     Jc    @Memory_Manager_Data2Code_Descr_LDT_LDT_Error
     Mov   FS,EDX
     Mov   EDX,EBX
     Call  Memory_Manager_Data2Code_Descr
     Mov   EDX,EBX
     Mov   EDX,FS
     POP   FS
     PUSHFD
      Call  Memory_Manager_Dispose_Descriptor
     POPFD
     Mov  EDX,EBX
     POP  EBX
     Ret
@Memory_Manager_Data2Code_Descr_LDT_LDT_Error :
     POP  FS EBX
     Ret
TSKMM_Memory_Manager_Data2Code_Descr_LDT ENDP
;############################################################################
;# Procedura meni DPL descriptoru v LDT
;############################################################################
;# Vstup  : EDX  - Selector descriptoru ke zmene
;#          EAX - novy DPL
;# Vysup    EDX - novy selector (zmenene RPL)
;############################################################################
;# Nici registery : zadne
;############################################################################
TSKMM_Memory_Manager_ChangeDPL_Descr_LDT PROC
    PUSH  EBX FS
     Mov   EBX,EDX
     SLDT  DX
     Call  Memory_Manager_Create_Alias
     Jc    @Memory_Manager_ChangeDPL_Descr_LDT_LDT_Error
     Mov   FS,EDX
     Mov   EDX,EBX
     Call  Memory_Manager_ChangeDPL_Descr
     Mov   EBX,EDX
     Mov   EDX,FS
     POP   FS
     PUSHFD
      Call  Memory_Manager_Dispose_Descriptor
     POPFD
     Mov   EDX,EBX
     POP   EBX
     RET
@Memory_Manager_ChangeDPL_Descr_LDT_LDT_Error :
     POP  FS EBX
     RET
TSKMM_Memory_Manager_ChangeDPL_Descr_LDT ENDP
;############################################################################
;# Procedura vytvari z datoveho segmentu stackovy segment
;# nebo, je-li to codovy segment, nastavuje C bit do jednicky...
;############################################################################
;# Vstup  : EDX - Selector descriptoru ke zmene
;############################################################################
;# Nici registery : zadne
;############################################################################
TSKMM_Memory_Manager_Data2Stack_Descr_LDT PROC
    PUSH  EBX FS
     Mov   EBX,EDX
     SLDT  DX
     Call  Memory_Manager_Create_Alias
     Jc    @Memory_Manager_Data2Stack_Descr_LDT_LDT_Error
     Mov   FS,EDX
     Mov   EDX,EBX
     Call  Memory_Manager_Data2Stack_Descr
     Mov   EDX,EBX
     Mov   EDX,FS
     POP   FS
     PUSHFD
      Call  Memory_Manager_Dispose_Descriptor
     POPFD
     Mov  EDX,EBX
     POP  EBX
     Ret
@Memory_Manager_Data2Stack_Descr_LDT_LDT_Error :
     POP  FS EBX
     Ret
TSKMM_Memory_Manager_Data2Stack_Descr_LDT ENDP
