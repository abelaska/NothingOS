;########################################################################################
;# Jednotka obsahujici procedury s obecnou praci s pages :)
;########################################################################################

;########################################################################################
;# Procedura nastavuje fyzicke page descriptoru na NotUsed
;########################################################################################
;# Vstup : EDX = Selektor descriptoru
;########################################################################################
PAGE_SetPageAS_NotUsed PROC
       PUSH  DS FS EDI ECX EAX EDX
        Mov   ECX,EDX
        LSL   ECX,ECX
        Inc   ECX
        Shr   ECX,12     ;ECX = pocet page ktery dany descriptor obsahuje :)
        Call  MemMan_ReturnDescriptorBaseAddress
        Mov   EDX,EAX    ;EDX = bazova adresa descriptoru
        ;Datovy alias na adresar page
        PUSH  EDX
         Create_Temp_Descriptor CR3
         Mov   FS, DX
         Invlpg Byte ptr FS:[0]
         Create_Temp_Descriptor EAX
         Mov   DS, DX
        POP   EDX
@PAGE_SetPageAS_NotUsed_Loop :
        PUSH   EDX
         Mov   EDI,EDX
         Shr   EDX,22
         Shl   EDX,2
         Mov   EAX,Dword ptr FS:[EDX]
         Test  EAX,1
         Jz    @PAGE_SetPageAS_NotUsed_Skip
@PAGE_SetPageAS_NotUsed_Skip_Create :
         And   AX,0F000h
         Remap_Temp_Descriptor DS,EAX
         Invlpg Byte ptr DS:[0]
         Shr   EDI,12
         Shl   EDI,2
         And   EDI,0FFFh
         Mov   EAX,Dword ptr DS:[EDI]
         Shr   EAX,12 ;cislo ente page :)
         Call  FPIT_PAGE_Set_FysPage_NotUsed
@PAGE_SetPageAS_NotUsed_Skip :
        POP    EDX
        Add    EDX,4096
        Dec    ECX
        Jnz    @PAGE_SetPageAS_NotUsed_Loop
        Dispose_Temp_Descriptor FS
        Dispose_Temp_Descriptor DS
       POP   EDX EAX ECX EDI FS DS
        RET
PAGE_SetPageAS_NotUsed ENDP
;########################################################################################
;# Procedura navraci adresu prvni volne page, nastavuje do 1 v FysLin_Mem_Descriptor
;# decrementuje  Free_FysPages_Count a incrementuje Pages_Used
;########################################################################################
;# Vystup  : If C then Error else
;#           EAX - cislo 4k page v poradi
;########################################################################################
PAGE_Get_Free_Page PROC
        PUSH  DS ES ECX EDI EBX EDX
         Mov   ECX,Data_Descriptor
         Mov   DS,CX
         Mov   ECX,FysLin_Mem_Descriptor
         Mov   ES,CX
         LSL   ECX,ECX
         Xor   EDI,EDI
         Mov   AL,0FFh
         Repz  Scasb
         Jz    Short @PAGE_Get_Free_Page_Error
         Dec   EDI
         Mov   DL,Byte ptr ES:[EDI]
         Xor   EAX,EAX
@PAGE_Get_Free_Page_Loop :
         Inc   Al
         RCL   DL,1
         Jc    @PAGE_Get_Free_Page_Loop
         Xor   EBX,EBX
         Mov   Bl,AL
         Dec   BL
         Mov   Bl,Byte ptr DS:[SetBit_Table2+EBX]
         Or    Byte ptr ES:[EDI],Bl
         Shl   EDI,3
         Add   EAX,EDI
         Dec   EAX
         CLC
         Jmp   Short @PAGE_Get_Free_Page_OK
@PAGE_Get_Free_Page_Error :
         STC
         Xor   EAX,EAX
         Jmp   Short @PAGE_Get_Free_Page_End
@PAGE_Get_Free_Page_OK :
         Dec   Dword ptr DS:[Free_FysPages_Count]
         Inc   Dword ptr DS:[Pages_Used]
@PAGE_Get_Free_Page_End :
       POP EDX EBX EDI ECX ES DS
        RET
PAGE_Get_Free_Page ENDP
;########################################################################################
;# Procedura nastavuje AVL page daneho descriptoru na hodnotu v BL
;########################################################################################
;# Vstup : EDX = selektor na descriptor (v GDT)
;#         EBX = Typ
;########################################################################################
PAGE_SetPagesAVL_Type PROC
       PUSH  DS FS EDI ECX EAX EDX
        Mov   ECX,EDX
        LSL   ECX,ECX
        Inc   ECX
        Shr   ECX,12     ;ECX = pocet page ktery dany descriptor obsahuje :)
        Call  MemMan_ReturnDescriptorBaseAddress
        Mov   EDX,EAX    ;EDX = bazova adresa descriptoru
        ;Datovy alias na adresar page
        PUSH  EDX
         Create_Temp_Descriptor CR3
         Mov   FS, EDX
         Invlpg Byte ptr FS:[0]
         Create_Temp_Descriptor EAX
         Mov   DS, EDX
        POP   EDX
@PAGE_SetPagesAVL_Type_Loop :
        PUSH   EDX
         Mov   EDI,EDX
         Shr   EDX,22
         Shl   EDX,2
         Mov   EAX,Dword ptr FS:[EDX]
         Test  EAX,1
         Jnz    @PAGE_SetPagesAVL_Type_Skip_Create
         PUSH   EBX EDX
          Mov   EBX,EDX
          Mov   EDX,EAX
          Call  PAGE_Create_New_Subdirectory_FysLin
         POP    EDX EBX
@PAGE_SetPagesAVL_Type_Skip_Create :
         And   AX,0F000h
         Remap_Temp_Descriptor DS, EAX
         Shr   EDI,12
         Shl   EDI,2
         And   EDI,0FFFh
         Mov   EAX,Dword ptr DS:[EDI]
         Ror   EAX,9
         And   Al,0F8h
         Or    Al,Bl
         Rol   EAX,9
         Mov   Dword ptr DS:[EDI],EAX
@PAGE_SetPagesAVL_Type_Skip :
        POP    EDX
        Add    EDX,4096
        Dec    ECX
        Jnz    @PAGE_SetPagesAVL_Type_Loop
        Dispose_Temp_Descriptor DS
        Dispose_Temp_Descriptor FS
       POP   EDX EAX ECX EDI FS DS
        RET
PAGE_SetPagesAVL_Type ENDP
;########################################################################################
;# Procedura se pokusi vytvorit novy podadresar pro popis fyzicke pameti
;########################################################################################
;# Vstup  : FS - Adresar pages
;#          FS:EBX - pozice, kde chceme vytvorit podadresar
;#          EDX - Obraz page
;# Vystup : None
;########################################################################################
PAGE_Create_New_Subdirectory_FysLin PROC
       PUSHAD
       PUSH  ES FS GS
        Call   PAGE_Get_Free_Page
        Jc    MM_Page_Fault_Create_New_Subdirectory_FysLin_Error
        Shl   EAX,12
        Ror   EDX,9
        And   DL,0F8h
        Or    DL,_PAGE_NonSwapable
        Rol   EDX,9
        And   EDX,000FFFh
        Or    Dl,1
        Or    EAX,EDX
        Mov   Dword ptr FS:[EBX],EAX
        Invlpg Byte ptr FS:[EBX]
        Cmp   EBX,0C00h
        Jae   @MM_Page_Fault_Create_New_SubdirectorySkip
        PUSH GS DS ES
        PUSHAD
         Mov   EDX,Data_Descriptor
         Mov   ES,EDX
         Cmp   EBX,0400h
         Jnb   @MM_Page_Fault_Create_New_SubdirectorySkip2
         Add   Dword ptr ES:[Free_FysPages_Count],1024
      @MM_Page_Fault_Create_New_SubdirectorySkip2 :
         Cmp   Byte ptr ES:[TaskMan_CR3_Semafor],0
         Je    @PAGE_Create_New_Subdirectory_Create_CR3Loop_end
         Mov   DS,Word ptr ES:[TaskMan_CR3s_Descriptor]
         Xor   EDX,EDX
         Mov   ECX,Max_Task
   @PAGE_Create_New_Subdirectory_Create_CR3Loop :
         Cmp    Dword ptr DS:[EDX],0
         Jne    @PAGE_Create_New_Subdirectory_Create_CR3Loop_found
   @PAGE_Create_New_Subdirectory_Create_CR3Loop_Ret :
         Add    EDX,4
         Dec    ECX
         Jnz    @PAGE_Create_New_Subdirectory_Create_CR3Loop
         Jmp    @PAGE_Create_New_Subdirectory_Create_CR3Loop_end
   @PAGE_Create_New_Subdirectory_Create_CR3Loop_found :
         Mov    ESI,Dword ptr DS:[EDX]
         PUSH   EDX
          Create_Temp_Descriptor ESI
          Mov    GS,EDX
         POP    EDX
         Mov     Dword ptr GS:[EBX],EAX
         Invlpg  Byte ptr GS:[EBX]
         Dispose_Temp_Descriptor GS
         Jmp    @PAGE_Create_New_Subdirectory_Create_CR3Loop_Ret
    @PAGE_Create_New_Subdirectory_Create_CR3Loop_end :
        POPAD
        POP  ES DS GS
@MM_Page_Fault_Create_New_SubdirectorySkip :
        Mov   EBX,EAX
       PUSH EBX
        Shr   EBX,22
        Shl   EBX,2
        Mov   EDX,Dword ptr FS:[EBX]
        And   DX,0F000h
        Create_Temp_Descriptor EDX
        Mov   ES, EDX
       POP  EBX
        Shr   EBX,12
        Shl   EBX,2
        And   EBX,0FFFh
        Mov   Dword ptr ES:[EBX],EAX
        Invlpg Byte ptr ES:[EBX]
        And   AX,0F000h
        Remap_Temp_Descriptor ES,EAX
        Mov   ES,EDX
        Mov   ECX,ES
        LSL   ECX,ECX
        Inc   ECX
        Shr   ECX,2
        Mov   EAX,PAGE_CONST_NP_NORMAL_MEMORY
        Xor   EDI,EDI
        Rep   Stosd
       Dispose_Temp_Descriptor ES
       POP   GS FS ES
       POPAD
        CLC
        RET
MM_Page_Fault_Create_New_Subdirectory_FysLin_Error :
       Dispose_Temp_Descriptor ES
       POP   GS FS ES
       POPAD
        STC
        RET
PAGE_Create_New_Subdirectory_FysLin ENDP
;########################################################################################
;# Procedura se pokusi najit a vytvorit Subdirectory
;########################################################################################
;# Vstup  : None
;# Vystup : C - Error
;########################################################################################
PAGE_Find_Free_Subdirectory_FysLin PROC
       PUSH  FS DS
       PUSHAD
        Mov   BX,Data_Descriptor
        Mov   DS,BX
        Create_Temp_Descriptor CR3
        Mov   FS, DX
        Mov   ECX,Dword ptr DS:[RAM_Size_MB]
        Shr   ECX,2 ;EDX = pocet podadresaru, ktere zabira fyslin
        Xor   EBX,EBX
@MM_Page_Find_Loop :
        Mov   EAX,Dword ptr FS:[EBX]
        Test  Al,1
        Jz    @MM_Page_Find_Found
        Add   EBX,4
        Dec   ECX
        Jnz   @MM_Page_Find_Loop
        Jmp   @MM_Page_Find_Error
@MM_Page_Find_Found :
        Mov   EDX,EAX
        Call  PAGE_Create_New_Subdirectory_FysLin
        Jc    @MM_Page_Find_Error
       Dispose_Temp_Descriptor FS
       POPAD
       POP DS  FS
       CLC
        RET
@MM_Page_Find_Error :
       Dispose_Temp_Descriptor FS
       POPAD
       POP DS FS
       STC
        RET
PAGE_Find_Free_Subdirectory_FysLin ENDP
;########################################################################################
;# Procedura nastavuje AVL page daneho descriptoru na hodnotu v BL
;# predpoklada se fyslin descriptor
;########################################################################################
;# Vstup : EDX = selektor na descriptor (v GDT)
;#         EBX = Typ
;########################################################################################
PAGE_SetPages_Fys_And_AVL PROC
       PUSH  DS FS EDI ECX EAX EDX EBX
        Shl   EBX,9
        Or    EBX,PagesMainFlags
        Mov   ECX,EDX
        LSL   ECX,ECX
        Inc   ECX
        Shr   ECX,12     ;ECX = pocet page ktery dany descriptor obsahuje :)
        Call  MemMan_ReturnDescriptorBaseAddress
        Mov   EDX,EAX    ;EDX = bazova adresa descriptoru
        ;Datovy alias na adresar page
        PUSH EDX
         Create_Temp_Descriptor CR3
         Mov   FS, DX
         Create_Temp_Descriptor EAX
         Mov   DS, DX
        POP  EDX
@PAGE_SetPagesFys_AVL_Type_Loop :
        PUSH   EDX
         Mov   EDI,EDX
         Shr   EDX,22
         Shl   EDX,2
         Mov   EAX,Dword ptr FS:[EDX]
         Test  EAX,1
         Jnz    @PAGE_SetPagesFys_AVL_Type_Skip_Create
         PUSH   EBX EDX
          Mov   EBX,EDX
          Mov   EDX,EAX
          Call  PAGE_Create_New_Subdirectory_FysLin
         POP    EDX EBX
@PAGE_SetPagesFys_AVL_Type_Skip_Create :
         And   AX,0F000h
         Remap_Temp_Descriptor DS,EAX
         MOV   EAX,EDI
         Shr   EDI,12
         Shl   EDI,2
         And   EDI,0FFFh
         AND   AX,0F000h
         OR    EAX,EBX
         Mov   Dword ptr DS:[EDI],EAX
@PAGE_SetPagesFys_AVL_Type_Skip :
        POP    EDX
        Add    EDX,4096
        Dec    ECX
        Jnz    @PAGE_SetPagesFys_AVL_Type_Loop
        Dispose_Temp_Descriptor DS
        Dispose_Temp_Descriptor FS
       POP   EBX EDX EAX ECX EDI FS DS
        RET
PAGE_SetPages_Fys_And_AVL ENDP
;################################################################################
;# Procedura nuluje fyzickou page z FysLin table
;################################################################################
;# Vstup  : EBX - fyslin page
;# Vystup : None
;################################################################################
PAGE_Set_Page_As_Free PROC
        PUSH  DS ES EDX EBX
         Mov   EDX,Data_Descriptor
         Mov   DS,EDX
         Mov   EDX,FysLin_Mem_Descriptor
         Mov   ES,EDX
         Shr   EBX,12
         Mov   EDX,EBX
         And   EDX,7
         Shr   EBX,3
         Mov   Al, Byte ptr ES:[EBX]
         And   Al, Byte ptr DS:[EDX+SetBit_Table2_Non]
         Mov   Byte ptr ES:[EBX],AL
       POP EBX EDX ES DS
        RET
PAGE_Set_Page_As_Free ENDP
