;############################################################################
;# DRIVER PRO EXT2FS
;############################################################################

EXT2_INIT proc
       pushad
       push   ds es
        ; Zaregistrovani souboroveho systemu ext2
        mov   ax, Data_descriptor
        mov   ds, ax
        mov   es, ax
        mov   edi, offset FS_EXT2_NAME
        mov   esi, offset FS_EXT2_Operations_Struc
        mov   eax, cs
        mov   ebx, EXT2_Driver_driver
        call  FS_Register
       pop    es ds
       popad
        retf
EXT2_INIT Endp

EXT2_DONE proc
       pushad
        ; odregistrovani souboroveho systemu ext2
        mov   eax, EXT2_Driver_driver
        call  FS_UnRegister
       popad
        retf
EXT2_DONE Endp