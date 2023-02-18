;###########################################################################
;# ATA Support for Kernel
;#################################################################################
;# Adaptovano na 32.bit rezim :)
;# Uprava na pouziti s IRQ
;#################################################################################

;###########################################################################
;# Vstupni bod ATA_Support
;###########################################################################
;# EDX - cislo discu
;# EBX - cislo sluzby
;# ES : EDI --> zdrojovy/cilovy buffer
;###########################################################################
ATA_Gate     Proc
             Push   EDX
             Push   EDI
             Push   ESI
             Push   DS
             Push   ES
             PUSH   EAX
              Mov    AX,Data_descriptor
              Mov    DS,AX
              Inc    Byte ptr DS:[Task_Switching_Disabled]
             POP    EAX
             CLD
             Shl    EDX,2
             Add    EDX,Offset IDE_TABLE_PTRs ;ukazatel na structuru IDE_DiscX
             Push   EDI
              Mov    EDI,dword ptr DS:[EDX]
              Mov    Dx,Word ptr Ds:[EDI]         ;Nastavime Basi
              Mov    Byte ptr DS:[IDE_Flags],0
              Push   EAX
               Mov    Ah,Byte ptr DS:[EDI+3]
               Shl    Ah,6
               And    Ah,40h
               Mov    Al,Byte ptr DS:[EDI+2]
               Shl    Al,4
               Shl    Al,4
               And    Al,10h
               Or     Al,Ah
               Or     Al,10100000b
               Add    Dl,6
               Out    Dx,Al                     ;Nastavime primary/secondary
               Sub    Dl,6                      ;a adresovani LBA/CHS
              Pop    EAX
             POP    EDI
             Shl    EBX, 2    ; vynasobime EBX*4 --> ukazatel v tabulce
             Add    EBX,Offset ATA_Table
             Jmp    DS:[EBX]



;###########################################################################
;# Macra
;###########################################################################
IDE_Macro_Wait MACRO
             Mov    Al,Byte ptr DS:[IDE_Flags]
             Test   Al,0FFh
             Jz     $-7 ;=jz @@IDE_Wait0001
             Mov    Byte ptr DS:[IDE_Flags],0
             Test   Al,01
             Jnz    @ATA_Error
ENDM
;###########################################################################
IDE_Macro_Wait_Write Proc
 @IDE_Macro_Wait_Write1 :
             Mov    Al,Byte ptr ES:[IDE_Flags]
             Test   Al,0FFh
             Jz     @IDE_Macro_Wait_Write1
             Mov    Byte ptr ES:[IDE_Flags],0
             Test   Al,01
             Jnz    @ATA_Error_WaitWrite
            Ret
IDE_Macro_Wait_Write ENDP
;###########################################################################
IDE_Macro_Write_Prepare MACRO
             Add   EDX,206h
             In    Al,DX
             Test  Al,80h
             Jnz   $-5
             Sub   EDX,206h
             Test  Al,1
             Jnz   @ATA_Error
             Push  DS
             Mov   Bx,ES
             Mov   Ds,BX
             Pop   ES
             Mov   ESI,EDI
EndM
;###########################################################################
;# Execute Drive Diagnostics
;# Diagnosticka sluzba, v ERROR registeru vysledek
;###########################################################################
;# Vstup  : none
;# Vystup : v Error Registreru ulozeny informace = BL
;###########################################################################
ATA_DriveDiagnostics :
             Add    Dl,007h
             Mov    Al,090h
             Out    Dx,Al
             IDE_Macro_Wait
             In     Al,Dx
             Mov    Bl,Al
             Jnz    @ATA_Error
             Jmp    @ATA_Finish
;###########################################################################
;# Format Track
;# Formatovani stopy
;###########################################################################
;# Vstup : EAX = Adresa stopy = 16-ti bitova
;#         Cl  = pocet sektoru pro format
;#         v ES information o formatu... viz tab na str 40 ATA manualu
;###########################################################################
ATA_FormatTrack :
             Add   Dl,004h ;nastavime adresu stopy - cylinder low a high
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Mov   Al,Cl  ; nastavime pocet sektoru pro formatovani
             Sub   Dl,003h
             Out   Dx,Al
             Add   Dl,005h; command Format
             Mov   Al,50h
             Out   Dx,Al
             Sub   Dl,7
             IDE_Macro_Write_Prepare
             Mov   ECX,128
             Rep   Outsd
             Call  IDE_Macro_Wait_Write
             Jmp   @ATA_End
;###########################################################################
;# Initialize Drive Parameters
;# Nastaveni cisla Sektoru na stopu a poctu hlav
;###########################################################################
;# Vstup : Al = sektoru na stopu
;#         Ah = cislo (hlav-1)
;###########################################################################
ATA_InitDriveParam :
             Add   Dl,02h
             Out   Dx,Al
             Add   Dx,004h
             In    Al,Dx
             And   Ah,0Fh
             Or    Al,Ah
             Out   Dx,Al
             Inc   Dl
             Mov   Al,091h
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# ATA Read Bufffer
;# Precte 512 byte z Bufferu
;###########################################################################
;# Vstup  : neni
;# Vystup : v datovem segmentu na selectoru ES ulozeno 512 bytu z bufferu
;###########################################################################
ATA_ReadBuffer :
             Add   Dl,07h
             Mov   Al,0E4h
             Out   Dx,Al
             IDE_Macro_Wait
             Mov   ECX,128
             Sub   Dl,07h
             Rep   Insd
             Jmp   @ATA_End
;###########################################################################
;# Read Long
;# Cteni 512 byte data, nasleduje x ECC byte
;###########################################################################
;# Vstup : EAX = addressa
;#          Cl = 0 - s opakovani
;#               1 - bez opakovani
;###########################################################################
ATA_ReadLong :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Shr   EAX,16
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,1
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,22h
             And   Cl,1
             Or    Al,Cl
             Out   Dx,Al
             IDE_Macro_Wait
             Sub   Dl,07
             Mov   ECX,130
             Rep   Insd
             Jmp   @ATA_End
;###########################################################################
;# Set Multiple Mode
;# Nastavuje, sektoru bude v bloku, ktery bude cist/psat ReadMultiple
;# nebo WriteMultiple command. Zavisi na velikosti bufferu. Viz DriveInfo
;###########################################################################
;# Vstup : Cl = Sektoru v bloku
;###########################################################################
ATA_SetMultipleMode :
             Mov   Al,Cl
             Add   Dl,2
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,0C6h
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @Ata_End
;###########################################################################
;# Read Multiple
;# Cte bez preruseni tolik sektoru, kolik je udano v CL (a v predchazejici
;# fci SetMultipleMode)
;###########################################################################
;# Vstup : EAX = addressa
;#         Cl  = pocet sektoru v bloku
;#             = musi byt stejne jako hodnota Sektoru v Bloku fce SetMultipleMode
;###########################################################################
ATA_ReadMultiple :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Shr   EAX,16
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,Cl
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,0C4h
             Out   Dx,Al
             IDE_Macro_Wait
             Sub   Dl,07
             Xor   EAX,EAX
             Mov   Al,128
             Mul   Cl
             Mov   ECX,EAX
             Rep   InsD
             Jmp   @ATA_End
;###########################################################################
;# Read Sectors
;# Cte tolik sektoru, kolik je udano v CL. Mezi nactenim kazdeho sektoru
;# musi cekat, nez pro nej HD pripravi data => pomalejsi nez Multiple :)
;###########################################################################
;# Vstup : EAX = addressa
;#         Cl  = pocet sektoru k precteni
;#         Ch  = 0 - s opakovanim
;#             = 1 - bez opakovani
;###########################################################################
ATA_ReadSectors :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Shr   EAX,16
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,CL
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,020h
             And   Ch,001h
             Or    Al,Ch
             Mov   Ah,Cl
             Out   Dx,Al
             Sub   Dl,7
@@IDE_Wait0007:
             IDE_Macro_Wait
             Mov   ECX,128
             Rep   Insd
             Dec   Ah
             Jnz   @@IDE_Wait0007
             Jmp   @ATA_End
;###########################################################################
;# Read Verify Sectors
;# proveruje tolik sektoru, kolik je v Cl. Je-li chyba, pak pouzit
;# ATA_GetCurrentPos pro zjisteni, na jakem sektoru vznikla chyba
;###########################################################################
;# Vstup : EAX = addressa
;#         Cl  = pocet sektoru k provereni
;#         Ch  = 0 - s opakovanim
;#             = 1 - bez opakovani
;###########################################################################
ATA_VerifySectors :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             Shr   EAX,16
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,cl
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,040h
             And   Ch,01h
             Or    Al,Ch
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# Get Position
;# Navraci addresu ulozenou v dane chvili v registerech
;###########################################################################
;# Vstup  : none
;# Vystup : EBX = Addresa
;###########################################################################
ATA_GetCurrentPos :
             Add   Dl,06h
             Xor   EAX,EAX
             In    Al,Dx
             And   Al,0Fh
             Shl   EAX,8
             Dec   Dl
             In    Al,Dx
             Shl   EAX,8
             Dec   Dl
             In    Al,Dx
             Shl   EAX,8
             Dec   Dl
             In    Al,Dx
             Mov   EBX,EAX
             Jmp   @ATA_Finish
;###########################################################################
;# Recalibrate
;# Nastavuje hlavu na 0 pozici... neli li 0 nalezena => Track Not Found error
;###########################################################################
;# Vstup  : Al - rychlost presunu - <0..0F>
;###########################################################################
ATA_Recalibrate :
             Add   Dl,07
             And   Al,0Fh
             Or    Al,010h
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# Seek
;# Nastavuje hlavu na pozici udanou v EAX rychlosti v Cl
;###########################################################################
;# Vstup  : Cl  - rychlost presunu - <0..0F>
;#          EAX - addresa
;###########################################################################
ATA_Seek :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             Shr   EAX,16
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Inc   Dl
             And   Cl,0Fh
             Mov   Al,070h
             Or    Al,Cl
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# Set Feauture
;# nastaveni urcitich parametru... ATA reference manuall 5.4.14
;###########################################################################
;# Vstup  : Al - parametr
;###########################################################################
ATA_SetFeauture :
             Inc   Dl
             Out   Dx,Al
             Add   Dl,06
             Mov   Al,0EFh
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# ATA Write Buffer
;# Zapise 512 byte do Bufferu
;###########################################################################
;# Vstup : v datovem segmentu na selectoru ES ulozeno 512 bytu z bufferu
;###########################################################################
ATA_WriteBuffer :
             Add   Dl,07h
             Mov   Al,0E8h
             Out   Dx,Al
             Sub   Dl,07h
             IDE_Macro_Write_Prepare
             Mov   ECX,128
             Rep   Outsd
             Call  IDE_Macro_Wait_Write
             Jmp   @ATA_End
;###########################################################################
;# ATA Write Long
;# Zapise 520 byte na adressu, 512 dat a 8 byte ECC bitu..
;###########################################################################
;# Vstup : Cl  - 0 = s opakovanim
;#             - 1 = bez opakovani
;#         EAX - Addresa
;###########################################################################
ATA_WriteLong :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Shr   Al,16
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,1
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,32h
             And   Cl,1
             Or    Al,Cl
             Out   Dx,Al
             Sub   Dl,07
             IDE_Macro_Write_Prepare
             Mov   ECX,130
             Rep   OutsD
             Call  IDE_Macro_Wait_Write
             Jmp   @ATA_End
;###########################################################################
;# ATA Write Multiple
;# Zapise Cl-sektoru bez vyruseni, Cl = nastaveni z fce ATA_SetMultiple
;###########################################################################
;# Vstup : Cl - pocet sektoru v bloku
;#         EAX - addresa
;###########################################################################
ATA_WriteMultiple :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Shr   EAX,16
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,Cl
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,0C5h
             Out   Dx,Al
             Sub   Dl,7
             IDE_Macro_Write_Prepare
             Xor   EAX,EAX
             Mov   Al,128
             Mul   Cl
             Mov   ECX,EAX
             Rep   OutsD
             Call  IDE_Macro_Wait_Write
             Jmp   @ATA_End
;###########################################################################
;# ATA Write sektors
;# Zapise Cl-sektoru - sektor po sektoru
;###########################################################################
;# Vstup : Cl  - pocet sektoru
;#         Ch  - 0 - s opakovanim
;#               1 - bez opakovani
;#         EAX - addresa
;###########################################################################
ATA_WriteSectors :
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Shr   EAX,16
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,cl
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,030h
             And   Ch,01h
             Or    Al,Ch
             Mov   Ah,Cl
             Out   Dx,Al
             Sub   Dl,7

             IDE_Macro_Write_Prepare
@@IDE_Wait000A:
             Mov   ECX,128
             Rep   Outsd
             Call  IDE_Macro_Wait_Write
             Test  Al,08h
             Jz    @ATA_End
             Dec   Ah
             Jnz   @@IDE_Wait000A
             Jmp   @ATA_End
;###########################################################################
;# ATA Check Power Mode
;# zjistiju aktualni mod HD
;###########################################################################
;# Vystup   : Al - mod
;###########################################################################
ATA_CheckPowerMode :
             Mov   Al,0E5h
             Add   Dl,7
             Out   Dx,Al
             IDE_Macro_Wait
             Sub   Dl,5
             In    Al,Dx
             Jmp   @ATA_Finish
;###########################################################################
;# ATA Set Idle Mode
;# Nastavuje mod HD na idle
;###########################################################################
;# Vstup   : Al - cas, po jehoz vyprseni se prepne do Idle Mode (x5s)
;###########################################################################
ATA_SetIDleMode :
             Add   Dl,2
             Out   Dx,Al
             Mov   Al,097h
             Add   Dl,5
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# ATA Set Idle Mode Immediatly
;# Nastavuje mod HD na idle, okamzite
;###########################################################################
ATA_IdleImmediatly :
             Mov   Al,095h
             Add   Dl,7
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# ATA Sleep
;# Nastavuje mod HD na sleep -> jediny navrat = reset :)
;###########################################################################
ATA_Sleep :  Mov   Al,099h
             Add   Dl,7
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# ATA Set Standby Mode
;# Nastavuje mod HD na standby
;###########################################################################
;# Vstup   : Al - cas, po jehoz vyprseni se prepne (x5s)
;###########################################################################
ATA_Standby :Add   Dl,2
             Out   Dx,Al
             Mov   Al,0E2h
             Add   Dl,5
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# ATA Set Standby Mode Immediatly
;# Nastavuje mod HD na standby, okamzite
;###########################################################################
ATA_StandByImm:
             Out   Dx,Al
             Mov   Al,0E0h
             Add   Dl,7
             Out   Dx,Al
             IDE_Macro_Wait
             Jmp   @ATA_End
;###########################################################################
;# Vystupni brana
;###########################################################################
@ATA_Error_WaitWrite :
             Pop   EDX
@ATA_Error : And   Dl,0F0h   ; ziskame zpatky basovou adressu
             Inc   Dl
             In    Al,Dx
             Mov   Bl,Al
             Add   Dl,6
             In    Al,Dx
             Mov   Bh,Al
             Mov   EAX,2
             Jmp   @ATA_Finish
@ATA_End   : Xor   EBX,EBX
             Xor   EAX,EAX
@ATA_Finish :
              Mov    EDI, Data_Descriptor
              Mov    DS,EDI
              Dec    Byte ptr DS:[Task_Switching_Disabled]
             Pop    ES
             Pop    DS
             Pop   ESI
             Pop   EDI
             Pop   EDX
             Ret
ATA_GATE Endp
