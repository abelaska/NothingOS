;###########################################################################
;# Rutiny pro praci s CMOS
;###########################################################################

; Precte byte z CMOS pameti
; Vstup:  AL - adresa 00h..03Fh
; Vystup: AL - data
CMOS_Read Proc
        out   70h, al
        call  Wait_Loop_001ms
        in    al, 71h
        ret
CMOS_Read endp

; Zapise  byte do CMOS pameti
; Vstup:  AH - adresa 00h..03Fh
;         AL - Data
; Vystup: -
CMOS_Write Proc
        xchg  al, ah
        out   70h, al
        call  Wait_Loop_001ms
        xchg  al, ah
        out   71h, al
        ret
CMOS_Write endp

; Vrati datum komprimovany do Dwordu
; Vystup: EAX - komprimovany datum (00h-Rok(B)-Mesic(B)-Den(B))
CMOS_Packed_Date Proc
       push  ecx
        xor  ecx, ecx
        mov  al, CMOS_RTC_Year
        Call CMOS_Read
        mov  cl, al
        shl  ecx, 8
        mov  al, CMOS_RTC_Month
        Call CMOS_Read
        mov  cl, al
        shl  ecx, 8
        mov  al, CMOS_RTC_Day
        Call CMOS_Read
        mov  cl, al
        mov  eax, ecx
       pop   ecx
        ret
CMOS_Packed_Date Endp

; Vrati cas komprimovany do Dwordu
; Vystup: EAX - komprimovany cas (00h-Hodina(B)-Minuta(B)-Sekunda(B))
CMOS_Packed_Time Proc
       push  ecx
        xor  ecx, ecx
        mov  al, CMOS_RTC_Hour
        Call CMOS_Read
        mov  cl, al
        shl  ecx, 8
        mov  al, CMOS_RTC_Minute
        Call CMOS_Read
        mov  cl, al
        shl  ecx, 8
        mov  al, CMOS_RTC_Second
        Call CMOS_Read
        mov  cl, al
        mov  eax, ecx
       pop   ecx
        ret
CMOS_Packed_Time Endp

; ulozi do pameti datum a cas v komprimovane podobe
; Vstup:  ES:EDI - buffer pro ulozeni datumu a casu
; Vystup: -
CMOS_Get_Date_Time Proc
       push  eax
        call CMOS_Packed_Date
        cld
        stosd
        call CMOS_Packed_Time
        cld
        stosd
       pop   eax
        ret
CMOS_Get_Date_Time Endp