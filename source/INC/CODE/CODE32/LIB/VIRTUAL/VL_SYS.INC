;#############################################################################
;# Virtualni jednotka System
;#############################################################################

;#############################################################################
;# Zakladni knihovna, call gate na ni lze kdykoliv ziskat
;#  fci EBX=00000002 int 0b1h
;#############################################################################
;# Tato knihovna ma specialni ucel, dovoluje pristupovat k tem nejdulezitejsim
;# castim OS na nekontrolovane urovni. Neexistuje tady zadna ochrana ala Booh,
;# vse je chraneno DPL call gate, ktery je jako jediny z jednotek nastaven na
;# hodnotu 0, tzn tato jednotka muze byt volana pouze programy c CPL0,
;# coz zadny normalni program nema (pouze Booh muze spustit program c CPL0, ale
;# proc by to delal). Takze tato knihovna je vlastne k pouziti pouze dalsim
;# knihovnam, Driverum a kodu kernelu, ktere vsechny maji CPL0
;#############################################################################
;# Fce      - name
;#############################################################################
;# 00000000 - Return_Kernel_Data_Segment
;# 00000001 - Get_DRV_Gate_Adr
;# 00000002 - MemMan_ReturnDescriptorBaseAddress
;# 00000003 - MemMan_SetDescriptorBaseAddress
;# 00000004 - Memory_Manager_Data2Code_Descr
;# 00000005 - Memory_Manager_Change2System_Descr_vl
;# 00000006 - Memory_Manager_ChangeDPL_Descr_vl
;# 00000007 - Memory_Manager_InvertRWBit_Descr
;# 00000008 - Memory_Manager_Data2Stack_Descr
;# 00000009 - Memory_Manager_Dispose_Descriptor
;# 0000000A - Memory_Manager_Create_Alias
;# 0000000B - Memory_Manager_Create_CallGate_vl
;# 0000000C - Create_Descriptor_GDT_Baze_vl
;# 0000000D - Memory_Manager_Allocate_Memory_TSS_LDT
;# 0000000E - MemMan_Create_CR3
;# 0000000F - FMPI_Map_File_To_Mem
;# 00000010 - FPIT_PAGE_Set_Page_Implicit_vl
;# 00000011 - FPIT_PAGE_Set_FysPage_NotUsed_vl
;# 00000012 - FPIT_PAGE_Set_FysPage_Used_vl
;# 00000013 - FPIT_PAGE_Inc_PageAge_vl
;# 00000014 - FPIT_PAGE_Set_Page_PTR_vl
;# 00000015 - FPIT_PAGE_Set_Page_CR3_vl
;# 00000016 - PAGE_SetPageAS_NotUsed
;# 00000017 - PAGE_Get_Free_Page
;# 00000018 - PAGE_SetPagesAVL_Type (in GDT)
;# 00000019 - PAGE_Find_Free_Subdirectory_FysLin
;# 0000001A - PAGE_Set_Page_As_Free
;# 0000001B - SwapDeamon_Over_Critical_Limit
;# 0000001C - SwapDeamon_Swap_Page
;# 0000001D - SwapDeamon_Restore_Page
;# 0000001E - SwapDeamon_Swap_FileMaped_Page
;# 0000001F - SwapDeamon_Restore_FileMaped_Page
;# 00000020 - TaskMan_Create_Task_Structure
;# 00000021 - TaskMan_InitializeTSS
;# 00000022 - Task_Return_Task_Info_Descriptor
;# 00000023 - Task_Return_Layout_Descriptor
;# 00000024 - Task_Return_TaskInfo_Rec_Offset
;# 00000025 - TaskMan_ReLayout
;# 00000026 - TSKMM_Inicialize_LDT
;# 00000027 - TSKMM_CreateDescriptor_LDT_vl
;# 00000028 - TSKMM_Memory_Manager_Data2Code_Descr_LDT
;# 00000029 - TSKMM_Memory_Manager_ChangeDPL_Descr_LDT_vl
;# 0000002A - TSKMM_Memory_Manager_Data2Stack_Descr_LDT
;# 0000002B - Set_Int_Routine_vl
;# 0000002C - Get_Int_Routine
;# 0000002D - Get_Partition_INFO_Selector
;#############################################################################

VL_System :
;################ Emulovana structura souboru knihovny
DB   'LIB',0                 ; Signatura
DW   0001h                   ; Version
DB   'system          '      ; Library Name
DW   0000h                   ; Unit Number  - not used in Virtual +18
DD   00000000h               ; CodeSeg size - not used in Virtual +20
Jmp VL_system_EnterPoint     ;+???
;##################################################################################
;# Vstupni bod knihovny
;##################################################################################
VL_system_EnterPoint :
          Cmp    EAX,VL_System_Num_Of_Fce
          Ja     @VL_System_End
        PUSH     DS
          PUSH   DATA_Descriptor
          Pop    DS
          Mov    EAX,Dword ptr DS:[VL_System_Fce+EAX*4]
        POP      DS
          PUSH   EAX
          Call   Dword ptr SS:[ESP]
          Jc     @VL_System_End_C
          Add    ESP,4
          CLC
          Jmp     @VL_System_End
   @VL_System_End_C :
          ADD     ESP,4
          STC
   @VL_System_End :
        RetF
;##################################################################################
;# Procedura navraci Data Descriptor kernelu
;##################################################################################
;# Vstup  : None
;# Vystup : EDX - Data Descriptor
;##################################################################################
Return_Kernel_Data_Segment PROC
        Mov   EDX,Data_Descriptor
        RET
Return_Kernel_Data_Segment ENDP
;##################################################################################
;# Procedura navraci addresu DRV_Call_Gate
;##################################################################################
;# Vstup  : None
;# Vystup : EAX:EBX - Data Descriptor
;##################################################################################
Get_DRV_Gate_Adr PROC
        Mov   EAX, Code_Descriptor
        Mov   EBX, Offset DRV_Gate
        RET
Get_DRV_Gate_Adr ENDP
;############################################################################
;# Procedura meni typ descriptoru na systemovy (pouze CPL=0)
;# Vyuziti napr. u TSS...
;############################################################################
;# Vstup  : EDX  - Selector na descriptor pro prevod - musi byt v GDT
;#          CX - typ systemoveho descriptoru
;############################################################################
;# Nici registery : EAX
;############################################################################
Memory_Manager_Change2System_Descr_vl PROC
         PUSH  EAX
          Mov   AX,CX
          Call  Memory_Manager_Change2System_Descr
         POP   EAX
        RET
Memory_Manager_Change2System_Descr_vl ENDP
;############################################################################
;# Procedura meni DPL descriptoru (pouze CPL=0)
;############################################################################
;# Vstup  : EDX  - Selector descriptoru ke zmene
;#          ECX - novy DPL
;#          FS  - selector na LDT (je-li DX bit TI = 1, tzn localni descr)
;# Vysup    EDX - novy selector (zmenene RPL)
;############################################################################
;# Nici registery : zadne
;############################################################################
Memory_Manager_ChangeDPL_Descr_vl PROC
         PUSH  EAX
         Mov   EAX,ECX
         Call  Memory_Manager_ChangeDPL_Descr
         POP   EAX
        RET
Memory_Manager_ChangeDPL_Descr_vl ENDP
;####################################################################################
;# Procedura se pokusi vytvorit novy descriptor v GDT, velikost 4k
;####################################################################################
;# Vstup  : ECX - baze
;# Vystup : If not C then EDX = Selektor
;# Nici   : EDX
;####################################################################################
Create_Descriptor_GDT_Baze_vl PROC
         PUSH  EAX
        Mov    EAX,ECX
        Call   Create_Descriptor_GDT_Baze
         POP   EAX
        RET
Create_Descriptor_GDT_Baze_vl ENDP
;########################################################################################
;# Procedura nastavuje vsechna pole fys_page_info_item na hodnoty :
;#  AGE      = 0
;#  Pointer to virtual page
;########################################################################################
;# Vstup  : ECX - cislo 4k page v poradi
;#          EBX - Obraz virtualni page
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Page_Implicit_vl PROC
         PUSH  EAX
         Mov    EAX, ECX
         Call   FPIT_PAGE_Set_Page_Implicit
         POP   EAX
        RET
FPIT_PAGE_Set_Page_Implicit_vl ENDP
;########################################################################################
;# Procedura nastavuje hodnotu zaznamu ve fys_page_info_table na cached
;########################################################################################
;# Vstup  : ECX - cislo 4k page v poradi
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_FysPage_NotUsed_vl PROC
         PUSH  EAX
        Mov   EAX,ECX
        Call  FPIT_PAGE_Set_FysPage_NotUsed
         POP   EAX
        RET
FPIT_PAGE_Set_FysPage_NotUsed_vl ENDP
;########################################################################################
;# Procedura nastavuje hodnotu zaznamu ve fys_page_info_table na Used
;########################################################################################
;# Vstup  : ECX - cislo 4k page v poradi
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_FysPage_Used_vl PROC
         PUSH  EAX
         Mov   EAX, ECX
         Call  FPIT_PAGE_Set_FysPage_Used
         POP   EAX
        RET
FPIT_PAGE_Set_FysPage_Used_vl ENDP
;########################################################################################
;# Procedura Incrementuje pole AGE
;########################################################################################
;# Vstup  : ECX - cislo 4k page v poradi
;# Vystup : ECX - Hodnota pole AGE
;########################################################################################
FPIT_PAGE_Inc_PageAge_vl PROC
         PUSH EAX
         Mov  EAX, ECX
         Call FPIT_PAGE_Inc_PageAge
         POP  EAX
        RET
FPIT_PAGE_Inc_PageAge_vl ENDP
;########################################################################################
;# Procedura nastavuje hodnotu pole VirtualPage
;########################################################################################
;# Vstup  : ECX - cislo 4k page v poradi
;#          EBX - Page nebo (Cislo_zaznamu_v_tabulce*4096)
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Page_PTR_vl PROC
         PUSH  EAX
          Mov   EAX,ECX
          Call  FPIT_PAGE_Set_Page_PTR
         POP   EAX
        RET
FPIT_PAGE_Set_Page_PTR_vl ENDP
;########################################################################################
;# Procedura nastavuje hodnotu pole CR3
;########################################################################################
;# Vstup  : ECX - cislo 4k page v poradi
;#          EBX - CR3
;# Vystup : None
;# Nici   : None
;########################################################################################
FPIT_PAGE_Set_Page_CR3_vl PROC
         PUSH  EAX
          Mov   EAX,ECX
          Call  FPIT_PAGE_Set_Page_CR3
         POP   EAX
        RET
FPIT_PAGE_Set_Page_CR3_vl ENDP
;########################################################################################
;# Procedura navraci
;########################################################################################
;# Vstup  : None
;# Vystup : EDX - Task_Info_Descriptor
;# Nici   : None
;########################################################################################
Task_Return_Task_Info_Descriptor PROC
        PUSH  DS
         Mov    EDX,Data_Descriptor
         Mov    DS,EDX
         Movzx  EDX,word ptr DS:[TaskMan_TSK_Info_Descriptor]
        POP   DS
        RET
Task_Return_Task_Info_Descriptor ENDP
;############################################################################
;# Procedura vytvari descriptor v dane tabulce, Datovy Descriptor,
;# G=1,D/B=1, CPL3
;############################################################################
;# Vstup  : ECX  - Size ve strankach
;#          EDX  - Virtualni Adresa
;############################################################################
;# Nici :   EDX
;############################################################################
TSKMM_CreateDescriptor_LDT_vl PROC
        PUSH  EAX
         Mov   EAX,EDX
         Call  TSKMM_CreateDescriptor_LDT
        POP   EAX
        RET
TSKMM_CreateDescriptor_LDT_vl ENDP
;############################################################################
;# Procedura meni DPL descriptoru v LDT
;############################################################################
;# Vstup  : EDX  - Selector descriptoru ke zmene
;#          ECX - novy DPL
;# Vystup   EDX - novy selector (zmenene RPL)
;############################################################################
TSKMM_Memory_Manager_ChangeDPL_Descr_LDT_vl PROC
        PUSH  EAX
         Mov   EAX,ECX
         Call  TSKMM_Memory_Manager_ChangeDPL_Descr_LDT
        POP   EAX
        RET
TSKMM_Memory_Manager_ChangeDPL_Descr_LDT_vl ENDP
;#############################################################################
;# nastavuje obsluhu preruseni daneho INT
;#############################################################################
;# Vstup  : (CX:EBX) - vstupni bod preruseni
;#          DL       - cislo preruseni
;# Vystup : None
;# Nici   : None
;#################################################################################
Set_Int_Routine_vl PROC
        PUSH EAX
          Mov  EAX,EBX
          Call Set_Int_Routine
        POP  EAX
        RET
Set_Int_Routine_vl ENDP
;########################################################################################
;# Procedura navraci Task_Layout_Descriptor
;########################################################################################
;# Vstup  : None
;# Vystup : EDX - Task_Layout_Descriptor
;# Nici   : None
;########################################################################################
Task_Return_Layout_Descriptor PROC
        PUSH  DS
         Mov    EDX,Data_Descriptor
         Mov    DS,EDX
         Movzx  EDX,word ptr DS:[TaskMan_TSK_TSK_Layout_Descriptor]
        POP   DS
        RET
Task_Return_Layout_Descriptor ENDP
;##############################################################################



;#############################################################################
;# nastavuje obsluhu preruseni daneho INT
;#############################################################################
;# Vstup  : (CX:EAX) - vstupni bod preruseni
;#          DL       - cislo preruseni
;# Vystup : None
;# Nici   : None
;#################################################################################
Set_Int_Routine :
       PUSH     EDX EAX ES
       PUSHFD
         CLI
         And     EDX,0FFh
         Shl     EDX,3
        Push    IDT_Descriptor
        Pop     Es
         Mov     Word ptr Es:[EDX.IDT_struc_Selector],CX
         Mov     Es:[EDX.IDT_struc_Offset_lo],Ax
         Shr     EAX,16
         Mov     Es:[EDX.IDT_struc_Offset_Hi],Ax
         Mov     Es:[EDX.IDT_struc_Flags],Int_Atr1
       POPFD
        POP      ES EAX EDX
    RET
;#############################################################################
;#  navraci informace o rutine preruseni
;#############################################################################
;# Vstup  : DL - Cislo preruseni
;# Vystup : (CX:EAX) = Addressa obsluhy preruseni
;# Nici   :  ECX, EAX
;#############################################################################
Get_Int_Routine :
       PUSH     EDX ES
       PUSHFD
         CLI
         And     EDX,0FFh
         Shl     EDX,3
        Push    IDT_Descriptor
        Pop     Es
         Mov     CX,Word ptr Es:[EDX.IDT_struc_Selector]
         Mov     AX,Es:[EDX.IDT_struc_Offset_Hi]
         Shl     EAX,16
         Mov     AX,Es:[EDX.IDT_struc_Offset_lo]
       POPFD
       POP      ES EDX
    RET
;#############################################################################
;#  navraci selektor na partitions
;#############################################################################
;# Vstup  : none
;# Vystup : AX            - selector na partition_info
;#          EBX           - Max_Partitions_Count
;#############################################################################
Get_Partition_INFO_Selector :
      PUSH  DS
      Mov   AX,DATA_Descriptor
      Mov   DS,AX
      Mov   AX,Word ptr DS:[Sel_Partitions]
      Mov   EBX,Max_Partition_Count
      POP   DS
    RET