;############################################################################################################
;# Procedura uzamkne kernel
;############################################################################################################
kernel_Lock proc
        Call  Task_Switch_Disable
       Ret
kernel_Lock endp
;############################################################################################################
;# Procedura odemkne kernelu
;############################################################################################################
kernel_UnLock Proc
        Call  Task_Switch_Enable_X
       Ret
kernel_UnLock endp
;############################################################################################################
;# Procedure pro porovnavani dvou bufferu
;############################################################################################################
;# Vstup: DS:ESI - ukazatel na buffer 1
;#        ES:EDI - ukazatel na buffer 2
;#        ECX    - kolik bajtu z obou bufferu porovnat
;############################################################################################################
;# Vystup: CF = 1 - buffery jsou shodne
;#         CF = 0 - buffery jsou rozdilne
;############################################################################################################
CmpStr Proc
        or   ecx ,ecx
        jnz  short CmpStr_Continue
        clc
        ret
CmpStr_Continue:
       push  eax ebx edi esi
       pushfd
        ; testuji proti preteceni descriptoru
        mov  eax, ds
        lsl  eax, eax
        sub  eax, esi
        jc   short CmpStr_Total_False ; skoci, pokud je eax mensi nez esi
        mov  ebx, es
        lsl  ebx, ebx
        sub  ebx, edi
        jc   short CmpStr_Total_False ; skoci, pokud je eax mensi nez esi
        sub  eax, ecx
        jc   short CmpStr_Total_False ; skoci, pokud je eax mensi nez ecx
        sub  ebx, ecx
        jc   short CmpStr_Total_False ; skoci, pokud je ebx mensi nez ecx

       push  ecx
        shr  ecx, 2
        mov  eax, ecx
        shl  ecx, 2
       pop   ebx
        sub  ebx, ecx
        xchg ecx, eax
        cld
        rep  cmpsd
        jnz  short CmpStr_Total_False

        xchg ecx, ebx
        rep  cmpsb
        jnz  short CmpStr_Total_False

       popfd
        stc
        jmp  short CmpStr_end

CmpStr_Total_False:
       popfd
        clc
CmpStr_End:
       pop   esi edi ebx eax
CmpStr_Total_End:
        ret
CmpStr endp
;############################################################################################################
;# Procedura pro kopirovani
;############################################################################################################
;# Vstup: DS:ESI - vstupni buffer
;#        ES:EDI - vystupni buffer
;#        ECX    - kolik bajtu se ma prekopirovat
;############################################################################################################
;# Vystup: CF = 1 - operace probehla uspesne
;#         CF = 0 - prenos selhal
;############################################################################################################
Move Proc
        or   ecx ,ecx
        jnz  short move_Continue
        clc
        ret
move_Continue:
       push  eax ebx edi esi
       pushfd
        ; testuji proti preteceni descriptoru
        mov  eax, ds
        lsl  eax, eax
        sub  eax, esi
        jc   short move_Total_False ; skoci, pokud je eax mensi nez esi
        mov  ebx, es
        lsl  ebx, ebx
        sub  ebx, edi
        jc   short move_Total_False ; skoci, pokud je eax mensi nez esi
        sub  eax, ecx
        jc   short move_Total_False ; skoci, pokud je eax mensi nez ecx
        sub  ebx, ecx
        jc   short move_Total_False ; skoci, pokud je ebx mensi nez ecx

       push  ecx
        shr  ecx, 2
        mov  eax, ecx
        shl  ecx, 2
       pop   ebx
        sub  ebx, ecx
        xchg ecx, eax
        cld
        rep  movsd
        jnz  short move_Total_False

        xchg ecx, ebx
        rep  movsb
        jnz  short move_Total_False

       popfd
        stc
        jmp  short move_end

move_Total_False:
       popfd
        clc
move_End:
       pop   esi edi ebx eax
move_Total_End:
        ret
move endp
;############################################################################################################
;# Procedura pro kopirovani pcharu, kopiruje tad dlouho dokud nenarazi ve stupnim buf. na nulu nebo konec descriptoru
;############################################################################################################
;# Vstup: DS:ESI - vstupni buffer
;#        ES:EDI - vystupni buffer
;############################################################################################################
;# Vystup: CF = 1 - operace probehla uspesne
;#            = ECX - kolik bytu bylo prekopirovano
;#         CF = 0 - prenos selhal (retezec neni ukonceny,nebo je jeho delka vetsi nez velikost vystupniho bufferu)
;############################################################################################################
CopyStr Proc
       push  eax ebx edi esi
       pushfd
       push  ecx
        ; testuji proti preteceni descriptoru
        mov  eax, ds
        lsl  eax, eax
        sub  eax, esi
        mov  ebx, es
        lsl  ebx, ebx
        sub  ebx, edi
        cmp  eax, ebx
        jg   short CopyStr_ecx_ebx
        mov  ecx, eax
        jmp  short CopyStr_ecx
CopyStr_ecx_ebx:
        mov  ecx, ebx
CopyStr_ecx:
        ; pokusim se najit konec retezce a tim zjistit, jestli konec ma :)
       push  es edi ecx
        mov  ax, ds
        mov  es, ax
        mov  edi, esi
        xor  eax ,eax
        rep  scasb
        mov  eax, ecx
       pop   ecx edi es
        jnz  short CopyStr_Total_False
        sub  ecx, eax

       push  ecx
        Call Move
       pop   ecx
        jnc  short CopyStr_Total_False

       pop   eax ; puvodne ecx, ale to chceme nechat zmenene
       popfd
        stc
        jmp  short CopyStr_end

CopyStr_Total_False:
       pop   ecx
       popfd
        clc
CopyStr_End:
       pop   esi edi ebx eax
CopyStr_Total_End:
        ret
CopyStr endp
;##################################################################################
;# Procedura restartuje pocitac...
;##############################################################################
Restart  PROC
    ;Try official type - via PCI
      Mov   DX,0CF9h
      Xor   Al,Al
      Out   DX,Al
      Mov   Al,6
      Out   DX,Al
    ;Still not reseting ? OK, using Keyboard mode
      Mov   Al,0FEh
      Out   64h,Al
    ;Hmmm, why not we used PS2 mouse style ?
      In    Al,92h
      And   Al,0FEh
      Out   92h,Al
      Or    Al,1
      Out   92h,Al
    ;Hoops ? Not yet ? Tripple Fault !!!
      Push  0000h
      LIDT  SS:[ESP]
      Int   03h
      RET
Restart  ENDP
