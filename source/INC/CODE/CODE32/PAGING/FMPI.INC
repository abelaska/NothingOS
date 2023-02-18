;############################################################################################
;# FMPI - poprogramy pro praci s File_Maped_Pages_Info structurou
;############################################################################################

;############################################################################################
;# Procedura Inicializuje FMPI strukturu
;############################################################################################
FMPI_Init PROC
         PUSH    DS
          Mov    EAX,Data_Descriptor
          Mov    DS,EAX
          Mov    EAX,VMem_Shared_Area_Size
          Shr    EAX,12 ;EAX = pocet 4k page v Shared Area
          Xor    EDX,EDX
          Mov    ECX,FMPI_Size
          Mul    ECX
          Call   KAlloc
          Mov    Dword ptr DS:[FMPI_Selector],EAX
          Xor    EAX,EAX
          Mov    Dword ptr DS:[FMPI_LastRec],EAX
         POP     DS
       RET
FMPI_Init ENDP
;############################################################################################
;# Procedura projde FMPI strukturou a hleda volnou polozku.
;############################################################################################
;# Vstup  : none
;# Vystup : EDX - Offset Zacatku volne polozky
;#          ECX - Cislo ente polozky
;#          If C Then Error
;############################################################################################
FMPI_Find_Free_Entry PROC
        PUSH  EAX EBX ES DS
         Mov   ECX,Data_Descriptor
         Mov   DS,ECX
         Mov   ECX,VMem_Shared_Area_Size
         Shr   ECX,12
         Mov   ES,Dword ptr DS:[FMPI_Selector]
         mov   ebx, Dword ptr DS:[FMPI_LastRec]
         mov   edx, Dword ptr DS:[FMPI_LastRecOfs]
@FMPI_Find_Free_Entry_Loop :
         Movzx EAX,Byte ptr ES:[EDX.VP_CountUse]
         Or    EAX,EAX
         Jz    Short @FMPI_Find_Free_Entry_Loop_Found
         Inc   EBX
         Add   EDX,FMPI_Size
         Dec   ECX
         Jnz   Short @FMPI_Find_Free_Entry_Loop
         Xor   EDX,EDX
         xor   ebx,ebx
         Jmp   @FMPI_Find_Free_Entry_Loop
@FMPI_Find_Free_Entry_Loop_Found :
         mov   Byte ptr ES:[EDX.VP_CountUse], 1
         Mov   Dword ptr DS:[FMPI_LastRecOfs],EDX
         Mov   Dword ptr DS:[FMPI_LastRec],EBX
         Mov   ECX,EBX
         CLC
@FMPI_Find_Free_Entry_Loop_End :
        POP   DS ES EBX EAX
       RET
FMPI_Find_Free_Entry ENDP
;############################################################################################
;# Procedura nastavuje hodnoty zaznamu v FMPI
;############################################################################################
;# Vstup : EDX - offset zaznamu
;#         EBX - Offset od zacatku souboru
;#         ESI - VFS inoda
;#         EAX - Virtual Page
;############################################################################################
FMPI_Set_Up_Entry    PROC
      PUSH  EAX ECX DS
         PUSH  Data_Descriptor
         POP   DS
         Mov   DS,Dword ptr DS:[FMPI_Selector]
         Mov   Dword ptr DS:[EDX+VP_VFS],ESI
         Mov   Dword ptr DS:[EDX+VP_Offset],EBX
         Mov   Byte  ptr DS:[EDX+VP_Flags],VP_Flag_Normal
         Mov   Byte  ptr DS:[EDX+VP_CountUse],1
         Mov   Dword ptr DS:[EDX+VP_VirtualPage],EAX
      POP  DS ECX EAX
       RET
FMPI_Set_Up_Entry    ENDP
;############################################################################################
;# Procedura namapuje VFS inodu na page descriptoru obrazu
;############################################################################################
;# Vstup  - EDX - Descriptor na ktery se ma namapovat soubor
;#          EBX - VFS Inode
;#          ES     - buffer s adresovacima polozkama (pocatecni ofs hodnota je 0)
;#                 - pricemz kazda polozka adresuje 4k blok
;# Vystup - If C then error
;############################################################################################
FMPI_Map_File_To_Mem PROC
       PUSH  DS GS FS
       PUSHAD
        MOV   ESI,EBX    ;ESI = VFS Inode
        Mov   ECX,EDX
        LSL   ECX,ECX
        Inc   ECX
        Shr   ECX,12     ;ECX = pocet page ktery dany descriptor obsahuje :)
        Call  MemMan_ReturnDescriptorBaseAddress
        Mov   EDX,EAX    ;EDX = bazova adresa descriptoru
        ;Datovy alias na adresar page
        Push  EDX
         Create_Temp_Descriptor CR3
         Jc    @FMPI_Map_File_To_Mem_No_Descriptor
         Mov   FS, EDX
         Create_Temp_Descriptor EAX
         Jc    @FMPI_Map_File_To_Mem_No_Descriptor
         Mov   GS,EDX
        POP   EDX
        Invlpg Byte ptr FS:[0]
        Xor   EBX,EBX
        ; EDX - bazova adresa descriptoru
        ; ECX - Pocet page
        ; FS  - alias na adresar page
        ; ESI - VFS indoda
        ; EBX - Offset od zacatku souboru po 4k blocich
@FMPI_Map_File_To_Mem_Loop :
       PUSH  EDX
        Mov   EDI,EDX
        Shr   EDX,22
        Shl   EDX,2
        Mov   EAX, Dword ptr FS:[EDX]
        Test  Al, 1
        Jnz   @FMPI_Map_File_To_Mem_Skip_Creat_SubDirectory
        PUSH  EDX EBX
          Mov  EBX,EDX
          Mov  EDX,EAX
          Call PAGE_Create_New_Subdirectory_FysLin
        POP   EBX EDX
          Invlpg Dword ptr FS:[EDX]
          Mov  EAX, Dword ptr FS:[EDX]
@FMPI_Map_File_To_Mem_Skip_Creat_SubDirectory :
         And   AX,0F000h
         Remap_Temp_Descriptor GS,EAX ;premapujem tempovaci descriptor
         Shr   EDI,12
         And   EDI,03FFh
         Mov   EBP,1024
         Sub   EBP,EDI
         Shl   EDI,2
         ;EBP = poctet page do konce
         ;EDI = offset uvnitr podadresare
        POP    EAX
@FMPI_Map_File_To_Mem_Inter_Loop :
        PUSH ECX
         Call  FMPI_Find_Free_Entry
         ;EDX = offset volne polozky
         ;ECX = cislo volne polozky
         ;EAX = virtual page
         ;ESI = VFS inode
         ;EBX = offset v souboru
         push   ebx
          mov   ebx, dword ptr es:[ebx]
          Call  FMPI_Set_Up_Entry
         pop    ebx
          Shl   ECX,12
          And   CX,0F000h
          Or    CX,_PAGE_FileSwap_All12
          Mov   Dword ptr GS:[EDI],ECX
          Invlpg Dword ptr GS:[EDI]
         POP  ECX
         Add   EDI, 4
         add   ebx, 4
         Add   EAX, 4096
         Dec   ECX
         Jz    @FMPI_Map_File_To_Mem_Loop_End
         Dec   EBP
         Jnz   @FMPI_Map_File_To_Mem_Inter_Loop
         Mov   EDX, EAX
         Jmp   @FMPI_Map_File_To_Mem_Loop
@FMPI_Map_File_To_Mem_Loop_End :
      Dispose_Temp_Descriptor FS
      Dispose_Temp_Descriptor GS
      POPAD
       POP   FS GS DS
       CLC
        RET
@FMPI_Map_File_To_Mem_No_Descriptor :
      POP   EDX
      POPAD
       POP   FS GS DS
       STC
        RET
FMPI_Map_File_To_Mem ENDP

;############################################################################################
;# Procedura ulozi soubor mapovany v pameti zpatky na hadr
;############################################################################################
;# Vstup  - EDX - Descriptor na ktery je namapovany soubor
;# Vystup - If C then error
;############################################################################################
FMPI_Save_File_To_HD PROC
       PUSH  DS GS FS
       PUSHAD
        Mov   ECX,EDX
        LSL   ECX,ECX
        Inc   ECX
        Shr   ECX,12     ;ECX = pocet page ktery dany descriptor obsahuje :)
        Call  MemMan_ReturnDescriptorBaseAddress
        Mov   EDX,EAX    ;EDX = bazova adresa descriptoru
        ;Datovy alias na adresar page
        Push  EDX
         Create_Temp_Descriptor CR3
         Jc    @FMPI_Save_File_To_HD_No_Descriptor
         Mov   FS, EDX
         Create_Temp_Descriptor EAX
         Jc    @FMPI_Save_File_To_HD_No_Descriptor
         Mov   GS,EDX
        POP   EDX
        Invlpg Byte ptr FS:[0]
        Xor   EBX,EBX
        ; EDX - bazova adresa descriptoru
        ; ECX - Pocet page
        ; FS  - alias na adresar page
@FMPI_Save_File_To_HD_Loop :
       PUSH  EDX
        Mov   EDI,EDX
        Shr   EDX,22
        Shl   EDX,2
        Mov   EAX, Dword ptr FS:[EDX]
@FMPI_Save_File_To_HD_Skip_Creat_SubDirectory :
         And   AX,0F000h
         Remap_Temp_Descriptor GS,EAX ;presaveujem tempovaci descriptor
         Shr   EDI,12
         And   EDI,03FFh
         Mov   EBP,1024
         Sub   EBP,EDI
         Shl   EDI,2
         ;EBP = poctet page do konce
         ;EDI = offset uvnitr podadresare
        POP    EAX
@FMPI_Save_File_To_HD_Inter_Loop :
        PUSH   EAX
         Mov   EBX, Dword ptr GS:[EDI]
;         Test  EBX, 1000000b
;         Jz    @FMPI_Save_File_To_HD_Inter_Loop_Skip
         Shr   EBX,9
         Mov   Al,Bl
         And   Al,7
         Cmp   Al,3
         Jne   @FMPI_Save_File_To_HD_Inter_Loop_Skip
         Shr   EBX,3
         Call  FPIT_PAGE_Ret_Page_VP
         Shr   EBX,12
         ;EBX - index do tabulky
        push    es ds esi edi eax ebx ecx edx
         Mov    EAX,Data_descriptor
         Mov    DS,EAX
         Mov    DS,Dword ptr DS:[FMPI_Selector]
         Mov    EAX,FMPI_Size
         Mul    EBX
         Mov    EDX,EAX
         Mov    EAX,Dword ptr DS:[EDX.VP_VFS]
         Mov    EBX,Dword ptr DS:[EDX.VP_Offset]
         Mov    ecx, Dword ptr DS:[EDX.VP_VirtualPage]
         Create_Temp_Descriptor ECX
         mov     es, edx
         call    VFS_Check_Inode
         jc      @FMPI_Save_File_To_HD_Inter_Loop_toobad
         lds     esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
         xor     edi, edi
         mov     eax, ebx
         call    Write_disk_block
@FMPI_Save_File_To_HD_Inter_Loop_toobad:
         Dispose_Temp_Descriptor ES
        pop     edx ecx ebx eax edi esi ds es
@FMPI_Save_File_To_HD_Inter_Loop_Skip :
        POP    EAX
         Add   EDI, 4
         Add   EAX, 4096
         Dec   ECX
         Jz    @FMPI_Save_File_To_HD_Loop_End
         Dec   EBP
         Jnz   @FMPI_Save_File_To_HD_Inter_Loop
         Mov   EDX, EAX
         Jmp   @FMPI_Save_File_To_HD_Loop
@FMPI_Save_File_To_HD_Loop_End :
      Dispose_Temp_Descriptor FS
      Dispose_Temp_Descriptor GS
      POPAD
       POP   FS GS DS
       CLC
        RET
@FMPI_Save_File_To_HD_No_Descriptor :
      POP   EDX
      POPAD
       POP   FS GS DS
       STC
        RET
FMPI_Save_File_To_HD ENDP


