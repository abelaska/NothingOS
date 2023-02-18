;#####################################################################################
;# Funce 00000000 : Init
;#####################################################################################
;# Autodetekce disku, detekce partitionu...
;# Musi byt zavolana pred prvnim pouzitim jakekoliv fce...
;#####################################################################################
;# Vstup  : none
;# Vystup : none
;# Nici   : none
;#####################################################################################
INIT :
   PUSH   DS EDX
    Mov    DX, Data_descriptor      ;DX = Data_Selector
    Mov    DS, DX
    Mov    Dl, Byte ptr DS:[Initialized]
    Or     Dl,Dl
    Jnz    Short @INIT_Already_Initialized
      PUSHAD
       ;Zjistime, kde je partition_info descriptor
       Mov    Byte ptr DS:[Initialized],0B1h
       mov    eax,4096
       call   KAlloc
       Mov    Word ptr DS:[Part_Sel],AX
       ;Presmerujeme na sebe vektory preruseni IRQ14 a IRQ15
       Mov    EDX,0FEh
       Mov    CX,CS
       Mov    EAX,Offset Primary_HardDisk_IRQ_Routine
       Call   Set_Int_Routine
       Mov    EDX,0FFh
       Mov    CX,CS
       Mov    EAX,Offset Secondary_HardDisk_IRQ_Routine
       Call   Set_Int_Routine
       ;Autodetekce disku
       Call   IDE_Initializing
       ;nacteni partitionu ze vsech disku...
       Call   Load_Partitions_Info
      POPAD
  @INIT_Already_Initialized :
   POP    EDX DS
   RetF
;#####################################################################################
;# Funce 00000001 : Done
;#####################################################################################
;# Vypne vsechny harddisky v pocitaci
;#####################################################################################
;# Vstup  : none
;# Vystup : none
;# Nici   : none
;#####################################################################################
DONE :
   PUSHAD
   PUSH GS
    Mov  AX,Data_descriptor
    Mov  GS,AX
    Cmp  Byte ptr GS:[IDE_Drive0. ATA_ATAPI],0
    Je   @Shut_Disk_1
    Mov  EBX,014h
    Xor  EDX,EDX
    Call ATA_Gate
@Shut_Disk_1 :
    Cmp  Byte ptr GS:[IDE_Drive1. ATA_ATAPI],0
    Je   @Shut_Disk_2
    Mov  EBX,014h
    Xor  EDX,EDX
    Add  Dl,1
    Call ATA_Gate
@Shut_Disk_2 :
    Cmp  Byte ptr GS:[IDE_Drive2. ATA_ATAPI],0
    Je   @Shut_Disk_3
    Mov  EBX,014h
    Xor  EDX,EDX
    Add  Dl,2
    Call ATA_Gate
@Shut_Disk_3 :
    Cmp  Byte ptr GS:[IDE_Drive3. ATA_ATAPI],0
    Je   @Shut_Disk_4
    Mov  EBX,014h
    Xor  EDX,EDX
    Add  Dl,3
    Call ATA_Gate
@Shut_Disk_4 :
   POP  GS
   Popad
   Xor  EAX,EAX
   RetF
;#####################################################################################
;# Funce 0000002h : Read
;#####################################################################################
;# Vstup  : ES  : EDI --> cilovy buffer
;#          EDX = addressa od zacatku partiton
;#          ECX = pocet sektoru
;#####################################################################################
READ :
   Jmp  G_ATA_ReadSectors
;#####################################################################################
;# Funce 0000003h : Write
;#####################################################################################
;# Vstup  : ES  : EDI --> Zdrojovy buffer
;#          EDX = addressa od zacatku partiton
;#          ECX = pocet sektoru
;#####################################################################################
WRITE :
   Jmp  G_ATA_WriteSectors
   RetF
;#####################################################################################
;# Funce 00000004 : Seek
;#####################################################################################
;# Vstup  : EDX - addresa
;###########################################################################
SEEK :
   Mov   ECX,7
   Jmp   G_ATA_Seek
   RetF
;#####################################################################################
;# Reservovane funce 05..0F
;#####################################################################################
;# Vstup  : none
;# Vystup : none
;# Nici   : none
;#####################################################################################
UnImplement :
   RetF