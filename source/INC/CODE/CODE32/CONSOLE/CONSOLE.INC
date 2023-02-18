; Zarizeni pro virtualni konzoli, budou znakova zarizeni tzn. ze se tam bude
; zapisovat po znaku, jeste to promyslim

Console_SetAddr Macro cil,zdroj
       mov  dword ptr ds:[esi+cil], offset zdroj
       mov  word ptr ds:[esi+cil+4], cs
endm

; zinicializuje konzoly
Consoles_Init Proc
       push   ds es eax ecx edi
        mov   ax, Data_descriptor
        mov   ds, ax

        ; alokuje pamet pro FS_Infos
        mov   eax, Max_Console_Count*Console_Size
        call  kAlloc
        mov   word ptr ds:[Sel_Consoles], ax
        ; vynuluje danou pamet
        mov   es, ax

        ; zinicializuje tabulku procedur pro praci s konzoli
        mov   ax, data_descriptor
        mov   ds, ax
        mov   byte ptr ds:[console_initialized],1
        mov   esi, offset Console_Proc_Tab
        Console_SetAddr Console_Proc_Write,Console_Write
        Console_SetAddr Console_Proc_Read,Console_Read
        Console_SetAddr Console_Proc_PutChar,Console_PutChar
        Console_SetAddr Console_Proc_Scroll,Console_Scroll
        Console_SetAddr Console_Proc_ScrollDelta,Console_ScrollDelta
        Console_SetAddr Console_Proc_AddLine,Console_AddLine
        Console_SetAddr Console_Proc_Clear,Console_Clear
        Console_SetAddr Console_Proc_Show_Cursor,Console_Show_Cursor
        Console_SetAddr Console_Proc_Hide_Cursor,Console_Hide_Cursor
        Console_SetAddr Console_Proc_GotoXY,Console_GotoXY
        Console_SetAddr Console_Proc_GetXY,Console_GetXY
        Console_SetAddr Console_Proc_Set_Pallete,Console_Set_Pallete
        Console_SetAddr Console_Proc_Switch,Console_Switch
        Console_SetAddr Console_Proc_LoadFont,Console_LoadFont
        Console_SetAddr Console_Proc_Redraw,Console_Redraw

        ; nastavime aktivni konzoli
        mov   dword ptr ds:[Active_Console], 0

        ; alokuje 0. konzoli
        mov   edx, Max_Console_Count
    @Consoles_Init_Consoles_Creating :
        mov   ebx, Default_Console_Virt_Res
        mov   ecx, Default_Console_Res
        call  Console_Create
        jc    @Consoles_Init_Consoles_Creating_All
        dec   edx
        jnz   @Consoles_Init_Consoles_Creating
     @Consoles_Init_Consoles_Creating_All :

        Call  Nothing_Welcome
        Print Consoles_Init_txt
       pop    edi ecx eax es ds
        ret
Consoles_Init Endp

; cleneni screen bufferu
;          Virtualni graficka pamet konzoli
; +(Res_Y-1) ---------------------- vrchol bufferu posledni radek textu
;            |      .             |
;            |      .             |
;            |      .             |
;            |      .             |
;            |      .             |
;            |--------------------|
;            |                    |
;            |                    |
;            |                    |
;            |  tato cast bufferu |
;            |  je zobrazena      |
;            |  na obrazovce      |
;            |                    |
; (View_Y-1) |--------------------|
;            |      .             |
;            |      .             |
;            |      .             |
;            |      .             |
;            |      .             |
; +0         ---------------------- na obrazovce se zobrazi na prvnim radku

; prida a inicizlizuje konzolu
; Vstup: EBX - Lo(Console_Virt_Res_X)+hi(Console_Virt_Res_Y)
;        ECX - Lo(Console_Res_X)+hi(Console_Res_Y)
; Vystup: CF = 0 - OK
;         CF = 1 - Error
;            -  EAX = 0 - jiz nemuze byt otevreno vic konzoli
Console_Create Proc
       pushad
       push ds es
        mov  ax, data_descriptor
        mov  ds, ax
        mov  eax, dword ptr ds:[Consoles_Count]
        cmp  eax, Max_Console_Count
        jge  Console_Create_False_0

        mov  ds, word ptr ds:[Sel_Consoles]
        xor  esi, esi

        ; pokusime se najit volnou polozku v console pro konzolu
        mov  eax, Max_Console_Count
Console_Create_Find:
        cmp  word ptr ds:[esi+Console_Scr_Buf], 0
        jz   short Console_Create_Found
        add  esi, Console_Size
        dec  eax
        jnz  short Console_Create_Find
        jmp  Console_Create_False_0
Console_Create_Found:
        ; DS:ESI - ukazatel na volnou polozku console
        ; EBX - Lo(Console_Virt_Res_X)+hi(Console_Virt_Res_Y)
        ; ECX - Lo(Console_Res_X)+hi(Console_Res_Y)
        mov   eax, ebx
        and   eax, 0FFFFh
        shr   ebx, 16
        ; EAX - X ; EBX - Y

        ; rozliseni virtualniho screen bufferu
        mov   word ptr ds:[esi+Console_Virt_Res_X], ax
        mov   word ptr ds:[esi+Console_Virt_Res_Y], bx

        ; selector na screen buffer
        xor   edx, edx
        mul   ebx
        shl   eax, 1
        call  kAlloc
        mov   word ptr ds:[esi+Console_Scr_Buf], ax

        mov   eax, ecx
        and   eax, 0FFFFh
        shr   ecx, 16
        ; EAX - X ; ECX - Y

        ; rozliseni zobrazovaciho okna (obrazovky)
        mov   word ptr ds:[esi+Console_Res_X], ax
        mov   word ptr ds:[esi+Console_Res_Y], cx

        ; pozice kurzoru ve screen bufferu
        mov   word ptr ds:[esi+Console_Cur_X], 1
        mov   word ptr ds:[esi+Console_Cur_Y], 1

        ; pozice kurzoru mysi ve screen bufferu
        mov   word ptr ds:[esi+Console_Mouse_X], 1
        mov   word ptr ds:[esi+Console_Mouse_Y], 1
        mov   word ptr ds:[esi+Console_View_Y], 0

        ; standartne pouzivane barevne attributy pro text a pozadi
        mov   word ptr ds:[esi+Console_Color_Atr], 07h

        ; attributy klavesnice (CapsLock,NumLock,ScrollLock)
        mov   byte ptr ds:[esi+Console_Keyb_Attr], 0

        ; vlajecky
        mov   dword ptr ds:[esi+Console_Flags], 1

        ; ukazatel na font pouzivany na konzoli
        ; !!!!!!!! dodelat praci s fonty
        mov   dword ptr ds:[esi+Console_Font], 0
        mov   word ptr  ds:[esi+Console_Font+4], 0

        ; ukazatel na tabulku s funkcema pro praci s konzoli
        mov   dword ptr ds:[esi+Console_Proc_Addr], offset Console_Proc_Tab
        mov   word ptr ds:[esi+Console_Proc_Addr+4], data_descriptor

        ; paleta konzoli <- nakopirovat standartni barevnou paletu
       push   ds esi
        mov   edi, esi
        add   edi, Console_Pallete
       push   ds
       pop    es
        ; udelat algoritmus pro vygenerovani defaultni palety
        mov   ax, data_descriptor
        mov   ds, ax
        mov   esi, offset Default_Pallete
        mov   ecx, 256*3
        call  move
       pop    esi ds

        ; vymazeme konzolu
        call Console_Clear

        mov   ax, data_descriptor
        mov   ds, ax
        inc   dword ptr ds:[Consoles_Count]

       pop es ds
       popad
       clc
        ret

Console_Create_False_0: ;vice konzoli nemuze byt otevreno
       pop es ds
       popad
       stc
       xor  eax, eax
       ret
Console_Create endp

; funkce pro raw zapis do konzoli
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
;         ES:EDI - vstupni buffer
;         EBX    - raw pozice ve screen bufferu
;         ECX    - delka retezce
; Vystup: CF = 0 - OK
;         CF = 1  - Error
Console_Write           Proc
      pushad
      push   ds es
       mov   ax, word ptr ds:[esi+Console_Scr_Buf]
      push   es
      pop    ds
       mov   esi, edi
       ; DS:ESI = ukazatel na vstupni buffer
       mov   es, ax
       mov   edi, ebx
       ; ES:EDI - ukazatel na pozici ve screen bufferu
       call  Move
      pop    es ds
      popad
       ret
Console_Write           Endp

; funkce pro raw cteni z konzoli
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
;         ES:EDI - vystupni buffer
;         EBX    - raw pozice ve screen bufferu
;         ECX    - delka retezce
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_Read            Proc
      pushad
      push   ds
       mov   ds, word ptr ds:[esi+Console_Scr_Buf]
       mov   esi, ebx
       call  Move
      pop    ds
      popad
       ret
Console_Read   Endp
; zapise na aktualni pozici kurzoru konzolu znak
; Vstup:  DS:ESI - ukazatel na polozku konzoli
;         AL     - znak, ktery ma zaobrazit
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_PutChar         Proc
      pushad
      push   ds es

       xchg  ecx, eax
       ; ECX=znak
       movzx ebx, word ptr ds:[esi+Console_Virt_Res_X]
       ; zjistime, jestli kurzor neukazuje mimo virtualni screen
       movzx eax, word ptr ds:[esi+Console_Cur_Y]
       cmp   ax, word ptr ds:[esi+Console_Virt_Res_Y]
       jg    short Console_PutChar_10
       jne   short Console_PutChar_1
       ; cur_Y=Virt_Res_Y => ze musi Cur_X<Virt_Res_X
       ; zjistime, jestli kurzor neukazuje mimo virtualni screen
       cmp   bx, word ptr ds:[esi+Console_Cur_X]
       ; pokud to hodi error tak tam dej jge
       jg   short Console_PutChar_1
Console_PutChar_10:
       call  console_addline
       movzx eax, word ptr ds:[esi+Console_Virt_Res_Y]
       mov   word ptr ds:[esi+Console_Cur_Y], ax
       mov   word ptr ds:[esi+Console_Cur_X], 1
Console_PutChar_1:
       dec   eax
       xor   edx, edx
       mul   ebx
      push   ebx
       movzx ebx, word ptr ds:[esi+Console_Cur_X]
       add   eax, ebx
      pop    ebx
       dec   eax
       shl   eax, 1
       mov   edi, eax
       mov   es, word ptr ds:[esi+Console_Scr_Buf]
       ; ES:EDI - ukazuje na pozici kurzoru ve virtualnim screen bufferu
       xchg  ecx, eax
       mov   ah, byte ptr ds:[esi+Console_Color_Atr]
       stosw
       ; posuneme kurzor
       mov   eax, edi
       shr   eax, 1
       xor   edx, edx
       div   word ptr ds:[esi+Console_Virt_Res_X]
       inc   eax
       inc   edx
       mov   word ptr ds:[esi+Console_Cur_X], dx
       mov   word ptr ds:[esi+Console_Cur_Y], ax

       movzx eax, word ptr ds:[esi+Console_Cur_Y]
       movzx ebx, word ptr ds:[esi+Console_Res_Y]
       cmp   eax, ebx
       jle   short Console_PutChar_True
       sub   eax, ebx
       mov   word ptr ds:[esi+Console_View_Y], ax
Console_PutChar_True:
       clc   ;CF=0
       jmp   short Console_PutChar_End
Console_PutChar_False:
       stc
Console_PutChar_End:
      pop    es ds
      popad
       ret
Console_PutChar         Endp

; zapise na aktualni pozici kurzoru konzolu znak
; Vstup:  EBX - index polozky konzoli
;         AL  - znak, ktery ma zaobrazit
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_PutChar_Ext         Proc
       pushad
       push ds es
       push   eax
        mov   eax, ebx
        call  Get_Console_Addr
       pop    eax
        jc    short Console_Putchar_Ext_End
        call  Console_Putchar
Console_Putchar_Ext_End:
       pop es ds
       popad
        ret
Console_PutChar_Ext         Endp

; fce pro scrolovani zobrazovaciho okna ve virtualni pameti
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
;         EAX    - od kolikateho radku zobrazovat
; Vystup: -
Console_Scroll          Proc
      push    ebx ecx
       movzx  ebx, word ptr ds:[esi+Console_Virt_Res_Y]
       movzx  ecx, word ptr ds:[esi+Console_View_Y]
       sub    ebx, ecx
       cmp    eax, ebx
       jle    short Console_Scroll0
       ; scroll max
       mov    eax, ebx
Console_Scroll0:
       mov    word ptr ds:[esi+Console_View_Y], ax
Console_Scroll_End:
      pop     ecx ebx
       ret
Console_Scroll          Endp

; fce pro scrolovani zobrazovaciho okna ve virtualni pameti o dany pocet radku (+/-)
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
;         EAX    - o kolik radku se ma scrolnout, pokud je cislo zaporne scroluje se dolu, ...
; Vystup: -
Console_ScrollDelta     Proc
      push    ebx
       movzx  ebx, word ptr ds:[esi+Console_View_Y]
       add    eax, ebx
       call   Console_Scroll
      pop     ebx
       ret
Console_ScrollDelta     Endp

; vymaze virt. pamet konzoli
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup:
Console_Clear           Proc
      pushad
      push   ds es
       mov   es, word ptr ds:[esi+Console_Scr_Buf]
       mov   word ptr ds:[esi+Console_Cur_X], 1
       mov   word ptr ds:[esi+Console_Cur_Y], 1
       xor   edi, edi
       xor   ecx, ecx
       mov   cx, es
       lsl   ecx, ecx
       shr   ecx, 2
       mov   eax, 07200720h
       cld
       rep   stosd
      pop    es ds
      popad
       ret
Console_Clear           Endp

; zobrazi kurzor
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup:
Console_Show_Cursor     Proc
      push   eax ebx ecx edx
       ; zjistime, jestli je kurzor nad zobrazovanym vyrezem virt. obrazovky
       movzx ecx, word ptr ds:[esi+Console_Res_Y]
       movzx eax, word ptr ds:[esi+Console_View_Y]
       add   ecx, eax
       movzx eax, word ptr ds:[esi+Console_Cur_Y]
       cmp   eax, ecx
       jle   short Console_Show_Cursor0
Console_Show_Cursor_hide:
       call  Console_Hide_Cursor
       jmp   short Console_Show_Cursor_End
Console_Show_Cursor0:
       ; zjistime, jestli je kurzor pod zobrazovanym vyrezem virt. obrazovky
       movzx edx, word ptr ds:[esi+Console_View_Y]
       cmp   eax, edx
       jl    short Console_Show_Cursor_hide
       sub   eax, edx

       movzx ebx, word ptr ds:[esi+Console_Res_X]
       movzx ecx, word ptr ds:[esi+Console_Cur_X]
       cmp   ecx, ebx
       jg    short Console_Show_Cursor_hide

       dec   ecx
       dec   eax
       mul   ebx
       add   eax, ecx
       call  Set_Screen_Cursor_Pos
Console_Show_Cursor_End:
      pop    edx ecx ebx eax
       ret
Console_Show_Cursor     Endp

; skryje kurzor
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup: -
Console_Hide_Cursor     Proc
      push   eax ebx edx
       movzx eax, word ptr ds:[esi+Console_Res_X]
       movzx ebx, word ptr ds:[esi+Console_Res_Y]
       xor   edx, edx
       mul   ebx
       call  Set_Screen_Cursor_Pos
      pop    edx ebx eax
       ret
Console_Hide_Cursor     Endp

; posune kurzor na danou pozici(v ramci virt. pameti)
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
;         EAX    - X souradnice kurzoru v ramci virtualniho rozliseni konzoli
;         EBX    - Y souradnice ...
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_GotoXY          Proc
      push   ecx edx
       ; zjistime, jestli kurzor neukazuje mimo virtualni screen
       movzx ecx, word ptr ds:[esi+Console_Virt_Res_Y]
       movzx edx, word ptr ds:[esi+Console_Virt_Res_X]
       cmp   bx, cx
       jg    short Console_GotoXY_False
       cmp   ax, dx
       jle   short Console_GotoXY_End
Console_GotoXY_False:
       mov   eax, edx
       mov   ebx, ecx
Console_GotoXY_End:
       mov   word ptr ds:[esi+Console_Cur_Y], bx
       mov   word ptr ds:[esi+Console_Cur_X], ax
      pop    edx ecx
       ret
Console_GotoXY          Endp

; posune kurzor na danou pozici(v ramci virt. pameti)
; Vstup:  EAX    - ukazatel na polozku informacni struktury dane konzoli (Console)
;         EBX    - X souradnice kurzoru v ramci virtualniho rozliseni konzoli
;         ECX    - Y souradnice ...
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_GotoXY_Ext          Proc
       pushad
       push ds es
        call  Get_Console_Addr
        jc    short Console_GotoXY_Ext_End
        mov   eax, ebx
        mov   ebx, ecx
        call  Console_GotoXY
Console_GotoXY_Ext_End:
       pop es ds
       popad
        ret
Console_GotoXY_Ext          Endp


; navrati aktualni pozici kurzoru
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup: EAX    - X souradnice kurzoru v ramci virtualniho rozliseni konzoli
;         EBX    - Y souradnice ...
Console_GetXY           Proc
       movzx ebx, word ptr ds:[esi+Console_Cur_Y]
       movzx eax, word ptr ds:[esi+Console_Cur_X]
       ret
Console_GetXY           Endp

; uvolni 1. radek a kurzor presune na jeho zacatek
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_AddLine         Proc
       pushad
       push ds es
        movzx eax, word ptr ds:[esi+Console_Virt_Res_Y]
        dec   eax
        mov   ebx, eax
        xor   edx, edx
        mul   word ptr ds:[esi+Console_Virt_Res_X]
        shl   eax, 1
        mov   ecx, eax

      push ds esi
        ; ECX = kolik bajtu ve screen bufferu se ma presunout (VirtResY-1)*VirtResX*2
        movzx eax, word ptr ds:[esi+Console_Virt_Res_X]
        shl   eax, 1
        mov   es, word ptr ds:[esi+Console_Scr_Buf]
        mov   ds, word ptr ds:[esi+Console_Scr_Buf]

        xor   edi, edi
        mov   esi, eax
       push   ecx eax
        call  move

        ; vymazeme last radek
       pop    ecx edi
        shr   ecx, 2
        mov   eax, 07200720h
        cld
        rep   stosd
       pop esi ds

        mov   eax, 1
        movzx ebx, word ptr ds:[esi+Console_Virt_Res_Y]
        call Console_GotoXY
        jc   short Console_AddLine_False
        clc  ;CF=0
        jmp  short Console_AddLine_End
Console_AddLine_False:
        stc  ;CF=1
Console_AddLine_End:
       pop es es
       popad
        ret
Console_AddLine         Endp

; uvolni 1. radek a kurzor presune na jeho zacatek
; Vstup:  EAX - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_AddLine_Ext        Proc
       pushad
       push ds es
        call  Get_Console_Addr
        jc    short Console_AddLine_Ext_End
        call  Console_addline
Console_AddLine_Ext_End:
       pop es ds
       popad
        ret
Console_AddLine_Ext        Endp

; nastavi pro danou konzoli barevnou paletu
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup:
Console_Set_Pallete     Proc
      push   es edi
       push  ds
       pop   es
       mov   edi, esi
       add   edi, Console_Pallete
       call  SetPallete
      pop    edi es
       ret
Console_Set_Pallete     Endp

; prepne na danou konzoli
; Vstup:  EAX - cislo konzoli, na ktera se ma aktivovat (0..31)
; Vystup: - Pokud se bude nekdo snazit prepnou na neexistujici konzolu
;           tak se aktivni konzola nezmeni, v opacnem pripade ano
Console_Switch          Proc
      pushfd
       call  exist_Console
       jc    Console_Switch_End
      push   esi ds ebx
       mov    bx, data_descriptor
       mov    ds, bx
       mov    dword ptr ds:[Active_Console], eax
       call   Get_Console_Addr
       call   Console_Set_Font
       call   Console_Set_Pallete
       call   Console_Redraw
      pop    ebx ds esi
Console_Switch_End:
      popfd
       ret
Console_Switch          Endp

; nastavi na konzoli font
; Vstup:  DS:ESI - ukazatel na polozku informacni struktury dane konzoli (Console)
; Vystup:
Console_LoadFont        Proc
       ret
Console_LoadFont        Endp



; obnovi zobrazovane data aktivni konzoli na obrazovce
; Vstup:  -
; Vystup: CF = 0 - Ok
;         CF = 1 - Error
Console_Redraw          Proc
      pushad
      push   ds es
       mov   ax, data_descriptor
       mov   ds, ax

       ; az pojedou tasky tak tu zjisti, jestli je tento task spusten
       ; na aktivni konzoli a pokud ano, refresne screen, jinak ne

       movzx eax, word ptr ds:[Active_Console]
       call  Get_Console_Addr
       jc    short Console_Redraw_False

       movzx eax, word ptr ds:[esi+Console_View_Y]
       ; testneme jestli nechce zobrazovat i data za virtualnim screen bufferem
       add   ax, word ptr ds:[esi+Console_Res_Y]
       cmp   eax, dword ptr ds:[esi+Console_Virt_Res_Y]
       jle   short Console_Redraw0
       mov   eax, dword ptr ds:[esi+Console_Virt_Res_Y]
       movzx ebx, word ptr ds:[esi+Console_Res_Y]
       sub   eax, ebx
       jmp   short Console_Redraw1
Console_Redraw0:
       movzx eax, word ptr ds:[esi+Console_View_Y]
Console_Redraw1:
      push   eax

       ; presuneme ukazatel na obrazovce na konec obrazoveho bufferu
       mov   ax, B8000_Descriptor
       mov   es, ax
       xor   edi, edi
       movzx eax, word ptr ds:[esi+Console_Res_Y]
       xor   edx, edx
       mul   word ptr ds:[esi+Console_Res_X]
       shl   eax, 1
       mov   ecx, eax
       ; ES:EDI - ukazatel na zacatek posledniho radku obrazovky
      pop    eax
       xor   edx, edx
       mul   word ptr ds:[esi+Console_Res_X]
       shl   eax, 1
       mov   ds, word ptr ds:[esi+Console_Scr_Buf]
       mov   esi, eax
       ; DS:ESI - ukazatel do virtualniho screen bufferu aktivni konzoli
       call  move

       ; zjistime, jestli je v prave zobrazovane casti virtualni videopameti
       ; a pokud ano zobrazime, pokud ne zneviditelnime
       mov   ax, data_descriptor
       mov   ds, ax
       movzx eax, word ptr ds:[Active_Console]
       call  Get_Console_Addr
       jc    short Console_Redraw_False
       call  Console_Show_Cursor
       clc   ;CF=0
       jmp   short Console_Redraw_End
Console_Redraw_False:
       stc   ;CF=1
Console_Redraw_End:
      pop    es ds
      popad
       ret
Console_Redraw          Endp


; POMOCNE PROCEDURY

; Zjisti, jeslit konzola existuje
; Vstup:  EAX - cislo konzoli
; vystup: CF = 0 - OK
;         CF = 1 - Konzola neexistuje
exist_Console Proc
      push   ds ebx edx eax
       cmp   eax, Max_Console_Count
       jge   short exist_Console_false
exist_Console_Test:
       ; EAX = 0..Max_Console_Count-1
       mov   bx, data_descriptor
       mov   ds, bx
       mov   ds, word ptr ds:[sel_consoles]
       xor   edx, edx
       mov   ebx, console_size
       mul   ebx
       mov   ebx, eax
       cmp   word ptr ds:[ebx+Console_Scr_Buf], 0
       jz    short exist_Console_false
       clc ; CF=0
       jmp   short exist_Console_End
exist_Console_false:
       stc ; CF=1
exist_Console_End:
      pop    eax edx ebx ds
       ret
exist_Console Endp

; navrati adresu infor struktury dane konzoli ve strukture Console
; Vstup:  EAX - cislo konzoli
; Vystup: CF = 0 - OK
;              DS:ESI - ukazatel na polozku konzoli
;         CF = 1 - Error -> konzola neexistuje
Get_Console_Addr Proc
      push   eax ebx edx ds
       call  Exist_Console
;       jc    short Get_Console_Addr_False
       mov   bx, data_descriptor
       mov   ds, bx
       mov   ds, word ptr ds:[Sel_Consoles]
       mov   ebx, Console_Size
       xor   edx, edx
       mul   ebx
       mov   esi, eax
      pop    edx edx ebx eax
       clc ;CF=0
       jmp   short Get_Console_Addr_End
Get_Console_Addr_False:
      pop    ds edx ebx eax
       stc ;CF=1
Get_Console_Addr_End:
       ret
Get_Console_Addr Endp

; Nastavi font podle aktivni konzoli
; Vstup:  -
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Console_Set_Font Proc
       ret
Console_Set_Font Endp

; Vstup:  DS:ESI - ukazatel na retezec
;         ECX    - delka retezce
kPrint_Write Proc
kPrint_Write_loop:
        lodsb
        xor   ebx, ebx
        call  console_PutChar_Ext
        dec   ecx
        jnz   short kPrint_Write_loop
        ret
kPrint_Write endp

; vypis na konzolu
; Vstup:  ES:EDI - ukazatel na vypisovany text, zakonceny na 0
; Vystup: CF = 0 - OK
;         CF = 1 - Error
; Syntaxe:
; %s  - string, ktery musi byt ukoncen na 0 (nepodporovano)
; %h1 - byte hex
; %h2 - word hex (nepodporovano)
; %h4 - dword hex
; %b1 - byte binary
; %b2 - word binary (nepodporovano)
; %b4 - dword binary
; %d1 - byte dec
; %d2 - word dec (nepodporovano)
; %d4 - dword dec
; \n  - odradkovani
; \b  - zpet na zacatek radku
; napr.
; segment, kde je ulozena promenna na zobrzeni je data segment kernelu
; db 'Hexa cislo 0x%h4'
; dd offset hexa_cislo
; db ' ...',0
kPrint Proc
       pushad
       push ds es
        push  es
        pop   ds
        mov   esi, edi

        xor   ecx, ecx
        mov   cx, es
        lsl   ecx, ecx
        sub   ecx, esi
        jc    kPrint_False

        mov   ebx, esi
kPrint_find_zero:
        cmp   byte ptr ds:[ebx], '%'
        jnz   short kPrint_find_zero1
        inc   ebx
        cmp   byte ptr ds:[ebx], 'h'
        jz    short kPrint_find_zero2
        cmp   byte ptr ds:[ebx], 'd'
        jz    short kPrint_find_zero2
        cmp   byte ptr ds:[ebx], 'b'
        jz    short kPrint_find_zero2
        jmp   short kPrint_find_zero1
kPrint_find_zero2:
        inc   ebx
        cmp   byte ptr ds:[ebx], '1'
        jz    short kPrint_find_zero3
        cmp   byte ptr ds:[ebx], '2'
        jz    short kPrint_find_zero3
        cmp   byte ptr ds:[ebx], '4'
        jz    short kPrint_find_zero3
        jmp   short kPrint_find_zero1
kPrint_find_zero3:
        add   ebx, 5
kPrint_find_zero1:
        cmp   byte ptr ds:[ebx], 0
        jz    short kPrint_found_zero
        inc   ebx
        dec   ecx
        jnz   short kPrint_find_zero
        jmp   kPrint_False
kPrint_found_zero:
       push   ebx
        sub   ebx, esi
        mov   ecx, ebx
       pop    edx
        ; ECX = delka zobrazovaneho retezce
        ; EDX = nejzazsi offsetova hodnota platna pro zobrazovany retezec

kPrint_print:
       push   edx
        lodsb
        cmp   al, '%'
        jnz   kPrint_print_test_lomitko
        lodsb
        cmp   al, '%'
        jnz   short kPrint_print_test200
        jmp   kPrint_print_show
kPrint_print_test200:
        cmp   al, 's'
        jnz   short kPrint_print_test201
        ; zobrazi string
        ; zatim nepodporovano
        jmp   kPrint_print_next
kPrint_print_test201:
        cmp   al, 'h'
        jnz   short kPrint_print_test300
        lodsb
        cmp   al, '1'
        jnz   short kPrint_print_test202
        ; zobrazi hexa byte
        lodsd
        mov   ebx, eax
        movzx eax, byte ptr ds:[ebx]
       push   ds esi
        call  hex2str_db
        call  kPrint_Write
       pop    esi ds
        jmp   kPrint_print_next
kPrint_print_test202:
;        cmp   al, '2'
;        jnz   short kPrint_print_test203
;        ; zobrazi hexa word
;        lodsd
;        mov   ebx, eax
;        movzx eax, word ptr ds:[ebx]
;       push   ds esi
;        call  hex2str_dw
;        call  kPrint_Write
;       pop    esi ds
;        jmp   short kPrint_print_next
kPrint_print_test203:
        cmp   al, '4'
        jnz   short kPrint_print_test204
        ; zobrazi hexa dword
        lodsd
        mov   ebx, eax
        mov   eax, dword ptr ds:[ebx]
       push   ds esi
        call  hex2str_dd
        call  kPrint_Write
       pop    esi ds
        jmp   kPrint_print_next
kPrint_print_test204:
        jmp   kPrint_print_show
kPrint_print_test300:
        cmp   al, 'b'
        jnz   short kPrint_print_test400
        lodsb
        cmp   al, '1'
        jnz   short kPrint_print_test301
        ; zobrazi binary byte
        lodsd
        mov   ebx, eax
        movzx eax, byte ptr ds:[ebx]
       push   ds esi
        call  bin2str_db
        call  kPrint_Write
       pop    esi ds
        jmp   kPrint_print_next
kPrint_print_test301:
;        cmp   al, '2'
;        jnz   short kPrint_print_test302
;        ; zobrazi binary word
;        lodsd
;        mov   ebx, eax
;        movzx eax, word ptr ds:[ebx]
;       push   ds esi
;        call  bin2str_dw
;        call  kPrint_Write
;       pop    esi ds
;        jmp   short kPrint_print_next
kPrint_print_test302:
        cmp   al, '4'
        jnz   short kPrint_print_test303
        ; zobrazi binary dword
        lodsd
        mov   ebx, eax
        mov   eax, dword ptr ds:[ebx]
       push   ds esi
        call  bin2str_dd
        call  kPrint_Write
       pop    esi ds
        jmp   kPrint_print_next
kPrint_print_test303:
        jmp   kPrint_print_show
kPrint_print_test400:
        cmp   al, 'd'
        jnz   short kPrint_print_test500
        lodsb
        cmp   al, '1'
        jnz   short kPrint_print_test401
        ; zobrazi dec byte
        lodsd
        mov   ebx, eax
        movzx eax, byte ptr ds:[ebx]
       push   ds esi
        call  dec2str_db
        call  kPrint_Write
       pop    esi ds
        jmp   short kPrint_print_next
kPrint_print_test401:
;        cmp   al, '2'
;        jnz   short kPrint_print_test402
;        ; zobrazi dec word
;        lodsd
;        mov   ebx, eax
;        movzx eax, word ptr ds:[ebx]
;       push   ds esi
;        call  dec2str_dw
;        call  kPrint_Write
;       pop    esi ds
;        jmp   short kPrint_print_next
kPrint_print_test402:
        cmp   al, '4'
        jnz   short kPrint_print_test403
        ; zobrazi dec dword
        lodsd
        mov   ebx, eax
        mov   eax, dword ptr ds:[ebx]
       push   ds esi
        call  DEC2Str_DD_WithoutNulls
        call  kPrint_Write
       pop    esi ds
        jmp   short kPrint_print_next
kPrint_print_test403:
kPrint_print_test500:
        jmp   short kPrint_print_show

kPrint_print_test_lomitko:
        cmp   al, '\'
        jnz   kPrint_print_show
        lodsb
        cmp   al, '\'
        jnz   short kPrint_print_test100
        jmp   short kPrint_print_show
kPrint_print_test100:
        cmp   al, 'n'
        jnz   short kPrint_print_test101
        ; odradkuj
       pushad
       push ds
        xor   eax, eax
        call  Get_Console_Addr
        call  Console_GetXY
        inc   ebx
        mov   eax, 1
        call  Console_GotoXY
       pop ds
       popad
        jmp   short kPrint_print_next
kPrint_print_test101:
        cmp   al, 'b'
        jnz   short kPrint_print_test102
        ; presun kurzor na zacatek radku
        xor   eax, eax
        mov   ebx, 1
        mov   ecx, 1
        call  Console_GotoXY_Ext
        jmp   short kPrint_print_next
kPrint_print_test102:
        ; neznama fce jeji znak zobrazime
kPrint_print_show:
        xor   ebx, ebx
        call  console_PutChar_ext
kPrint_print_next:
       pop    edx
        cmp   esi, edx
        jl    kPrint_print

        call  Console_Redraw
        clc   ;CF=0
        jmp   short kPrint_End
kPrint_False:
        stc   ; CF=1
kPrint_End:
       pop es ds
       popad
        ret
kPrint Endp
;############################################################################################################
;# nastavi kurzor na obrazovce na danou pozici
;############################################################################################################
;# Vstup:  EAX - offsetova hodnota pozoce kurzoru na obrazovce
;#             - napr. pozice X:1 Y:10 = Y*Screen_Res_X(80)+X => 801
;############################################################################################################
;# Vystup: -
;############################################################################################################
Set_Screen_Cursor_Pos Proc
      push   eax ecx edx
      pushfd
       cli
       xchg  eax, ecx
       mov   dx, 3D4h
       mov   al, 0eh
       out   dx, al
       inc   dx
       mov   al, ch ;Y
       out   dx, al
       dec   dx
       mov   al, 0fh
       out   dx, al
       inc   dx
       mov   al, cl ;X
       out   dx, al
      popfd
      pop    edx ecx eax
       ret
Set_Screen_Cursor_Pos Endp


Include   .\..\inc\code\code32\CONSOLE\consdrv.inc
Include   .\..\inc\code\code32\CONSOLE\chars.inc