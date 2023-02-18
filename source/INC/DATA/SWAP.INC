;#####################################################################################
;# Promenne swap demona
;#####################################################################################

Swap_Partition_Descriptor  dd 0  ;# obsahuje bitmapu vyuziti swap partition
Swap_Partition_Size        dd 0  ;# velikost swap partition ve 4k blocich
Swap_Partition_Free        dd 0  ;# pocet volnych 4k bloku
Swap_Partition_Size_MB     dd 0  ;# velikost swap partition v MB

Swap_Style                 db 0  ;# urcuje zpusob spusteni Swap Deamona
Swap_Mode_Pasive           equ  0
Swap_Mode_Active           equ  1