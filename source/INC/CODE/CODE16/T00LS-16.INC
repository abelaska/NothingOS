;#################################################################################
;# Rutinky pro zobrazovani textu na obrazovce v 16-bitovem prostredi
;#################################################################################

;16-bitove verze

; vymaze obrazovku
Clrscr_16:
        xor   edi, edi
        mov   ax, 0b800h
        mov   es, ax
        mov   ax, 0720h
       PUSH AX
        mov   cl, 16
        shl   eax, cl
       POP AX
        mov   ecx, (80*30*2)/4
        cld
        rep   stosd
        ret

;##############################
;# Vypise text na obrazovku
;##############################
;Vstup: DS:ESI - buffer s textem
;       ECX    - delka textu
;       EDI    - pozice pocatku vypisovani
;Vystup: EDI = EDI_input+ECX_input
;        ECX = 0
;##############################
Write_Text_16 Proc
       PUSH ES
       PUSH EAX
       PUSH ESI
        mov   ax, 0B800h
        mov   es, ax
        mov   ah,02
@Write_Text_Looping_16 :  lodsb
        stosw
        loop @Write_Text_Looping_16
       POP ESI
       POP EAX
       POP ES
        ret
Write_Text_16 Endp

;##############################
;# Vypise text zakonceny na nulu na obrazovku
;##############################
;Vstup: DS:ESI - buffer s textem
;       EDI    - pozice pocatku vypisovani
;Vystup: EDI = EDI_input+ECX_input
;##############################
Write_Pchar_16 Proc
       PUSH ES
       PUSH EAX
       PUSH ESI
        mov   ax, 0B800h
        mov   es, ax
        mov   ah,02
@Write_Pchar_Looping_16:
        lodsb
        or al, al
        jz @Write_Pchar_End_16
        stosw
        loop @Write_Pchar_Looping_16
@Write_Pchar_End_16:
       POP ESI
       POP EAX
       POP ES
        ret
Write_Pchar_16 Endp
