;############################################################################
;# Procedura vytvori zakladni descriptor v LDTcku tasku, totiz Mem_Layout
;# Od teto chvile muzeme kdykoliv alokovat pamet do LDTcka pomoci TSKMM_funkci
;############################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;############################################################################
;# v CR3 by mela byt pageovaci struktura pro dany task
;############################################################################
TSKMM_Inicialize_LDT PROC
        PUSHAD
        PUSH    ES FS
          Xor   EDX,EDX
          SLDT  DX
          Call  Memory_Manager_Create_Alias
          Jc    @TSKMM_Inicialize_LDT_Error
          Mov   FS,EDX
          Invlpg Byte ptr FS:[0]
          Mov   EAX,VMem_Private_Area
          Mov   ECX,(((TMA_Size*8192) shr 12)+1)
          Call  TSKMM_CreateDescriptor
          Xor   Al,Al
          Call  Memory_Manager_ChangeDPL_Descr

          ;# EDX = selektor na Task_Mem_Alloc_Structures
          ;# Nastavime prvni zaznam
          Mov   ES,DX
          Mov   Dword ptr ES:[TMA_Block_Start],VMem_Private_Area
          Mov   DWord ptr ES:[TMA_Block_Size],ECX
          Mov   Word ptr  ES:[TMA_Selector],DX
          Shl   ECX,12
          Mov   EDX,VMem_Private_Area_Size
          Sub   EDX,ECX
          Add   ECX,VMem_Private_Area
          Dec   EDX
          Shr   EDX,12
          Inc   EDX
          Mov   Dword ptr ES:[(TMA_Size*1)+TMA_Block_Start],ECX
          Mov   DWord ptr ES:[(TMA_Size*1)+TMA_Block_Size],EDX
          Mov   Word ptr  ES:[(TMA_Size*1)+TMA_Selector],0
        Mov  EDX,FS
        POP  FS ES
        Call  Memory_Manager_Dispose_Descriptor
        POPAD
         CLC
         RET
@TSKMM_Inicialize_LDT_Error :
        POP  FS ES
        POPAD
         STC
         RET
TSKMM_Inicialize_LDT ENDP