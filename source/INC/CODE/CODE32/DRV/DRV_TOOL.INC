;############################################################################
;# Pomocne procedury pro praci s drivery
;############################################################################

; najde volnou polozku pro ulozeni informaci o nacitanem driveru
; Vstup: -
; Vystup:  CF = 0 - Ok
;                 - ES:EDI = ukazatel na volnou polozku
;          CF = 1 - Error
DRV_Find_Free Proc
      push   ecx ds es edi
       mov   ecx, data_descriptor
       mov   es, ecx
       mov   ecx, Max_DRVS
       mov   es, word ptr es:[sel_drivers_info]
       xor   edi, edi
DRV_Find_Free_loop:
       cmp   word ptr es:[edi+Device_Type], 0
       jz    short DRV_Find_Free_True
       add   edi, DRV_Device_Struc_Size
       dec   ecx
       jnz   short DRV_Find_Free_loop
DRV_Find_Free_False:
      pop    edi es ds
       stc   ;CF=1
       jmp   short DRV_Find_Free_End
DRV_Find_Free_True:
      pop    ecx ds ds ; edi es ds
       clc   ;CF=0
DRV_Find_Free_End:
      pop    ecx
       ret
DRV_Find_Free Endp

; uvolni dany driver z pameti
; Vstup: EAX - typove cislo driveru, ktery ma uvolnit
; Vystup:  CF = 0 - Ok
;          CF = 1 - Error
DRV_Free Proc
      push   es edi
       call  DRV_Find
       jc    short DRV_Free_false
       mov   word ptr es:[edi+Device_Type], 0
DRV_Free_true:
       clc   ;CF=0
       jmp   short DRV_Free_end
DRV_Free_false:
       stc   ;CF=1
DRV_Free_end:
      pop    edi es
       ret
DRV_Free Endp

; pokusi se najit polozku hledaneho driveru
; Vstup: EAX - typove cislo driveru, ktery se ma najit (pouze  lo(EAX) )
; Vystup:  CF = 0 - Ok
;                 - ES:EDI - ukazatel na polozku hledaneho driveru
;          CF = 1 - Error
DRV_Find Proc
      push   ecx ds es edi
       mov   ecx, data_descriptor
       mov   es, ecx
       mov   ecx, Max_DRVS
       mov   es, word ptr es:[sel_drivers_info]
       xor   edi, edi
DRV_Find_loop:
       cmp   word ptr es:[edi+Device_Type], ax
       jz    short DRV_Find_True
       add   edi, DRV_Device_Struc_Size
       dec   ecx
       jnz   short DRV_Find_loop
DRV_Find_False:
      pop    edi es ds
       stc   ;CF=1
       jmp   short DRV_Find_End
DRV_Find_True:
      pop    ecx ds ds ; edi es ds
       clc   ;CF=0
DRV_Find_End:
      pop    ecx
       ret
DRV_Find Endp

; vrati buffer s nazvy a typovymi cisly vsech nactenych driveru
; Vstup:   -
; Vystup:  CF = 0 - Ok
;                 - ECX - o kolika driverech jsou v bufferu ulozeny informace
;                 - ES:EDI - buffer s informacema o nactenych driverech
;                 - struktura polozky v bufferu
;                 - DW typove cislo driveru
;                 - retezec nazvu driveru zakonceny na 0
;                 - pak nasleduje dalsi polozka se stejnou strukturou
;          CF = 1 - Error
DRV_Ls_Drivers Proc
      push   eax ebx edx esi fs ds ecx edi es
       mov   ecx, data_descriptor
       mov   ds, ecx
       mov   edx, dword ptr ds:[drivers_count]
       mov   ds, word ptr ds:[sel_drivers_info]
      push   edx
       mov   eax, edx
       mov   ebx, 33+2
       mul   ebx
      pop    edx
       ; eax = drivers_count*35
       call  Alloc
       jc    DRV_Ls_Drivers_False
       mov   es, eax
       mov   ecx, Max_DRVS
       xor   esi, esi
       xor   edi, edi
DRV_Ls_Drivers_loop:
       cmp   word ptr ds:[esi+Device_Type], 0
       jz    short DRV_Ls_Drivers_loop_end

       ; zapiseme typove cislo driveru
       mov   ax, word ptr ds:[esi+Device_Type]
       stosw

       mov   fs, word ptr ds:[esi+Device_Sel_Data]
       movzx ebx, word ptr ds:[esi+Device_Func_Count]
       shl   ebx, 2
      push   ecx
       mov   ecx, 32
       ; FS:EBX - ukazatel na zacatek nazvu driveru
       ; ECX - maximalni delka nazvu driveru
DRV_Ls_Drivers_load_driver_name_loop:
       mov   al, byte ptr fs:[ebx]
       cmp   al, ' '
       jz    short DRV_Ls_Drivers_load_driver_name_write_null
       cmp   al, 0ah
       jz    short DRV_Ls_Drivers_load_driver_name_write_null
       stosb
       or    al, al
       jz    short DRV_Ls_Drivers_load_driver_name_loop_end
       inc   ebx
       dec   ecx
       jnz   short DRV_Ls_Drivers_load_driver_name_loop
DRV_Ls_Drivers_load_driver_name_write_null:
       xor   eax, eax
       stosb
DRV_Ls_Drivers_load_driver_name_loop_end:
      pop    ecx
DRV_Ls_Drivers_loop_end_0:
       dec   edx
       jz    short DRV_Ls_Drivers_loop_end_total
DRV_Ls_Drivers_loop_end:
       add   esi, DRV_Device_Struc_Size
       dec   ecx
       jnz   DRV_Ls_Drivers_loop ;short
DRV_Ls_Drivers_loop_end_total:
       mov   ecx, data_descriptor
       mov   ds, ecx
       mov   ecx, dword ptr ds:[drivers_count]
      pop    ds esi edx ; es edi ecx
       xor   edi, edi
       clc   ;CF=0
       jmp   short DRV_Ls_Drivers_end
DRV_Ls_Drivers_false_memory:
       mov   eax, es
      pop    es edi ecx
       call  DeAlloc
       stc   ;CF=1
DRV_Ls_Drivers_false:
      pop    es edi ecx
       stc   ;CF=1
DRV_Ls_Drivers_end:
      pop    ds fs esi edx ebx eax
       ret
DRV_Ls_Drivers Endp