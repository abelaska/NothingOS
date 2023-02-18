;###########################################################################
;# Rutiny pro obsluhu Page Faultu
;###########################################################################

;############################################################################
;# Hlavni procedura pro swapovani page
;############################################################################
;# Vstup:  CR2 - adresa page
;############################################################################
MM_Page_Fault Proc
MM_Page_Fault_Enter_Point :
        PUSHFD
        PUSHAD
        PUSH   ES FS DS GS
        CLI
        Mov     BX,Data_Descriptor
        Mov     DS,BX
        ;Datovy alias na adresar page
        Create_Temp_Descriptor CR3
        Jnc     Short @MM_Page_Fault_Continue1
        ;neni volny descriptor, musime, bohuzel pouzit Last_Chance_Descriptor
        Mov     EDX, Last_Chance_Descriptor
        Mov     EBX, CR3
        Remap_Temp_Descriptor EDX,EBX
@MM_Page_Fault_Continue1 :
        Mov     FS, EDX
        Invlpg Byte ptr FS:[0]
        Mov   EBX,CR2 ;Page, na ktere nastala chyba
       PUSH EBX
        Shr   EBX,22
        Shl   EBX,2
        Mov   EAX,Dword ptr FS:[EBX]
        Test  Al,2
        Jnz   @MM_Page_Fault_Test_UserPrivLevel
        And   Ah,0FEh
        And   Al,0Fh
        Or    Al,0Eh
        Mov   Dword ptr FS:[EBX],EAX
        Invlpg Dword ptr FS:[EBX]
       POP    EBX
        Jmp   MM_Page_Fault_End
@MM_Page_Fault_Test_UserPrivLevel :
        Test  Al,4
        Jnz   @MM_Page_Fault_Test_SubDirectory
        And   Ah,0FEh
        And   Al,0Fh
        Or    Al,0Eh
        Mov   Dword ptr FS:[EBX],EAX
        Invlpg Dword ptr FS:[EBX]
       POP   EBX
        Jmp   MM_Page_Fault_End
@MM_Page_Fault_Test_SubDirectory :
        Test  Al,1
        Jz    @MM_Page_Fault_SubDirectory_NP
@MM_Page_Fault_SubDirectory_P  :
        And   AX,0F000h
        Remap_Temp_Descriptor FS,EAX
        Invlpg Byte ptr FS:[0]
       POP   EBX
        Shr   EBX,12
        Shl   EBX,2
        And   EBX,0FFFh
        Mov   EDX,Dword ptr FS:[EBX]
        Test  Dl,2
        Jnz   Short @MM_Test_Priv_Level_2
        And   Dh,0FEh
        And   Dl,0Fh
        Or    Dl,0Eh
        Mov   Dword ptr FS:[EBX],EDX
        Invlpg Dword ptr FS:[EBX]
        Jmp   MM_Page_Fault_End
@MM_Test_Priv_Level_2 :
        Test  Dl,4
        Jnz   Short @MM_Page_Fault_Get_PageType
        And   Dh,0FEh
        And   Dl,0Fh
        Or    Dl,0Eh
        Mov   Dword ptr FS:[EBX],EDX
        Invlpg Dword ptr FS:[EBX]
        Jmp   MM_Page_Fault_End
@MM_Page_Fault_Get_PageType :
        Mov   EAX,EDX
        Shr   EAX,9
        And   EAX,07h
        Cmp   EAX,_PAGE_NotAssign
        Je    @MM_Page_Fault_Assign_Free_Page
        Cmp   EAX,_PAGE_NormalSwap
        Je    @MM_Page_Fault_Swaped_Page       ; dana page je swapla ve swap partitonu
        Cmp   EAX,_PAGE_ReadFromFile
        Je    @MM_Page_Fault_ReadFromFile      ; page jeste neni nactena ze souboru
        Cmp   EAX,_PAGE_Fileswap
        Je    @MM_Page_Fault_SwapFile          ; page je swapla ve swap partitionu, zaznam o swapu je ve virtual_pagFS_table
        Jmp   @MM_Page_Fault_Fatal_Error
;###########################################################################
;# Musime najit volny page a priradit ho
;###########################################################################
;# Vstup : EDX - obraz page
;#         FS:EBX - umisteni page
;###########################################################################
@MM_Page_Fault_Assign_Free_Page :
        Call  PAGE_Get_Free_Page     ;pokusime se najit nejaky volny page :)
        Jc    @MM_No_Free_Page
        PUSH  EBX
         Mov   EBX,CR2
         Call  FPIT_PAGE_Set_Page_Implicit ;nastavime implicitni hodnoty pro novou page
        POP    EBX
        Shl    EAX,12
        And    EDX,0001FFh
        Ror    EDX,9
        Or     EDX,_PAGE_NormalSwap
        Rol    EDX,9
        Or     Dl,1
        Or     EAX,EDX
        Mov    Dword ptr FS:[EBX],EAX
        Invlpg Byte ptr FS:[EBX]
        PUSH  ES EAX EBX
         Mov    EAX,CR2
         And    AX,0F000h
         Create_Temp_Descriptor EAX
         Mov   ES,EDX
         Mov   ECX,ES
         LSL   ECX,ECX
         Inc   ECX
         Shr   ECX,2
         Xor   EDI,EDI
         Xor   EAX,EAX
         Rep   Stosd
         Dispose_Temp_Descriptor ES
        POP  EBX EAX ES
         Mov    Dword ptr FS:[EBX],EAX
         Invlpg Byte ptr FS:[EBX]
        Jmp   MM_Page_Fault_End
;###########################################################################
;# Page je swapla, musime ji natahnout ze swap partitiony
;###########################################################################
;# Vstup : EDX - obraz page
;#         FS:EBX - umisteni page
;#         20b v EDX - nta 4k page ve swap partition
;###########################################################################
@MM_Page_Fault_Swaped_Page :
       PUSH EDX
        Call  PAGE_Get_Free_Page
        Jc    @MM_No_Free_Page
        PUSH  EBX
         Mov   EBX,CR2
         Call  FPIT_PAGE_Set_Page_Implicit ;nastavime implicitni hodnoty pro novou page
        POP   EBX
        Shl   EAX,12
        And   EDX,0001FFh
        Or    Dl,1
        Or    EAX,EDX
        Mov   Dword ptr FS:[EBX],EAX
        Invlpg Byte ptr FS:[EBX]
       POP    EDX
      PUSH ES EAX EBX
        Mov   EAX,CR2
        And   AX,0F000h
        Create_Temp_Descriptor EAX
        Mov    ES,EDX
        Invlpg Byte ptr ES:[0]
        STI
        Call  SwapDeamon_Restore_Page
        CLI
        Dispose_Temp_Descriptor ES
      POP  EBX EAX ES
        Mov   Dword ptr FS:[EBX],EAX
        Invlpg Byte ptr FS:[EBX]
        Jmp  MM_Page_Fault_End
;###########################################################################
;# Procedura nacte kus soboru do pameti
;###########################################################################
;# Vstup : EDX - obraz page
;#         FS:EBX - umisteni page
;###########################################################################
@MM_Page_Fault_ReadFromFile :
        Push EDX
         Call  PAGE_Get_Free_Page
         Jc    @MM_No_Free_Page
         Call   FPIT_PAGE_Set_Page_Implicit_File ;nastavime implicitni hodnoty pro novou page
         Shl    EAX,3
         Or     EAX,_PAGE_FileSwap
         Shl    EAX,9
         And    EDX,0001FFh
         Or     Dl,1
         Or     EAX,EDX
         Mov    Dword ptr FS:[EBX],EAX
         Invlpg Byte ptr FS:[EBX]
        POP   EDX
        PUSH   ES EAX EBX
        PUSH   EDX
         Mov    EAX,CR2
         And    AX,0F000h
        push   eax
         Create_Temp_Descriptor EAX
         Mov    ES,EDX
        pop    eax
        POP    EDX
        push    eax
         Mov    EAX,Data_descriptor
         Mov    DS,EAX
         Mov    DS,Dword ptr DS:[FMPI_Selector]
         Shr    EDX,12
         Mov    EAX,FMPI_Size
         Mul    EDX
         Mov    EDX,EAX
        pop     eax
         Mov    Byte ptr DS:[EDX.VP_Flags],VP_Flag_Accesed
         Mov    Dword ptr DS:[EDX.VP_VirtualPage],EAX
         Mov    EAX,Dword ptr DS:[EDX.VP_VFS]
         Mov    EBX,Dword ptr DS:[EDX.VP_Offset]
         Xor    EDI,EDI
          ; sem pozdeji pride kontrola, jestli se jedna o nacteni 4k bloku
          ; ze zarizeni, ktere ma bloky mensi nez 4k
          ; EBX - cislo bloku, ktery se ma nacist
          ; EAX - cislo VFS inody
         PUSH   DS ESI EDI
          STI
           Call  VFS_Check_Inode
           JC    @MM_Page_Fault_ReadFromFile_end
           LDS   ESI, fword ptr ds:[esi+VFS_I_Mount_Struc]
           Mov   EAX, EBX
           Call  Read_disk_block
          CLI
@MM_Page_Fault_ReadFromFile_end :
         POP    EDI ESI DS
         Dispose_Temp_Descriptor ES
        POP    EBX EAX ES
         Mov    Dword ptr FS:[EBX],EAX
         Invlpg Dword ptr FS:[EBX]
        Jmp    MM_Page_Fault_End
;###########################################################################
;#
;###########################################################################
;# Vstup : EDX - obraz page
;#         FS:EBX - umisteni page
;###########################################################################
@MM_Page_Fault_SwapFile :
        Jmp  MM_Page_Fault_End
;###########################################################################
;# Podadresar ve kterem pracujeme neexistuje, musime ho vytvorit
;###########################################################################
;# Vstup : EAX - obraz page
;#         FS:EBX - umisteni page
;###########################################################################
@MM_Page_Fault_SubDirectory_NP :
        Mov   EDX,EAX
        Call  PAGE_Create_New_Subdirectory_FysLin
        POP   EBX
        Jc    @MM_No_Free_Page
        Jmp   MM_Page_Fault_End
;###########################################################################
;# Vystupni brana PageFaultu
;###########################################################################
MM_Page_Fault_End:
       PUSH  FS
        Mov    AX,Data_Descriptor
        Mov    DS,AX
        Mov    EAX,Dword ptr DS:[Free_FysPages_Count]
        Cmp    EAX,Free_FysPages_Count_Limit
        Ja     @MM_Page_Fault_End_Continue
        Call   PAGE_Find_Free_Subdirectory_FysLin
        Jc     @MM_No_Free_Page
@MM_Page_Fault_End_Continue :
        Mov    EAX,Dword ptr DS:[Pages_Used]
        Mov    EBX,Dword ptr DS:[Pages_Used_Limit]
        Cmp    EAX,EBX
        Ja     @MM_Page_Fault_End_Continue2
        Call   SwapDeamon_Wake_Up_Monster
@MM_Page_Fault_End_Continue2 :
      POP FS
        Dispose_Temp_Descriptor FS
        Mov    Al,20h
        Out    0a0h,al
        OUT    20h,al
        POP    GS DS FS ES
        POPAD
        POPFD
        Add    ESP,4
         Iret





;##########################################################################
;# Obsluha specialnich chyb
;##########################################################################
;# nastala dost spatna situace - neni misto v pameti a neni
;# misto ani ve swapu... navrhuji kilnout nejaky program...
;##########################################################################
@MM_Page_Fault_General_Error :
         JMP   MM_Page_Fault_End
;##########################################################################
;# neni volny page ve fyzicke pameti, takze musime zavolat swap demona, at nam to trosku uklidi
;##########################################################################
@MM_No_Free_Page :
        Call   SwapDeamon_Wake_Up_Monster
        Jc     @MM_Page_Fault_General_Error
        Jmp  MM_Page_Fault_End
;##########################################################################
;# chyba pri swapovani - page fault vyvolan u non-swapable page, coz je picovina
;##########################################################################
@MM_Page_Fault_Fatal_Error   :
         Mov    EAX,er_MM +0003h
         Call   Fatal_Error
MM_Page_Fault Endp

