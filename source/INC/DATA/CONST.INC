;############################################################################
;# Konstanty Kernelu
;############################################################################

;cisla descriptoru
Data_Descriptor               equ 8
Code_Descriptor               equ 8+Data_Descriptor
Stack_Descriptor              equ 8+Code_Descriptor
Stack_Descriptor_Helper       equ 8+Stack_Descriptor
IDT_Descriptor                equ 8+Stack_Descriptor_Helper
B8000_Descriptor              equ 8+IDT_Descriptor
A0000_Descriptor              equ 8+B8000_Descriptor
GDT_Descriptor                equ 8+A0000_Descriptor
FysPageInfoItem_Descriptor    equ 8+GDT_Descriptor
Shared_Mem_Descriptor         equ 8+FysPageInfoItem_Descriptor
FysLin_Mem_Descriptor         equ 8+Shared_Mem_Descriptor
Pages_Descriptor              equ 8+FysLin_Mem_Descriptor
IDE_DATA_Descriptor           equ 8+Pages_Descriptor
EXEC_DATA_Descriptor          equ 8+IDE_DATA_Descriptor
CONS_DATA_Descriptor          equ 8+EXEC_DATA_Descriptor
KEYB_DATA_Descriptor          equ 8+CONS_DATA_Descriptor
BIOS_Area_Descriptor          equ 8+KEYB_DATA_Descriptor
EXT2_DATA_Descriptor          equ 8+BIOS_Area_Descriptor
Last_Chance_Descriptor        equ 8+EXT2_DATA_Descriptor
; posleni descriptor
First_Avaible_Descriptor      equ 8+Last_Chance_Descriptor

; velikosti descriptoru, page=4096 B
Data_Pages              equ (Data_Size / 4096)
Code_Pages              equ (Code_Size / 4096)+3
Stack_Pages             equ 4
GDT_Pages               equ 16
IDT_Pages               equ 1
; base offset hodnoty pameti alokovane descriptory
Data_Base      equ 0
Code_Base      equ Data_Pages*4096
Stack_Base     equ Code_Base+Code_Pages*4096
GDT_Base       equ Stack_Base+Stack_Pages*4096
IDT_Base       equ GDT_Base+GDT_Pages*4096
MemTab1_Base   equ IDT_Base+IDT_Pages*4096

; attributy descriptoru
Data_Atr1   equ 10010010b
Data_Atr2   equ 11000000b
Code_Atr1   equ 10011000b
Code_Atr2   equ 11000000b
Stack_Atr1  equ 10010110b
Stack_Atr2  equ 11000000b
IDT_Atr1    equ 10010010b
IDT_Atr2    equ 11000000b
MemTab_Atr1 equ 10010010b
MemTab_Atr2 equ 11000000b
INT_Atr1    equ 10001111b
INT_Atr2    equ 11101111b
TSS_Atr1    equ 10000101b

Data_Atr1_CPL3  equ 11110010b
