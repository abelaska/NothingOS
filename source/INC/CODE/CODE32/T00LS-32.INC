;##############################################################################
;# Jednotka, obsahujici pomocne podprogramy pro ladeni kernelu...
;# v ostre verzi nebude... chudinka jedna mala,...:)
;##############################################################################

; Vypise vsechny nactene drivery na obrazovku
Wr_Dir Proc
       pushad
       push    es
        call   VFS_Dir
        print  new_line_txt
Wr_Dir_loop:
        mov    eax, dword ptr es:[edi]
        or     eax, eax
        jz     short Wr_Dir_end
        wr_cons_nowait eax
        add    edi, 4
        call   kPrint
        print  new_line_txt
Wr_Dir_posun:
        cmp    byte ptr es:[edi], 0
        jz     short Wr_Dir_next
        inc    edi
        jmp    short Wr_Dir_posun
Wr_Dir_next:
        inc    edi
        jmp    short Wr_Dir_loop
Wr_Dir_end:
       pop     es
       popad
        ret
Wr_Dir Endp

; Vypise vsechny nactene drivery na obrazovku
Wr_LS_Drivers Proc
       pushad
       push    es
        call   DRV_Ls_Drivers
        print  new_line_txt
Wr_LS_Drivers_loop:
        movzx  eax, word ptr es:[edi]
        wr_cons_nowait eax
        add    edi, 2
        call   kPrint
        print  new_line_txt
Wr_LS_Drivers_posun:
        cmp    byte ptr es:[edi], 0
        jz     short Wr_LS_Drivers_next
        inc    edi
        jmp    short Wr_LS_Drivers_posun
Wr_LS_Drivers_next:
        inc    edi
        dec    ecx
        jnz    short Wr_LS_Drivers_loop
       pop     es
       popad
        ret
Wr_LS_Drivers Endp


;##############################################################################
;# Barva, kterou se vypisuje text na screen
;##############################################################################
default_color equ 15
;##############################################################################
;# Procedura Ceka na zmacknuti klavesy, libovolne...
;##############################################################################
WaitKey PROC
   Push  EAX
@WK_Looping1 :
   In    Al,60h
   Test  Al,80h
   Jnz   @WK_Looping1
   Mov   Ah,Al
@WK_Looping :
   In    Al,60h
   Or    Al,AH
   Jz    @WK_Looping
@WK_Looping2 :
   In    Al,60h
   Test  Al,80h
   Jz    @WK_looping2
   Pop   EAX
   Ret
WaitKey ENDP
;##############################################################################
;# Vypise na obrazovku 25 zaznamu z tabulky GDT, i s jejich selectory
;##############################################################################
;# Vstup :  ebx - cislo prvniho descriptoru
;#          edi - kde na obrazovce se zacne vypisovat... melo by byt od 0..160
;# Vystup : none
;# Nici   : none
;##############################################################################
Write_GDT PROC
     pushad
     push fs
     mov  edx,25
     Mov  ax,gdt_descriptor
;     mov  fs,ax
@GDT_lop :
     mov  eax,ebx
     shl  eax,3

     call hex2str_dd
     add  esi,4
     sub  ecx,4
     call write_text
     mov  al,' '
     stosw
     mov  eax,dword ptr fs:[(ebx*8)+4]
     call hex2str_dd
     call write_text
     mov  eax,dword ptr fs:[(ebx*8)]
     call hex2str_dd
     call write_text
     inc  ebx
     add  edi,160-42
     dec  edx
     jnz  @GDT_lop
     pop  fs
     popad
     ret
Write_GDT ENDP
;##############################################################################
;# Vypis tasku
;##############################################################################
write_tasks proc
 pushad
 push ds
    Mov     AX,Data_Descriptor
    Mov     DS,AX
    mov     ecx, dword ptr ds:[Current_Task_Num]
    Mov     DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
    xor     edx,edx
    print   family_info
@smycka :
    print   new_line_txt
    mov     eax,dword ptr ds:[edx.TSK_Info_ID]
    wr_cons eax
    mov     eax,dword ptr ds:[edx.TSK_Info_ParentTask]
    wr_cons eax
    mov     eax,dword ptr ds:[edx.TSK_Info_ChildTask]
    wr_cons eax
    mov     eax,dword ptr ds:[edx.TSK_Info_OlderBrother]
    wr_cons eax
    mov     eax,dword ptr ds:[edx.TSK_Info_YoungerBrother]
    wr_cons eax
    movzx   eax,word ptr ds:[edx.TSK_Info_Priority]
    wr_cons eax
    movzx   eax,word ptr ds:[edx.TSK_Info_Status]
    wr_cons eax
    mov    eax,dword ptr ds:[edx.TSK_Info_Variable]
    wr_cons eax
    add     edx,TSK_Info_Size
    dec     ecx
    jnz     @smycka
 pop ds
 popad
 Ret
write_tasks endp
;##############################################################################
;# Vypise text na obrazovku
;##############################################################################
;Vstup: DS:ESI - buffer s textem
;       ECX    - delka textu
;       EDI    - pozice pocatku vypisovani
;Vystup: EDI = EDI_input+ECX_input
;        ECX = 0
;##############################################################################
Write_Text Proc
       PUSH ES
       PUSH EAX
       PUSH ESI
        mov   ax, B8000_Descriptor
        mov   es, ax
        mov   ah,default_color
@Write_Text_Looping :
        lodsb
        stosw
        loop @Write_Text_Looping
       POP ESI
       POP EAX
       POP ES
        ret
Write_Text Endp
;##############################################################################
;# Vypise text na obrazovku danou barvou
;##############################################################################
;Vstup: DS:ESI - buffer s textem
;       ECX    - delka textu
;       EDI    - pozice pocatku vypisovani
;       Ah     - barva
;Vystup: EDI = EDI_input+ECX_input
;        ECX = 0
;##############################################################################
Write_Text_col Proc
       PUSH ES
       PUSH EAX
       PUSH EBX
       PUSH ESI
        mov   bx, B8000_Descriptor
        mov   es, bx
@Write_Text_Looping1 :
        lodsb
        stosw
        loop @Write_Text_Looping1
       POP ESI
       POP EBX
       POP EAX
       POP ES
        ret
Write_Text_Col Endp
;##########################################################################################
;# Vypise text zakonceny na nulu na obrazovku
;##########################################################################################
;Vstup: DS:ESI - buffer s textem
;       EDI    - pozice pocatku vypisovani
;Vystup: EDI = EDI_input+ECX_input
;##########################################################################################
Write_Pchar Proc
       PUSH ES
       PUSH EAX
       PUSH ESI
        mov   ax, B8000_Descriptor
        mov   es, ax
        mov   ah,default_color
@Write_Pchar_Looping:
        lodsb
        or   al,al
        jz   @Write_Pchar_End
        stosw
        jmp @Write_Pchar_Looping
@Write_Pchar_End:
       mov al,'X'
       mov ah,15
       stosw
       POP ESI
       POP EAX
       POP ES
        ret
Write_Pchar Endp
;##########################################################################################
;# Vypise text zakonceny na nulu na obrazovku
;##########################################################################################
;Vstup: DS:ESI - buffer s textem
;       EDI    - pozice pocatku vypisovani
;       AH - color
;Vystup: EDI = EDI_input+ECX_input
;##########################################################################################
Write_Pchar_Col Proc
       PUSH ES
       PUSH EAX
       PUSH ESI
       PUSH ECX
        mov   cx, B8000_Descriptor
        mov   es, cx
@Write_Pchar_Looping_C:
        lodsb
        or   al,al
        jz   @Write_Pchar_End_c
        stosw
        jmp @Write_Pchar_Looping_c
@Write_Pchar_End_c:
       POP ECX
       POP ESI
       POP EAX
       POP ES
        ret
Write_Pchar_Col Endp

