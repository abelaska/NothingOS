;############################################################################
;# Rutiny pro praci se zarizenimi
;############################################################################

; cte sektory z disku
; Vstup:  ECX - kolik sektoru nacist
;         EDX - od kolikateho sektoru zacist cist
;         DS:ESI - ukazatel na polozku Mount_Struc
;         ES:EDI - vystupni buffer
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Read_Disk_Sectors Proc
       push   eax ebx
        mov   eax, dword ptr ds:[esi+VFS_MS_Device]
        mov   ebx,2
        call  drv_gate
       pop    ebx eax
        ret
Read_Disk_Sectors Endp

; cte sektory z disku
; Vstup:  ECX - kolik sektoru nacist
;         EDX - od kolikateho sektoru zacist cist
;         DS:ESI - ukazatel na polozku Mount_Struc
;         ES:EDI - vstupni buffer
; Vystup: CF = 0 - OK
;         CF = 1 - Error
Write_Disk_Sectors Proc
       push   eax ebx
        mov   eax, dword ptr ds:[esi+VFS_MS_Device]
        mov   ebx,3
        call  drv_gate
       pop    ebx eax
        ret
Write_Disk_Sectors Endp

;##########################################################
; Prectez disku blok dat o velikosti diskoveho bloku
; procedura specialne upravena pro page fault nacitani z disku
;##########################################################
; Vstup:  ES:EDI - Buffer pro ulozeni dat
;         DS:ESI - ukazatel na polozku Mount_Struc
;         EAX    - cislo bloku
; Vystup: CF     - 0 = OK
;                - EDI = EDI + velikost diskoveho bloku
;         CF     - 1 = Error
Read_disk_block Proc

; testovat, jestli je zarizeni namontovane i pro zapis

       push   eax ebx ecx edx
        ; zjistime pocatecni pozici bloku v sektorech
       push   ds
        mov   ds, word ptr ds:[esi+VFS_MS_Cache_Area]
        add   eax, dword ptr ds:[Ext2_First_Data_Block]
        mov   ecx, eax
        mov   eax, dword ptr ds:[Ext2_Block_Size]
        mov   ebx, 512
        xor   edx, edx
        div   ebx
       push   eax
       ;eax=kolik sektoru tvori jeden blok
        xor   edx, edx
        mul   ecx
        mov   edx, eax
       pop    ecx
        mov   eax, dword ptr ds:[Ext2_Block_Size]
       pop    ds
       push   eax
        mov   eax, dword ptr ds:[esi+VFS_MS_Device]
        mov   ebx,2
        call  drv_gate
       pop    eax
        jc    short Read_disk_block_false
        add   edi, eax
        clc ;CF=0
        jmp   short Read_disk_block_End
Read_disk_block_false:
        stc ;CF=1
Read_disk_block_End:
       pop    edx ecx ebx eax
       ret
Read_disk_block Endp

;##########################################################
; Zapise na disk blok dat o velikosti diskoveho bloku
;##########################################################
; Vstup:  ES:EDI - Vstupni Buffer
;         DS:ESI - ukazatel na polozku Mount_Struc
;         EAX    - cislo bloku
; Vystup: CF     - 0 = OK
;         CF     - 1 = Error
Write_disk_block Proc
       push   eax ebx ecx edx edi
        ; zjistime pocatecni pozici bloku v sektorech
       push   ds
        mov   ds, word ptr ds:[esi+VFS_MS_Cache_Area]
        add   eax, dword ptr ds:[Ext2_First_Data_Block]
        mov   ecx, eax
        mov   eax, dword ptr ds:[Ext2_Block_Size]
        mov   ebx, 512
        xor   edx, edx
        div   ebx
       push   eax
        ;eax=kolik sektoru tvori jeden blok
        mul   ecx
        mov   edx, eax
       pop    ecx
       pop    ds
        mov   eax, dword ptr ds:[esi+VFS_MS_Device]
        mov   ebx, 3
        call  drv_gate
        jc    short Write_disk_block_false
        clc   ;CF=0
        jmp   short Write_disk_block_End
Write_disk_block_false:
        stc   ;CF=1
Write_disk_block_End:
       pop    edi edx ecx ebx eax
        ret
Write_disk_block Endp
