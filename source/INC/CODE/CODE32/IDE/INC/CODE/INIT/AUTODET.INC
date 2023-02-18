;########################################################################
;# Procedura nastavuje zakladni parametry hardisku. Musi byt volana pred
;# jakoukoliv procedurou ze souboru Ata.inc...
;########################################################################
;# Vstup  : DS = Datovy segment, ve kterem jsou umisteny struktury
;# Vystup : V promennych Ide_Disk0..Ide_Disk3 jsou nastaveny spravne parametry
;# Nici   : Nic :)
;########################################################################
IDE_Initializing Proc
        Pushfd
        Pushad
        Push   ES
        Push   DS
        Push   DS
        Pop    ES
        Xor    ECX,ECX
        Mov    EDI,Offset IDE_Buffer ;EDI - buffer
        Mov    EDX,01F0h             ;zacneme s primarnim radice,
        ;inicializace IDE primary master
        Call   IDE_AutoDetectionDrive
        Test   Bl,0FFh
        Jz     @IDE_Next01
        Mov    Byte ptr DS:[IDE_Drive0.ATA_ATAPI],Bl
        Mov    EBX,Offset IDE_Drive0
        Call   IDE_Setting_ATA
@IDE_Next01 :
        ;inicializace IDE primary slave
        Mov    Cl,1
        Mov    EDI,Offset IDE_Buffer ;EDI - buffer
        Call   IDE_AutoDetectionDrive
        Test   Bl,0FFh
        Jz     @IDE_Next02
        Mov    Byte ptr DS:[IDE_Drive1.ATA_ATAPI],Bl
        Mov    EBX,Offset IDE_Drive1
        Call   IDE_Setting_ATA
@IDE_Next02 :
        ;inicializace IDE secondary master
        Mov    EDX,0170h  ; prejdeme na sekundarni radic
        Xor    Cl,Cl
        Mov    EDI,Offset IDE_Buffer ;EDI - buffer
        Call   IDE_AutoDetectionDrive
        Test   Bl,0FFh
        Jz    @IDE_Next03
        Mov    Byte ptr DS:[IDE_Drive2.ATA_ATAPI],Bl
        Mov    EBX,Offset IDE_Drive2
        Call   IDE_Setting_ATA
@IDE_Next03 :
       ;inicializace IDE secondary slave
        Mov    Cl,1
        Mov    EDI,Offset IDE_Buffer ;EDI - buffer
        Call   IDE_AutoDetectionDrive
        Test   Bl,0FFh
        Jz     @IDE_Next04
        Mov    Byte ptr DS:[IDE_Drive3.ATA_ATAPI],Bl
        Mov    EBX,Offset IDE_Drive3
        Call   IDE_Setting_ATA
@IDE_Next04 :
        Pop    DS
        Pop    ES
        Popad
        Popfd
        Ret
IDE_Initializing ENDP
;############################################################################
;# Procedura, ktera nastavi data v IDE_DiskX na spravne informace
;############################################################################
IDE_Setting_ATA Proc
        Pushad
        Push   ES
        Push   DS
        Push   Ds
        Push   ES
        Pop    Ds
        Pop    ES
        Mov    ESI,EDI
        Mov    EDI,EBX
        ;LBA/CHS
        Mov    Ax,word ptr DS:[ESI+(49*2)]
        Shr    Ax,9
        And    Al,1
        Mov    Byte ptr DS:[EDI.LBA_CHS],Al
        Test   Al,1
        Jnz    @IDE_GetLBASize
        ;CHS Size
        Mov    Ax,Word ptr DS:[Esi+2]  ;pocet cylinderu
        Mov    Word ptr DS:[Edi.CHS_Cyl],AX
        Mov    Bx,Word ptr DS:[Esi+6]  ;pocet hlav
        Mov    Word ptr DS:[EDI.CHS_Heads],Bx
        Mul    BX
        Xor    EBX,EBX
        Mov    Bx,Word ptr DS:[Esi+12] ;pocet sectoru
        Mov    Word ptr DS:[EDI.CHS_Sectors],BX
        Mul    EBX
        Jmp    @IDE_StoreSize
@IDE_GetLBASize :
        ;LBA Size
        Mov    EAX,Dword ptr DS:[ESI+(60*2)]
@IDE_StoreSize :
        Mov    EDX,EAX
        Shr    EDX,16
        Mov    BX,2048
        Div    BX
        Mov    Word ptr DS:[EDI.DiskSize],AX
        ;DMA_Support
        Mov    Ax,word ptr DS:[ESI+(49*2)]
        Shr    Ax,8
        And    Al,1
        Mov    Byte ptr DS:[EDI.DMA_Support],Al
        ;LBA max
        Mov    EAX,Dword ptr DS:[ESI+(60*2)]
        Mov    Dword ptr DS:[EDI.LBA_Max],EAX
        ; ulozime jmeno disku
        Push   ESI EDI
        Mov    CX,20
@IDE_Setting_Loop01  :
        Mov    Ax,word ptr DS:[ESI+(27*2)]
        Xchg   Al,AH
        Mov    Word ptr DS:[EDI.Drive_Name],Ax
        Inc    EDI
        Inc    EDI
        Inc    ESI
        Inc    ESI
        Loop   @IDE_Setting_Loop01
        POP    EDI ESI
        ;ulozime seriove cislo
        Mov    CX,19
@IDE_Setting_Loop02  :
        Mov    Al,Byte ptr DS:[ESI+(10*2)]
        Mov    Byte ptr DS:[EDI.Serial],Al
        Inc    EDI
        Inc    ESI
        Loop   @IDE_Setting_Loop02

        Pop    DS
        Pop    ES
        Popad
        Ret
IDE_Setting_ATA Endp
;###########################################################################
;# Auto Detekce Disku
;###########################################################################
;# EDX = Port base
;# Cl  = Disk - 0=Master, 1=Slave
;# ES:EDI = ukazatel na 512 bytovy buffer
;# DS - Datovy segment
;###########################################################################
IDE_AutoDetectionDrive Proc
         Push   EDI
         Push   EDX
         Push   DX
         Xor    Al,Al
         Add    DX,206h
         Out    Dx,Al
         Pop    DX
         Mov    Byte ptr DS:[IDE_Flags],0
         Add    Dl,6     ; nastaveni master/slave
         Shl    Cl,4
         In     Al,Dx
         And    Al,11100000b
         Or     Al,Cl
         Out    Dx,Al
         Inc    Dl
         In     Al,DX
         Cmp    Al,0FFh
         Je     @IDE_Error
         Test   Al,0FFh
         Jz     @IDE_Error
         Sub    Dl,4     ;chceme nacist 1 sektor dat
         Mov    Al,1
         Out    Dx,Al
         In     Al,DX
         Dec    Al
         Jnz    @IDE_Error
         Add    Dl,4     ;ATA auto Identifikace
         Mov    Al,0ECh
         Out    Dx,Al
         Mov    Dword ptr DS:[Timer_Hook_ATA],Initializing_Delay
@IDEWait1 :
          Cmp    Dword ptr DS:[Timer_Hook_ATA],0
          Je     @IDE_Error
          Mov    Al,Byte ptr DS:[IDE_Flags]
          Test   Al,0FFh
         Jz     @IDEWait1
         Xor    Ah,Ah
         Mov    Byte ptr DS:[IDE_Flags],Ah
         Test   Al,1
         Jnz    @IDE_ATAPI
         Test   Al,8     ; jesli nejsou potrebna data, pak error...
         JZ     @IDE_Error

         Mov    Bl,1     ; buffer je naplneny daty
         Jmp    @IDE_Cont

@IDE_ATAPI :
         Mov    Al,0A1h
         Out    Dx,Al
         Mov    Dword ptr DS:[Timer_Hook_ATA],Initializing_Delay
@IDEWait2 :
         Cmp    Dword ptr DS:[Timer_Hook_ATA],0
         Je     @IDE_Error
         Mov    Al,Byte ptr DS:[IDE_Flags]
         Test   Al,0FFh
         Jz     @IDEWait2
         Xor    Ah,Ah
         Mov    Byte ptr DS:[IDE_Flags],Ah
         Test   Al,1
         Jnz    @IDE_Error
         Test   Al,8
         Jz     @IDE_Error
         Mov   Bl,2
@IDE_Cont :
         And  Dl,0F0h ; chceme zpet ziskat basi
         Mov  ECX,128
         Rep  Insd
         Jmp  @IDE_End
@IDE_Error :
         Xor  Bx,Bx
@IDE_End :
         And  EBX,0FFh
         Pop  EDX
         POP  EDI
         Ret
IDE_AutoDetectionDrive ENDP
