;############################################################################
;# Promenne kernelu
;############################################################################

;########################################################################
;# Pomocne promenen pro vypis na obrazovku
;########################################################################
New_Line_Txt            db '\n',0
Num_txt                 db '%h4'
                        dd offset Num_Txt_Num
                        db ' ',0
Num_Txt_Num             DD 0
;########################################################################
;# Zakladni informace o compu
;########################################################################
Processor_Type          db 0
Processor_Speed_hi      dd 0
Processor_Speed_low     dd 0
Processor_Speed         dd 0
_vendor_id              dd 3 dup (0)
_intel_CPU              db 0
;########################################################################
System_Timer            DD 0  ; pocitadlo pruchodu casovace :)
Timer_Hook_ATA          DD 0  ; Promenna, kterou casovac pri kazdem pruchodu decrementuje
;########################################################################
Begin_Cursor_Pos_Y      db 16
; Tabulka zanaku pro konverzi cisla na znak
Numbers_Tab             db '0123456789ABCDEF'
; Tabulka radu pro prevod DD cisla na DEC string
DEC_Numbers      dd 0000000001
                 dd 0000000010
                 dd 0000000100
                 dd 0000001000
                 dd 0000010000
                 dd 0000100000
                 dd 0001000000
                 dd 0010000000
                 dd 0100000000
                 dd 1000000000
;Tabulka pro prevod z decadickeho cisla 0..7 na 00000001b...10000000b
SetBit_Table1 :
                 db 00000001b
                 db 00000010b
                 db 00000100b
                 db 00001000b
                 db 00010000b
                 db 00100000b
                 db 01000000b
                 db 10000000b

;Tabulka pro prevod z decadickeho cisla 0..7 na 10000000b...00000001b - reverzni
SetBit_Table2 :
                 db 10000000b
                 db 01000000b
                 db 00100000b
                 db 00010000b
                 db 00001000b
                 db 00000100b
                 db 00000010b
                 db 00000001b

SetBit_Table2_Non :
                 db 01111111b
                 db 10111111b
                 db 11011111b
                 db 11101111b
                 db 11110111b
                 db 11111011b
                 db 11111101b
                 db 11111110b


;Tabulka pro prevod z decadickeho cisla 0..7 na johnsonuv kod
JohnBit_Table1 :
                 db 00000001b
                 db 00000011b
                 db 00000111b
                 db 00001111b
                 db 00011111b
                 db 00111111b
                 db 01111111b
                 db 11111111b

;Tabulka pro prevod z decadickeho cisla 0..7 na johnsonuv kod - reversni
JohnBit_Table2 :
                 db 10000000b
                 db 11000000b
                 db 11100000b
                 db 11110000b
                 db 11111000b
                 db 11111100b
                 db 11111110b
                 db 11111111b

; Buffer pro ulozeni vysledku konverze Cisla na String
X2Str_Buffer     db 32 dup(0)

