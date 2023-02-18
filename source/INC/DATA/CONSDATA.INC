;############################################################################
;# Datove structury a promenne pro praci s CONSOLES
;############################################################################

; struktura pameti, s ukazately na procedury pro praci s konzoli
Console_Proc Struc
        Console_Proc_Write           Far_Pointer <> ; funkce pro raw zapis do konzoli
        Console_Proc_Read            Far_Pointer <> ; funkce pro raw cteni z konzoli
        Console_Proc_PutChar         Far_Pointer <> ; zapise na aktualni pozici kurzoru konzolu znak
        Console_Proc_Scroll          Far_Pointer <> ; fce pro scrolovani zobrazovaciho okna ve virtualni pameti
        Console_Proc_ScrollDelta     Far_Pointer <> ; fce pro scrolovani zobrazovaciho okna ve virtualni pameti o dany pocet radku (+/-)
        Console_Proc_AddLine         Far_Pointer <> ; fce ktera uvolni 1. radek, cili posune vsechny radky od 2. nahoru s tim, ze posledni radek bude ztracen
        Console_Proc_Clear           Far_Pointer <> ; vymaze virt. pamet konzoli
        Console_Proc_Show_Cursor     Far_Pointer <> ; zobrazi kurzor
        Console_Proc_Hide_Cursor     Far_Pointer <> ; skryje kurzor
        Console_Proc_GotoXY          Far_Pointer <> ; posune kurzor na danou pozici(v ramci virt. pameti)
        Console_Proc_GetXY           Far_Pointer <> ; navrati aktualni pozici kurzoru
        Console_Proc_Set_Pallete     Far_Pointer <> ; nastavi pro danou konzoli barevnou paletu
        Console_Proc_Switch          Far_Pointer <> ; prepne na danou konzoli
        Console_Proc_LoadFont        Far_Pointer <> ; nastavi na konzoli font
        Console_Proc_Redraw          Far_Pointer <> ; obnovy zobrazovane data na obrazovce
Console_Proc Ends

Console Struc
        Console_Start           equ $
        Console_Proc_Addr       Far_Pointer <>

        ; vsechny hodnoty tykajici se rozliseni maji takovyto tvar:
        ; napr. rozliseni 80x30:
        ;  Console_Res_X=80, Console_Res_Y=30, ...
        ; napr. kurzor ukazuje na 0. znak na obrazovce:
        ; Console_Cur_X=1 Console_Cur_Y=1
        Console_Virt_Res_X      dw 0 ; rozliseni screen bufferu
        Console_Virt_Res_Y      dw 0 ;
        Console_Res_X           dw 0 ; rozliseni zobrazovaciho okna (obrazovky)
        Console_Res_Y           dw 0 ;
        Console_Cur_X           dw 0 ; pozice kurzoru ve screen bufferu
        Console_Cur_Y           dw 0 ;
        Console_Mouse_X         dw 0 ; pozice kurzoru mysi ve screen bufferu
        Console_Mouse_Y         dw 0 ;
        Console_View_Y          dw 0 ; vysvetleno na obrazku
                                     ; <0..Console_Virt_Res_Y-Console_Res_Y-1>
        Console_Color_Atr       db 0 ; standartne pouzivane barevne attributy pro text a pozadi
        Console_Keyb_Attr       db 0 ; attributy klavesnice (CapsLock,NumLock,ScrollLock)
        Console_Flags           dd 0 ; vlajecky
        Console_Font            Far_Pointer <> ; ukazatel na font pouzivany na konzoli
        Console_Scr_Buf         dw 0 ; selector na screen buffer 32Kb
                                     ; pokud je hodnota 0, je tato polozka zatim nepouzita
        Console_Pallete         db 256*3 dup(0) ; paleta konzoli
        Console_End             equ $
Console Ends

Console_Size             equ Console_End - Console_Start ;882B ?

Default_Console_Res_X    equ 80
Default_Console_Res_Y    equ 30
Default_Console_Virt_Res equ (Default_Console_Res_Y*3)*10000h+Default_Console_Res_X
Default_Console_Res      equ (Default_Console_Res_Y*10000h)+Default_Console_Res_X

Console_Proc_Tab         Console_Proc <>
Max_Console_Count        equ     12
Sel_Consoles             dw      0
Consoles_Count           dd      0
Active_Console           dd      -1 ; -1 = none

console_initialized      db 0

; Console_Flags:
; 0. bit - stav kurzoru = '0'-Off, '1'-On

; Console_Keyb_Attr:
; 0.bit - NumLock
; 1.bit - CapsLock
; 2.bit - ScrollLock