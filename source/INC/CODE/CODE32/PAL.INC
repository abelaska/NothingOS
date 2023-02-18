;###########################################################################
;# Rutiny pro praci s paletou
;###########################################################################

; zinicializuje defaultni paletu
Pallete_Init Proc
      push   es eax edi
       mov   ax, data_descriptor
       mov   es, ax
       mov   edi, offset Default_Pallete
       call  GetPallete
      pop    edi eax es
       ret
Pallete_Init endp

; nastavi paletu
; Vstup:  ES:EDI - ukazatel na buffer s ulozenyma slozkama barev
; Vystup: CF = 0 - OK
;         CF = 1 - Error
SetPallete Proc
       pushad
       push  ds es
        mov  ecx, es
        lsl  ecx, ecx
        sub  ecx, edi
        jc   short SetPallete_False ; skoci, pokud je eax mensi nez esi
        sub  ecx, 256*3
        jc   short SetPallete_False ; skoci, pokud je eax mensi nez esi

       push  es
       pop   ds
        mov  esi, edi

        mov  ecx, 256
        xor  ebx, ebx
SetPallete_Loop:
        mov  dx, 3c8h
        mov  al, bl
        out  dx, al
        inc  dx
        lodsb ;RedValue
        out  dx, al
        lodsb ;GreenValue
        out  dx, al
        lodsb ;BlueValue
        out  dx, al
        inc  ebx
        dec  ecx
        jnz  short SetPallete_Loop
        clc
        jmp  short SetPallete_End
SetPallete_False:
        stc
SetPallete_End:
       pop   es ds
       popad
        ret
SetPallete Endp

; precte paletu
; Vstup:  ES:EDI - ukazatel na buffer, do ktereho se maji ulozit slozky barev
; Vystup: CF = 0 - OK
;         CF = 1 - Error
GetPallete Proc
       pushad
       pushfd
       cli
        mov  ecx, es
        lsl  ecx, ecx
        sub  ecx, edi
        jc   short GetPallete_False ; skoci, pokud je eax mensi nez esi
        sub  ecx, 256*3
        jc   short GetPallete_False ; skoci, pokud je eax mensi nez esi

        mov  ecx, 256
        xor  ebx, ebx
GetPallete_Loop:
        mov  dx, 3c7h
        mov  al, bl
        out  dx, al
        inc  dx
        inc  dx
        in   al, dx
        stosb ;RedValue
        in   al, dx
        stosb ;GreenValue
        in   al, dx
        stosb ;BlueValue
        inc  ebx
        dec  ecx
        jnz  short GetPallete_Loop
        clc
        jmp  short GetPallete_End
GetPallete_False:
        stc
GetPallete_End:
       popfd
       popad
        ret
GetPallete Endp
