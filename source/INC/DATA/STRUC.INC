;############################################################################
;# Datove structury kernelu
;############################################################################

;############################################################################
;# Datova structura obecneho Descriptoru
;############################################################################
Descriptor      Struc
    Limit       dw      0;+0
    Base_Lo     dw      0;+2
    Base_Hi_Lo  db      0;+4
    Atr1        db      0;+5
    Atr2        db      0;+6
    Base_Hi_Hi  db      0;+7
Descriptor      Ends
;############################################################################
;# Datova structura obecneho Descriptoru preruseni
;############################################################################
IDT_struct Struc
    IDT_struc_Offset_LO dw 00h
    IDT_struc_selector  dw 00h
    IDT_struc_reserved  db 00h
    IDT_struc_flags     db 87h
    IDT_struc_Offset_Hi dw 00h
IDT_struct Ends
