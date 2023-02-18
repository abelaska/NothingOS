;###########################################################################
;# Driver pro prevod scancodu na ascii
;###########################################################################

.386

Header segment
;###########################################################################
;# Drv header
;###########################################################################
DB         'DRV',0             ; signature
DW         08100h              ; typ zarizeni
DW         (DataEnd-DataStart) ; data size
DD         (CodeEnd-CodeStart) ; code size
DD         HeaderEnd           ; ukazatel na zacatek data segmentu
DD         20h+DataEnd         ; ukazatel na zacatek code segmentu (20h=header size)
DW         (Fce_Count-DataStart) /4 ; Num_Of_FCE
align      16
HeaderEnd:
Header Ends

Data segment
;###########################################################################
;# Data
;###########################################################################
DataStart :
   DD      Offset INIT                          ;00h
   DD      Offset DONE                          ;01h
   DD      Offset UnImplement                   ;02h
   DD      Offset UnImplement                   ;03h
   DD      Offset UnImplement                   ;04h
   DD      Offset UnImplement                   ;05h
   DD      Offset UnImplement                   ;06h
   DD      Offset UnImplement                   ;07h
   DD      Offset UnImplement                   ;08h
   DD      Offset UnImplement                   ;09h
   DD      Offset UnImplement                   ;0Ah
   DD      Offset UnImplement                   ;0Bh
   DD      Offset UnImplement                   ;0Ch
   DD      Offset UnImplement                   ;0Dh
   DD      Offset UnImplement                   ;0Eh
   DD      Offset UnImplement                   ;0Fh
   DD      Offset Scancode_2_ASCII_Entrance     ;10h
Fce_Count :
   DB      'SCAN2ASCII',0
   Include keybdata.inc
   Align  32
DataEnd :
Data Ends
;###########################################################################
;# Code
;###########################################################################
Code Segment Public Use32 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;

CodeStart:
Sel_Data  dw      0           ; selector na datasegment daneho driveru
;############################################################################################
INIT :
    RetF
;############################################################################################
DONE :
    RetF
;############################################################################################
UnImplement :
    RetF
;############################################################################################
;# Vykonna procedura na prevod
;############################################################################################
;# Vstup  : DL  - Scan code Znak
;#          CX  - Flagy
;# Vystup : DX  - ASCII extended
;#          EBX - 000000000h = Klavesa sla dolu
;#                0FFFFFFFFh = Klavesa sla nahoru
;#          CX  - Flagy
;############################################################################################
Scancode_2_ASCII_Entrance :
   PUSH DS EAX ESI EDI EBP
        Mov    DS, Word ptr CS:[Sel_Data]
        Mov    Word ptr DS:[TEMP_Buffer_Flags],CX
        Mov    BX,CX
        Xor    ECX,ECX
        Mov    Al,DL
        Test   BH,08
        Jnz    Scan2Ascii_Long_Seq
        Cmp    Al,0e0h
        Jnb    Scan2Ascii_Long_Seq_Start ; je to zacatek vice-znakova sekvence
        Test   Al,80h
        Jz     Scan2Ascii_Store_Key      ; uvolneni klavesy
Scan2Ascii_Key_Up :
        And    Al,07Fh
        Dec    CH
Scan2Ascii_Store_Key :            ;vsechno probehlo v poradku, ulozit na buffer klavesu
        ;zjisteni, jestli je NumLock
        Test   Bl,02h
        Jnz    Scan2Ascii_StoreWithNum
        Cmp    Al,47H
        Jb     Scan2Ascii_StoreWithNum
        Cmp    Al,53h
        Ja     Scan2Ascii_StoreWithNum
        Sub    Al,46h
        Mov    ESI,Offset Keyboard_ScanCode2ascii_NonNumLock
        Jmp    Scan2Ascii_S2A_Now
Scan2Ascii_StoreWithNum :
        ;nastaveni spravne tabulky, podle Shiftu a CapsLocku
        Test   BX,0120h
        Jz     Scan2Ascii_NoShift
        Mov    ESI,offset Keyboard_ScanCode2ascii_Table3
        Test   Bl,04h
        Jz     Scan2Ascii_S2A_Now ;Shift = 1, Caps = 0 => Table3
        Mov    ESI,offset Keyboard_ScanCode2ascii_Table4
        Jmp    Scan2Ascii_S2A_Now ;Shift = 1, Caps = 1 => Table4
Scan2Ascii_NoShift :
        Mov    ESI,offset Keyboard_ScanCode2ascii_Table1
        Test   Bl,04h
        Jz     Scan2Ascii_S2A_Now ;Shift = 0, Caps = 0 => Table1
        Mov    ESI,offset Keyboard_ScanCode2ascii_Table2 ;Shift = 0, Caps = 1 => Table2
Scan2Ascii_S2A_Now :
        Movzx  EAX,Al
        DEC    AL
        Shl    EAX,1
        Add    ESI,EAX
        Mov    DX,Word ptr DS:[ESI]
        ;Test, jesli byl zmacnuty Shift,Caps,Num,...
        Test   Dh,1
        Jz     Scan2Ascii_Ascii2Buffer
        Cmp    Dl,01h
        Jne    Scan2Ascii_Next_0001 ; neni LALT
        Or     CH,CH
        Jnz    Scan2Ascii_Uvol_0001 ; je to uvolneni klavesy
        Or     Word ptr DS:[TEMP_Buffer_Flags],0040h
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Uvol_0001 :
        Xor    Word ptr DS:[TEMP_Buffer_Flags],0040h
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Next_0001:
        Cmp    Dl,02h
        Jne    Scan2Ascii_Next_0002 ; neni LCTRL
        Or     CH,CH
        Jnz    Scan2Ascii_Uvol_0002 ; je to uvolneni klavesy
        Or     Word ptr DS:[TEMP_Buffer_Flags],0080h
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Uvol_0002 :
        AND    Word ptr DS:[TEMP_Buffer_Flags],0FF7Fh
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Next_0002:
        Cmp    Dl,03h
        Jne    Scan2Ascii_Next_0003 ; neni LSHIFT
        Or     CH,CH
        Jnz    Scan2Ascii_Uvol_0003 ; je to uvolneni klavesy
        Or    Word ptr DS:[TEMP_Buffer_Flags],0100h
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Uvol_0003 :
        Xor    Word ptr DS:[TEMP_Buffer_Flags],0100h
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Next_0003:
        Cmp    Dl,08h
        Jne    Scan2Ascii_Next_0004 ; neni NUM
        Or     CH,CH
        Jnz    Scan2Ascii_Next_0004 ; je to uvolneni klavesy, nebereme
        Xor    Word ptr DS:[TEMP_Buffer_Flags],2
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Next_0004:
        Cmp    Dl,09h
        Jne    Scan2Ascii_Next_0005 ; neni CapS
        Or     CH,CH
        Jnz    Scan2Ascii_Next_0005 ; je to uvolneni klavesy, nebereme
        Xor    Word ptr DS:[TEMP_Buffer_Flags],4
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Next_0005:
        Cmp    Dl,0Ah
        Jne    Scan2Ascii_Next_0006 ; neni ScroolLock
        Or     CH,CH
        Jnz    Scan2Ascii_Next_0006 ; je to uvolneni klavesy, nebereme
        Xor    Word ptr DS:[TEMP_Buffer_Flags],1
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Next_0006:
        Cmp    Dl,06h
        Jne    Scan2Ascii_Ascii2Buffer ; neni Rshift
        Or     CH,CH
        Jnz    Scan2Ascii_Uvol_0004 ; je to uvolneni klavesy
        Or     Word ptr DS:[TEMP_Buffer_Flags],0020h
        Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Uvol_0004 :
        Xor    Word ptr DS:[TEMP_Buffer_Flags],0020h
        Jmp    Scan2Ascii_Ascii2Buffer


Scan2Ascii_Long_Seq_Start :  ;zacatek long seqvence
; DS = Data Segment Kernelu

       Mov    EDI,offset Keyboard_Internal_Buffer
       Mov    Byte ptr Ds:[EDI],1
       Mov    Byte ptr Ds:[EDI+1],Al
       Or     Word ptr DS:[TEMP_Buffer_Flags],0800h
       Jmp    Scan2Ascii_End

Scan2Ascii_Long_Seq :
       Xor    EBX,EBX
       Mov    EDI,offset Keyboard_Internal_Buffer
       Mov    Bl,Byte ptr Ds:[EDI]
       Inc    Bl
       Mov    Byte ptr Ds:[EDI],Bl
       Add    EDI,EBX
       Test   Al,80h
       Jz     Scan2Ascii_Ok1
       Cmp    Al,0e0h
       Jnb    Scan2Ascii_OK1
       Or     Ch,02h
       Xor    Al,80h
Scan2Ascii_Ok1 :
       Mov    Byte ptr Ds:[EDI],Al
       Sub    EDI,EBX
       Cmp    Bl,02h
       Je     @Keyborad_Test2
       Cmp    Bl,06h
       Je     Scan2Ascii_Test6
@Keyborad_Test2 :
       Inc    EDI
       Mov    ESI,Offset Keyboard_2Chars2ascii
       Mov    AX,Word ptr DS:[EDI]
       Mov    EBX,Dword ptr Ds:[ESI]
Scan2Ascii_Test2Loop :
       Add    ESI,4
       Mov    EDX,dword ptr Ds:[ESI]
       Cmp    AX,DX
       Je     Scan2Ascii_Equal
       Dec    EBX
       Jnz    Scan2Ascii_Test2Loop
       Jmp    Scan2Ascii_End
Scan2Ascii_Equal :
       And    Byte ptr DS:[TEMP_Buffer_Flags+1],0F7h
       Shr    EDX,16
       Cmp    Dl,04h
       Jne    Scan2Ascii_Next_0008 ; neni RALT
       Or     CH,CH
       Jnz    Scan2Ascii_Uvol_0008 ; je to uvolneni klavesy
       Or     Word ptr DS:[TEMP_Buffer_Flags],0008h
       Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Uvol_0008 :
       And    Word ptr DS:[TEMP_Buffer_Flags],0FFF7h
       Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Next_0008 :
       Cmp    Dl,05h
       Jne    Scan2Ascii_Ascii2Buffer ; neni RCTRL
       Or     CH,CH
       Jnz    Scan2Ascii_Uvol_0009 ; je to uvolneni klavesy
       Or     Word ptr DS:[TEMP_Buffer_Flags],0010h
       Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Uvol_0009 :
       And    Word ptr DS:[TEMP_Buffer_Flags],0FFEFh
       Jmp    Scan2Ascii_Ascii2Buffer


Scan2Ascii_Test6 :
       And    Byte ptr DS:[TEMP_Buffer_Flags+1],0F7h
       Mov    ESI,Offset Keyboard_6Chars2ascii
       Inc    EDI
       Mov    EAX,Dword ptr Ds:[EDI]
       Mov    EDX,Dword ptr Ds:[ESI]
       Cmp    EAX,EDX
       Jne    Scan2Ascii_Unknown_Key
       Mov    Dx,0107h
       Jmp    Scan2Ascii_Ascii2Buffer
Scan2Ascii_Unknown_Key :
       Mov    Dx,'A'
Scan2Ascii_Ascii2Buffer :
       PUSH   ECX
        Mov    CX,Word ptr DS:[TEMP_Buffer_Flags]
        Mov    EBX,ECX
        Shr    EBX,3
        And    EBX,3Fh
        Shl    EBX,10
        Or     DX,BX  ; nastaveni spravnych flagu
       POP    EBX
        Movsx  EBX,BH
Scan2Ascii_End : ;Standardni konec rutiny preruseni
        Mov    CX,Word ptr DS:[TEMP_Buffer_Flags]
       POP  EBP EDI ESI EAX DS
        RetF
CodeEnd:
Code Ends
end CodeStart

