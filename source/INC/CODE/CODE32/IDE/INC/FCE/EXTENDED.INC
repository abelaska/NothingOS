;###########################################################################
;# Funkce 0000010h : Execute Drive Diagnostics
;# Diagnosticka sluzba, v ERROR registeru vysledek
;###########################################################################
;# Vstup  : none
;# Vystup : v Error Registreru ulozeny informace = BL
;###########################################################################
G_ATA_DriveDiagnostics :
     Call  Count_LBA_And_Disk_2
     Xor   EBX,EBX
     Or      EBX,EBX
     Jz      @IDE_error_invalid_input_parameters
     Xor     EBX,EBX
     Call    ATA_Gate
     Xor     EAX,EAx
     Retf
;#####################################################################################
;# Funce 00000011h : Format Track
;#####################################################################################
;# Procedura provadi Low level format
;#####################################################################################
;# Vstup  : ES  = EDI --> zdrojovy buffer
;#          EDX = addressa od zacatku partitiony
;#####################################################################################
G_ATA_FormatTrack      :
      Call    Count_LBA_And_Disk
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,01h
      Call    ATA_Gate
      Retf
;###########################################################################
;# Funce 012h - Initialize Drive Parameters
;# Vsup : Dl = sektoru na stopu
;#        Dh = cislo (hlav-1)
;###########################################################################
G_ATA_InitDriveParam   : ;012h
      Shr    EAX,24
      Push   EAX
      Mov    EAX,EDX
      Pop    EDX
      Dec    EDX
      Mov    EBX,02h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 013h - ATA Read Bufffer
;# Precte 512 byte z Bufferu
;###########################################################################
;# Vstup  : ES : EDI --> cilovy buffer
;###########################################################################
G_ATA_ReadBuffer       :
     Mov    EBX, 03h
     Call   ATA_Gate
     RetF
;#####################################################################################
;# Funce 00000014h : Read Long
;#####################################################################################
;# Cteni 512 byte data, nasleduje x ECC byte
;#####################################################################################
;# Vstup  : ES  : EDI --> cilovy buffer
;#          EDX = addressa od zacatku partitiony
;#          Cl = 0 - s opakovani
;#               1 - bez opakovani
;#####################################################################################
G_ATA_ReadLong :
      Call    Count_LBA_And_Disk
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,04h
      Call    ATA_Gate
      Retf
;###########################################################################
;# Funkce 0015h : Set Multiple Mode
;# Nastavuje, sektoru bude v bloku, ktery bude cist/psat ReadMultiple
;# nebo WriteMultiple command. Zavisi na velikosti bufferu. Viz DriveInfo
;###########################################################################
;# Vstup : Cl = Sektoru v bloku
;###########################################################################
G_ATA_SetMultipleMode  :
      Shr     EAX,24
      Mov     EDX,EAX
      Dec     EDX
      Mov     EBX,05h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 0000016 : Read Multiple
;###########################################################################
;# Cte bez preruseni tolik sektoru, kolik je udano v CL (a v predchazejici
;# fci SetMultipleMode)
;###########################################################################
;# Vstup : ES  = EDI --> cilovy buffer
;#         EDX = addressa
;#         Cl  = pocet sektoru v bloku
;#             = musi byt stejne jako hodnota Sektoru v Bloku fce SetMultipleMode
;###########################################################################
G_ATA_ReadMultiple :
      Call    Count_LBA_And_Disk
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,06h
      Call    ATA_Gate
      Retf
;#####################################################################################
;# Funce 00000017h : ReadSectors
;#####################################################################################
;# Read Sectors
;# Cte tolik sektoru, kolik je udano v CL. Mezi nactenim kazdeho sektoru
;# musi cekat, nez pro nej HD pripravi data => pomalejsi nez Multiple :)
;###########################################################################
;# Vstup  : ES  = EDI --> cilovy buffer
;#          EDX = addressa od zacatku partitiony
;#          Cl  = pocet sektoru k precteni
;#          Ch  = 0 - s opakovanim
;#              = 1 - bez opakovani
;#####################################################################################
G_ATA_ReadSectors      :
      Call    Count_LBA_And_Disk
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,07h
      Call    ATA_Gate
      Retf
;###########################################################################
;# Funkce Read Verify Sectors : 18h
;# proveruje tolik sektoru, kolik je v Cl. Je-li chyba, pak pouzit
;# ATA_GetCurrentPos pro zjisteni, na jakem sektoru vznikla chyba
;###########################################################################
;# Vstup : EDX = addressa
;#         Cl  = pocet sektoru k provereni
;#         Ch  = 0 - s opakovanim
;#             = 1 - bez opakovani
;###########################################################################
G_ATA_VerifySectors    :
      Call    Count_LBA_And_Disk_2
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,08h
      Call    ATA_Gate
      Retf
;###########################################################################
;# Funkce 19h : Get Position
;# Navraci addresu ulozenou v dane chvili v registerech
;###########################################################################
;# Vstup  : none
;# Vystup : EBX = Addresa
;###########################################################################
G_ATA_GetCurrentPos    : ;019h
      Shr     EAX,24
      Mov     EDX,EAX
      Dec     EDX
      Mov     EBX,09h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 1Ah - Recalibrate
;# Nastavuje hlavu na 0 pozici... neli li 0 nalezena => Track Not Found error
;###########################################################################
;# Vstup  : Dl - rychlost presunu - <0..0F>
;###########################################################################
G_ATA_Recalibrate      :
      Shr     EAX,24
      Push    EDX
       Mov     EDX,EAX
       Dec     EDX
      Pop     EAX
      Mov     EBX,0Ah
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 1Bh - Seek
;# Nastavuje hlavu na pozici udanou v EAX rychlosti v Cl
;###########################################################################
;# Vstup  : Cl  - rychlost presunu - <0..0F>
;#          EDX - addresa
;###########################################################################
G_ATA_Seek             : ;01Bh
      Shr     EAX,24
      Push    EDX
       Mov     EDX,EAX
       Dec     EDX
      Pop     EAX
      Mov     EBX,0Bh
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 1Ch - Set Feauture
;# nastaveni urcitich parametru... ATA reference manuall 5.4.14
;###########################################################################
;# Vstup  : Dl - parametr
;###########################################################################
G_ATA_SetFeauture      : ;01Ch
      Shr     EAX,24
      Push    EDX
       Mov     EDX,EAX
       Dec     EDX
      Pop     EAX
      Mov     EBX,0Ch
      Call    ATA_Gate
      RetF
G_ATA_WriteBuffer      : ;01Dh
      RetF
;###########################################################################
;# Funkce 01Eh : ATA Write Long
;# Zapise 520 byte na adressu, 512 dat a 8 byte ECC bitu..
;###########################################################################
;# Vstup : Cl  - 0 = s opakovanim
;#             - 1 = bez opakovani
;#         EDX - Addresa od zacatku partitiony
;#         ES  : EDI --> Zdrojovy buffer
;###########################################################################
G_ATA_WriteLong        :
      Call    Count_LBA_And_Disk
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,0Eh
      Call    ATA_Gate
      Retf
;###########################################################################
;# Funkce 01Fh : ATA Write Multiple
;# Zapise Cl-sektoru bez vyruseni, Cl = nastaveni z fce ATA_SetMultiple
;###########################################################################
;# Vstup : Cl - pocet sektoru v bloku
;#         EDX - Addresa od zacatku partitiony
;#         ES  : EDI --> Zdrojovy buffer
;###########################################################################
G_ATA_WriteMultiple    : ;01Fh
      Call    Count_LBA_And_Disk
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,0Fh
      Call    ATA_Gate
      Retf
;#####################################################################################
;# Funce 00000020h : WriteSectors
;#####################################################################################
;# Read Sectors
;# Cte tolik sektoru, kolik je udano v CL. Mezi nactenim kazdeho sektoru
;# musi cekat, nez pro nej HD pripravi data => pomalejsi nez Multiple :)
;###########################################################################
;# Vstup  : ES  : EDI --> Zdrojovy buffer
;#          EDX = addressa od zacatku partitiony
;#          Cl  = pocet sektoru k zapsani
;#          Ch  = 0 - s opakovanim
;#              = 1 - bez opakovani
;#####################################################################################
G_ATA_WriteSectors     :
      Call    Count_LBA_And_Disk
      Or      EBX,EBX
      Jz      @IDE_error_invalid_input_parameters
      Mov     EBX,10h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 21h - ATA Check Power Mode
;# zjistiju aktualni mod HD
;###########################################################################
;# Vystup   : Al - mod
;###########################################################################
G_ATA_CheckPowerMode   : ;021h
      Shr     EAX,24
      Mov     EDX,EAX
      Dec     EDX
      Mov     EBX,011h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 22h - ATA Set Idle Mode
;# Nastavuje mod HD na idle
;###########################################################################
;# Vstup   : Dl - cas, po jehoz vyprseni se prepne do Idle Mode (x5s)
;###########################################################################
G_ATA_SetIDleMode      :
      Shr     EAX,24
      Push    EDX
       Mov     EDX,EAX
       Dec     EDX
      Pop     EAX
      Mov     EBX,012h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 23h - ATA Set Idle Mode Immediatly
;# Nastavuje mod HD na idle, okamzite
;###########################################################################
G_ATA_IdleImmediatly   : ;023h
      Shr     EAX,24
      Mov     EDX,EAX
      Dec     EDX
      Mov     EBX,013h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 24h - ATA Sleep
;# Nastavuje mod HD na sleep -> jediny navrat = reset :)
;###########################################################################
G_ATA_Sleep            : ;024h
      Shr     EAX,24
      Mov     EDX,EAX
      Dec     EDX
      Mov     EBX,014h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 25h - ATA Set Standby Mode
;# Nastavuje mod HD na standby
;###########################################################################
;# Vstup   : Dl - cas, po jehoz vyprseni se prepne (x5s)
;###########################################################################
G_ATA_Standby          : ;025h
      Shr     EAX,24
      Push    EDX
       Mov     EDX,EAX
       Dec     EDX
      Pop     EAX
      Mov     EBX,015h
      Call    ATA_Gate
      RetF
;###########################################################################
;# Funkce 26h - ATA Set Standby Mode Immediatly
;# Nastavuje mod HD na standby, okamzite
;###########################################################################
G_ATA_StandByImm       : ;026h
      Shr     EAX,24
      Mov     EDX,EAX
      Dec     EDX
      Mov     EBX,016h
      Call    ATA_Gate
      RetF
;###############################################################################
;# Procedura prevadi vstupni parametry do spravneho tvaru
;###############################################################################
;# Vstup  : EAX = disk,partitition
;#          EDX = LBA adresa od zacatku partitiony
;# Vystup : EAX = LBA adresa od zacatku disku
;#          EDX = disk
;#          EBX = 0 => Error, invalid input parameters
;###############################################################################
Count_LBA_And_Disk PROC
       ; obrana pred pageama, ktere jsou swaple
        PUSH    ES DS ECX
         PUSH  EAX EDI EDX
          Movzx ECX,Cl
          Dec   ECX
          Shr   ECX,3
          Inc   ECX
          Mov   EDX, ES
          LSL   EDX, EDX
@Count_LBA_And_DiskLoop :
          Cmp   EDX, EDI
          Ja    @Count_LBA_And_DiskLoop_cont
            POP  EDX EDI EAX
            Jmp  @Count_Invalid_Params
@Count_LBA_And_DiskLoop_cont :
          Mov   Al, Byte ptr ES:[EDI]
          Add   EDI,4096
          Dec   ECX
          Jnz   @Count_LBA_And_DiskLoop
         POP  EDX EDI EAX
         Jmp  @Count_LBA_And_Disk_Skip
Count_LBA_And_Disk_2 :
         PUSH    ES DS ECX
@Count_LBA_And_Disk_Skip :
         Mov    CX,Data_descriptor
         Mov    DS,CX
         Mov    CX,word ptr DS:[part_sel]
         Mov    ES,CX
         Mov    ECX,EAX
         Shr    ECX,24
         Cmp    ECX,4
         Ja     @Count_Invalid_Params
         Or     ECX,ECX
         Jz     @Count_Invalid_Params
         PUSH   ECX EDX  ;ECX = cislo disku
           Mov    EDX,EAX
           Shr    EDX,16
           And    EDX,0FFh  ;EDX = cislo partitiony
           Cmp    EDX,dword ptr DS:[Part_per_disk]
           Ja     @Count_Invalid_Params_1
           Or     EDX,EDX
           Jnz    @Count_Partition
            Xor    ECX,ECX
            Jmp    @Count_All_Disk ;nepracujeme s partitionou, ale s diskem :)
    @Count_Partition :
           Dec    EDX
           Shl    EDX,4     ;EDX = offset partitiony
           Mov    EAX,dword ptr DS:[Part_Per_Disk]
           Shl    EAX,4
           Sub    EDX,EAX
    @Count_Loop :
           Add    EDX,EAX
           Dec    ECX
           Jnz     @Count_Loop
          ;# v EDX je offset partitiony v segmentu ES
           Mov    Al, Byte ptr ES:[EDX.partition.sys_code]
           Or     Al,Al
           Jz     @Count_Invalid_Params_1 ;nemuzeme pracovat s part. se syskodem 0
           Mov    ECX,dword ptr ES:[EDX.partition.Relative_Beginning]
    @Count_All_Disk :
         POP    EAX EDX
         Dec    EDX
         Add    EAX,ECX
        POP     ECX DS ES
         RET
        POP     ECX DS ES
   @Count_Invalid_Params_1 :
         POP    EDX ECX
   @Count_Invalid_Params :
         Xor    EBX,EBX
         POP    ECX DS ES
         Ret
Count_LBA_And_Disk ENDP
;###############################################################################
;# Vystupy
;###############################################################################
@IDE_error_invalid_input_parameters :
      Xor   EAX,EAX
      Inc   Al
      RetF




