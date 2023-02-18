;#############################################################################
;# Procedury ktere pracuji s Fys_Page_Info_Table structurou
;#############################################################################

;########################################################################################
;# Procedura nastavuje vsechna pole fys_page_info_item na hodnoty :
;#  AGE      = 0
;#  Pointer to virtual page
;########################################################################################
;# Vstup  : EAX - cislo 4k page v poradi
;#          EBX - Obraz virtualni page
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Page_Implicit PROC
       PUSH  EBX EDX EAX ES
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EDX,FPIT_Size
        Mul  EDX
        Mov  EDX,EAX
        Mov  EAX,CR3
        Shr  EAX,12
        Mov  Dword ptr ES:[EDX.FP_CR3Coded],EAX
        And  BX,0F000h
        Or   EBX,Fys_Page_Used_Mask
        Mov  DWord ptr ES:[EDX.FP_VirtualPage],EBX
       POP   ES EAX EDX EBX
        Ret
FPIT_PAGE_Set_Page_Implicit ENDP
;########################################################################################
;# Procedura nastavuje vsechna pole fys_page_info_item na hodnoty :
;#  AGE      = 0
;#  Pointer to virtual page
;########################################################################################
;# Vstup  : EDX - Obraz virtualni page
;#          EAX - Cislo 4k page v poradi
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Page_Implicit_File PROC
       PUSH  EBX EDX EAX ES
        Mov  EBX,EDX
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EDX,FPIT_Size
        Mul  EDX
        Mov  EDX,EAX
        Mov  EAX,CR3
        Shr  EAX,12
        Mov  Dword ptr ES:[EDX.FP_CR3Coded],EAX
        And   BX,0F000h
        Or    EBX,Fys_Page_Used_Mask_File
        Mov   DWord ptr ES:[EDX.FP_VirtualPage],EBX
       POP   ES EAX EDX EBX
        Ret
FPIT_PAGE_Set_Page_Implicit_File ENDP
;########################################################################################
;# Procedura nastavuje hodnotu zaznamu ve fys_page_info_table na cached
;########################################################################################
;# Vstup  : EAX - cislo 4k page v poradi
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_FysPage_NotUsed PROC
       PUSH  EDX EAX ES
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EDX,FPIT_Size
        Mul  EDX
        Mov  EDX,EAX
        And  DWord ptr ES:[EDX.FP_VirtualPage],Fys_Page_Used_MaskN
       POP   ES EAX EDX
        Ret
FPIT_PAGE_Set_FysPage_NotUsed ENDP
;########################################################################################
;# Procedura nastavuje hodnotu zaznamu ve fys_page_info_table na Used
;########################################################################################
;# Vstup  : EAX - cislo 4k page v poradi
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_FysPage_Used PROC
       PUSH  EDX EAX ES
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EDX,FPIT_Size
        Mul  EDX
        Mov  EDX,EAX
        Or   DWord ptr ES:[EDX.FP_VirtualPage],Fys_Page_Used_Mask
       POP   ES EAX EDX
        Ret
FPIT_PAGE_Set_FysPage_Used ENDP
;########################################################################################
;# Procedura Incrementuje pole AGE
;########################################################################################
;# Vstup  : EAX - cislo 4k page v poradi
;# Vystup : ECX - Hodnota pole AGE
;# Nici   : None
;########################################################################################
FPIT_PAGE_Inc_PageAge PROC
       PUSH  EBX EDX EAX ES
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EDX,FPIT_Size
        Mul  EDX
        Mov  EDX,EAX
        Mov  ECX,Dword ptr ES:[EDX.FP_VirtualPage]
        Mov  EBX,ECX
        And  ECX,Fys_Page_Age_Mask
        Cmp  ECX,255
        Je   @PAGE_Inc_PageAgeSkip_Incrementing
        Inc  CX
        And  EBX,Fys_Page_Age_Mask_N
        Or   BX,CX
        Mov  Dword ptr ES:[EDX.FP_VirtualPage],ECX
@PAGE_Inc_PageAgeSkip_Incrementing :
       POP   ES EAX EDX EBX
        Ret
FPIT_PAGE_Inc_PageAge ENDP
;########################################################################################
;# Procedura nastavuje hodnotu pole VirtualPage
;########################################################################################
;# Vstup  : EAX - cislo 4k page v poradi
;#          EBX - Page nebo (Cislo_zaznamu_v_tabulce*4096)
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Page_PTR PROC
       PUSH  EBX EDX EAX ES
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EDX,FPIT_Size
        Mul  EDX
        Mov  EDX,EAX
        Mov  EAX,DWord ptr ES:[EDX.FP_VirtualPage]
        And  EAX,0FFFh
        And  BX,0F000h
        Or   EAX,EBX
        Mov  DWord ptr ES:[EDX.FP_VirtualPage],EAX
       POP   ES EAX EDX EBX
        Ret
FPIT_PAGE_Set_Page_PTR ENDP
;########################################################################################
;# Procedura nastavuje hodnotu pole CR3
;########################################################################################
;# Vstup  : EAX - cislo 4k page v poradi
;#          EBX - CR3
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Page_CR3 PROC
       PUSH  EBX EDX EAX ES
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EDX,FPIT_Size
        Mul  EDX
        Mov  EDX,EAX
        Shr  EBX,12
        Mov  Word ptr ES:[EDX.FP_CR3Coded],BX
       POP   ES EAX EDX EBX
        Ret
FPIT_PAGE_Set_Page_CR3 ENDP
;########################################################################################
;# Procedura nastavuje hodnotu pole CR3
;########################################################################################
;# Vstup  : EBX - cislo 4k page v poradi
;# Vystup : EBX - ukazatel do  FP_VirtualPage
;# Nici   : None
;########################################################################################
FPIT_PAGE_Ret_Page_VP PROC
       PUSH  EDX EAX ES
        Mov  EDX,FysPageInfoItem_Descriptor
        Mov  ES,EDX
        Mov  EAX,FPIT_Size
        Mul  EBX
        Mov  EBX,DWord ptr ES:[EAX.FP_VirtualPage]
       POP   ES EAX EDX
        Ret
FPIT_PAGE_Ret_Page_VP ENDP
;########################################################################################
;# Procedura nastavuje hodnotu poli FPIT fyzickych pagi pro dany descriptor
;# na inicializacni hodnoty
;# Napriklad nastavuje hodnoty pro FysLin descriptory,...
;########################################################################################
;# Vstup  : EDX - Descriptor
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Kernels_Descriptor PROC
       PUSH  DS GS FS EDI ECX EAX EDX EBX
        Mov   ECX,EDX
        LSL   ECX,ECX
        Inc   ECX
        Shr   ECX,12     ;ECX = pocet page ktery dany descriptor obsahuje :)
        Call  MemMan_ReturnDescriptorBaseAddress
        Mov   EDX,EAX    ;EDX = bazova adresa descriptoru

        ;Datovy alias na adresar page
        PUSH   EDX
         Create_Temp_Descriptor CR3
         Mov    FS, DX
         Invlpg Byte ptr FS:[0]
         Create_Temp_Descriptor EAX
         Mov    DS, DX
         Invlpg Byte ptr DS:[0]
        POP    EDX

@PAGE_SetUp_Kernel_Descriptor_Loop :
        Mov   EBX,EDX
        PUSH   EDX
         Mov   EDI,EDX
         Shr   EDX,22
         Shl   EDX,2
         Mov   EAX,Dword ptr FS:[EDX]
         Test  EAX,1
         Jz    @PAGE_SetUp_Kernel_Descriptor_Skip
@PAGE_SetUp_Kernel_Descriptor_Skip_Create :
         And   AX,0F000h
         Remap_Temp_Descriptor DS,EAX
         Shr   EDI,12
         Shl   EDI,2
         And   EDI,0FFFh
         Mov   EAX,Dword ptr DS:[EDI]
         Shr   EAX,12 ;cislo ente page :)
         Call  FPIT_PAGE_Set_Page_PTR
         Mov   EBX,CR3
         Call  FPIT_PAGE_Set_Page_CR3
         Call  FPIT_PAGE_Set_FysPage_Used
@PAGE_SetUp_Kernel_Descriptor_Skip :
        POP    EDX
        Add    EDX,4096
        Dec    ECX
        Jnz    @PAGE_SetUp_Kernel_Descriptor_Loop
        Dispose_Temp_Descriptor FS
        Dispose_Temp_Descriptor DS
       POP   EBX EDX EAX ECX EDI FS GS DS
        CLC
        RET
FPIT_PAGE_Set_Kernels_Descriptor ENDP


