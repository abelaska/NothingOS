; Vstup: EBX - Cislo funkce (0..FFFFFFFEh)
Main_Kernel_Interrupt :
      PUSHFD
      CLI
      PUSH    DS
      PUSH    EAX
      PUSH    ESI
       cmp    ebx, 0FFFFFFFFh
       jz     short b1_end
       mov    esi, offset Int_B1_Functions
       mov    ax,data_descriptor
       mov    ds,ax
b1_Find:
       lodsd
       cmp     eax, 0FFFFFFFFh
       jz      short b1_end
       cmp     eax, ebx
       jz      short b1_found
       lodsd
       jmp     short b1_Find
b1_found:
       lodsd
       xchg    eax, ebx
       POP     ESI
       POP     EAX
       POP     DS
       POPFD
       call    ebx
       jmp     short B1_OK
B1_end:
       POP     ESI
       POP     EAX
       POP     DS
       POPFD
B1_ok:
       PUSHFD
       PUSH EAX
        mov  al,20h
        out  0a0h,al
        out  20h,al
       POP EAX
       POPFD
       Iret

Include .\..\inc\code\code32\INT\B1\B1fce.inc

