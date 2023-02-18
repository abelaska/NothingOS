;############################################################################
;# Jednotka pro praci s radici preruseni
;#################################################################################

;######################################################################
;# Procedura PIC_MapIRQ2INT mapuje IRQ preruseni na INT
;# Vsech 16 IRQ preruseni se mapuji za sebou, tzn obsadi cely jedem
;# block => napr. 0f0h..0ffh
;# preruseni musi zacinat na adrese delitelne 16-ti, beze zbytku :)
;######################################################################
;# Vstup  : DL = offset v tabulce preruseni, kde budou IRQ
;#          DL se zarovnava na 16, tzn muze nabyvat
;#          hodnot 00h,10h,20h,..F0h.
;# Vystup : neni
;# Nici registery : zadne
;######################################################################
PIC_MapIRQ2INT PROC
        Pushad
        cli
; nastaveni radice cislo 1

        xor  al, al
        out  0F1h, al         ; reset koprocesoru
        mov  al, 11h          ; ICW1 - (potreba ICW4, kaskadni zapojeni,
        out  20h,al           ; interval vektoru preruseni 8, rezim spousteni
        call @@PIC_WAIT0001   ; hranou)

        and  dl, 0F0h          ; zajistime si, aby addresa byla zarovnana na 16
        mov  al, dl           ; ICW2 - Bazova adresa, kde zacinaji vektory
        out  21h, al          ; preruseni pro IRQ - zarovnane na osm :)
        call @@PIC_WAIT0001

        mov  al, 04h          ; ICW3 - na IRQ2 pripojeny podrizeny radic
        out  21h, al
        call @@PIC_WAIT0001

        mov  al, 01h           ; ICW4 - rezim 86/88, normalni konec preruseni,
        out  21h, al           ; nebuferovany rezim
        call @@PIC_WAIT0001


;nastaveni radice cislo 2

        mov  al, 11h           ; ICW1 - (potreba ICW4, kaskadni zapojeni,
        out  0A0h,al           ; interval vektoru preruseni 8, rezim spousteni
        call @@PIC_WAIT0001    ; hranou)

        add  dl,8             ; adresa zarovnana na osm
        mov  al, dl           ; ICW2 - Bazova adresa, kde zacinaji vektory
        out  0A1h, al          ; preruseni pro IRQ - zarovnane na osm :)
        call @@PIC_WAIT0001

        mov  al, 02h          ; cislo podrizeneho radice = 0
        out  0A1h, al
        call @@PIC_WAIT0001

        mov  al, 01h           ; ICW4 - rezim 86/88, normalni konec preruseni,
        out  0A1h, al           ; nebuferovany rezim
        call @@PIC_WAIT0001

@Pic_End :

        Sti
        Popad
        Ret

@@PIC_Wait0001 :
       jmp  @PIC_Delay0
@PIC_Delay0:jmp  @PIC_Delay1
@PIC_Delay1:jmp  @PIC_Delay2
@PIC_Delay2:jmp  @PIC_Delay3
@PIC_Delay3:jmp  @PIC_Delay4
@PIC_Delay4:jmp  @PIC_Delay5
@PIC_Delay5:ret
PIC_MapIRQ2Int   ENDP
;############################################################################
;# Procedura zakazuje veskera hardwareova preruseni IRQ0..15
;# Vstup  : neni
;# Vystup : neni
;# Nici registery : zadne
;############################################################################
PIC_Disable_All_IRQ  Proc
          Push  Ax
          Mov   Al,0FFh
          Out   021h,Al
          Out   0A1h,Al
          Pop   Ax
          Ret
PIC_Disable_All_IRQ  ENDP
;############################################################################
;# Procedura nastavuje pocatecni konfiguraci IRQ kernelu
;# Vstup  : neni
;# Vystup : neni
;# Nici registery : zadne
;############################################################################
IRQ_Initializing PROC
        Pushad
        ; namapovani IRQ na int F0..FF
        Mov     Dl,0F0h
        Call    PIC_MapIrq2Int
        ;radic preruseni c.1 - povolime vse krome klavesnice
        Mov     Al,00000010b
        Out     21h,Al
        ;radic preruseni c.2 - povolime HD
        Mov     Al,00011111b
        Out     0A1h,Al
        Popad
        Ret
IRQ_Initializing ENDP