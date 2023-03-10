;############################################################################
;# Datove structury a promenne driveru KEYBOARD
;############################################################################

KEYB_DATA :
KEYB_Data_Start :
;################# standardni fce
   DD      Offset KEYB_INIT     ;00h
   DD      Offset KEYB_DONE     ;01h
   DD      Offset KEYB_READ     ;02h
   DD      Offset UnImplement   ;03h
   DD      Offset UnImplement   ;04h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;06h
   DD      Offset UnImplement   ;07h
   DD      Offset UnImplement   ;08h
   DD      Offset UnImplement   ;09h
   DD      Offset UnImplement   ;0Ah
   DD      Offset UnImplement   ;0Bh
   DD      Offset UnImplement   ;0Ch
   DD      Offset UnImplement   ;0Dh
   DD      Offset UnImplement   ;0Eh
   DD      Offset UnImplement   ;0Fh
;################# rozsirene fce
   DD      Offset Keyb_SetFlags_vd              ;10h
   DD      Offset Keyb_SetAutoRepeat_vd         ;11h
   DD      Offset Keyb_EnableKeyboard_vd        ;12h
   DD      Offset Keyb_DisableKeyboard_vd       ;13h

KEYB_Fce_Count :
   KEYB_Library_Name     DB    'keyb',0
   KEYB_Buffer_Size      DD    20+(160*2)  ; Velikost Bufferu 160 znaku
   KEYB_Buffers_Selector DW    0   ; selector na buffery konzoli
   KEYB_Tasks_Sleeping   DD    Max_Console_Count Dup (0)
   KEYB_Initialize_Flags equ   0000001000000010b
   KEYB_Initialize_Speed equ   000h
KEYB_Data_End :
KEYB_DATA_SIZE equ (((KEYB_DATA_END - KEYB_DATA_Start - 1) shr 12) +1)
;############################################################################
;# Datova structura standartiho vstupniho bufferu
;############################################################################
Standard_Input_Buffer Struc
  KB_Start equ $
     Keyb_Buffer_Start      dd 20
     Keyb_Buffer_End        dd 20 + 160
     Keyb_Buffer_Flags      dw 0000011000000010b ;Key_Down,Num_Lock
     Keyb_Buffer_Speed      dw 0101h
     Keyb_Buffer_Write_Pos  dd 20
     Keyb_Buffer_Read_Pos   dd 20
  KB_Size equ ($ - KB_Start)
Standard_Input_Buffer EndS
