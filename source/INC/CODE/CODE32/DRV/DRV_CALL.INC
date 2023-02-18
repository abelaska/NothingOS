;############################################################################
;# Rutiny pro volani driveru
;############################################################################

; spusti init rutiny u vsech nactenych driveru
DRV_Init Proc
      push   ebp
       mov   ebp, offset DRV_Init_Rutine
       call  DRV_Call_All
      pop    ebp
       ret
DRV_Init Endp

; spusti done rutiny u vsech nactenych driveru
DRV_Done Proc
      push   ebp
       mov   ebp, offset DRV_Done_Rutine
       call  DRV_Call_All
      pop    ebp
       ret
DRV_Done Endp

; univerzalni fce pro volani funkci jednotlivych driveru
; Vstup: EAX - zarizeni (typ driveru)
;        EBX - cislo fce. driveru
;        < EAX,EBX,GS > POUZIVA DRV_GATE
;        ECX,EDX,EDI,ESI,EBP,DS,ES,FS - pro pouziti fci driveru,
;        fce. driveru muze vracet hodnoty i v eax a ebx, gs budiz zapovezeno
;        ostatni je rezervovane pro jine pouziti
; Vystup: < CF > rezervovano pro drv_gate
;        if CF=1 then EAX=error code
;         EAX - error code
;          1  - device not found
;          2  - fce driveru nenalezena
;          3  - driver jeste nebyl inicializovan
;          4  - driver jiz byl inicializovan
;        else vsechny registry jsou vystupem volane fce driveru
DRV_Gate Proc
        push  gs es edi ecx
         call DRV_Find
         jnc  short DRV_Gate_find_driver_entry_found
         ; driver nebyl nalezen
        pop   ecx edi es
         ; driver nenalezen
         mov  eax, 1
         stc  ; CF = 1
         jmp  DRV_Gate_end

DRV_Gate_find_driver_entry_found:
        ; EBX=cislo fce driveru
        push  fs
         mov  fs, word ptr es:[edi+Device_Sel_Data]
         inc  ebx
         cmp  word ptr es:[edi+Device_Func_Count], bx
         jge  short DRV_Gate_find_driver_entry_found_0
        pop   fs
        pop   ecx
        pop   edi
        pop   es
         ; fce driveru nenalezena
         mov  eax, 2
         stc  ; CF = 1
         jmp  DRV_Gate_end
DRV_Gate_find_driver_entry_found_0:
         dec  ebx

         ; vola se fce INIT ?
         or   ebx, ebx
         jz   short DRV_Gate_driver_INIT
         ; vola se fce DONE ?
         cmp  ebx, 1
         jz   short DRV_Gate_driver_DONE
         ; vola se jina fce
         jmp  short DRV_Gate_driver_ELSE

DRV_Gate_driver_INIT:
         mov  cx, word ptr es:[edi+Device_Semafor]
         and  cx, 1
         cmp  cx, 1
         jnz  short DRV_Gate_driver_INIT_OK
        pop   fs
        pop   ecx
        pop   edi
        pop   es
         ; driver jiz byl inicializovan
         mov  eax, 4
         stc  ; CF = 1
         jmp  short DRV_Gate_end
DRV_Gate_driver_INIT_OK:
         or   word ptr es:[edi+Device_Semafor], 1
         jmp  short DRV_Gate_driver_ELSE
DRV_Gate_driver_DONE:
         mov  cx, word ptr es:[edi+Device_Semafor]
         and  cx, 1
         or   cx, cx
         jnz  short DRV_Gate_driver_DONE_OK
        pop   fs
        pop   ecx
        pop   edi
        pop   es
         ; driver jeste nebyl inicializovan
         mov  eax, 3
         stc  ; CF = 1
         jmp  short DRV_Gate_end
DRV_Gate_driver_DONE_OK:
         and  word ptr es:[edi+Device_Semafor], 0FFFEh
DRV_Gate_driver_ELSE:
         shl  ebx, 2
         mov  ax, word ptr es:[edi+Device_Sel_Code]
         mov  ebx, dword ptr FS:[ebx]

        pop   fs
        pop   ecx
        pop   edi
        pop   es
        PUSH  eax   ; selector
        PUSH  ebx   ; offset adr.
         call fword ptr SS:[ESP]
         add  esp, 2*4
         clc ; CF=0
DRV_Gate_end:
        pop   gs
         ret
DRV_Gate Endp

; zavola rutinu EBP pro vsechny nactene drivery
; Vstup:  EBP - offsetova pozice rutinky v CS kernelu
; Vystup: -
DRV_Call_All Proc
       pushad
       push  gs fs ds es
       mov   ecx, data_descriptor
       mov   ds, ecx
       mov   edx, dword ptr ds:[drivers_count]
       mov   ds, word ptr ds:[sel_drivers_info]

       mov   eax, edx
       shl   eax, 1
       call  Alloc
       jc    DRV_Call_All_False
       mov   es, eax

       mov   ecx, Max_DRVS
       xor   esi, esi
       xor   edi, edi
       xor   ebx, ebx
       cld
DRV_Call_All_loop:
       cmp   word ptr ds:[esi+Device_Type], 0
       jz    short DRV_Call_All_loop_end
       mov   ax, word ptr ds:[esi+Device_Semafor]
       and   ax, 1
       cmp   ax, 1
       jz    short DRV_Call_All_loop_end_0
       mov   ax, word ptr ds:[esi+Device_Type]
       stosw
       inc   ebx
DRV_Call_All_loop_end_0:
       dec   edx
       jz    short DRV_Call_All_loop_end_total
DRV_Call_All_loop_end:
       add   esi, DRV_Device_Struc_Size
       dec   ecx
       jnz   short DRV_Call_All_loop
DRV_Call_All_loop_end_total:
      push   es
      pop    ds
       xor   edi, edi
       xor   esi, esi
       mov   ecx, ebx
       ; ECX - kolik driveru bylo nalezeno
       ; ES:EDI - buffer s typ. cislama driveru
       ; DS:ESI - buffer s typ. cislama driveru
      pushad
      push   es
       call  ebp
      pop    es
      popad
DRV_Call_All_end_true:
       mov   eax, es
      pop    es ds
       call  DeAlloc
       clc   ;CF=0
       jmp   short DRV_Call_All_end
DRV_Call_All_False:
      pop    es ds
       stc   ;CF=1
DRV_Call_All_end:
      pop    fs gs
      popad
        ret
DRV_Call_All Endp

; rutina pro inicializaci driveru
DRV_Init_Rutine Proc
       ; tady sesortuju typove cisla od nejmensiho po nejvetsi

       ; inicializujeme nalezeny drivery
       xor   esi, esi
DRV_Init_init_loop:
       xor   eax, eax
       lodsw
       or    eax, eax
       jz    short DRV_Init_Rutine_end
      push   esi ecx es ds
       mov   ebx, Drv_Std_INIT
       call  drv_gate
      pop    ds es ecx esi
       dec   ecx
       jnz   short DRV_Init_init_loop
DRV_Init_Rutine_end:
       ret
DRV_Init_Rutine endp

; rutina pro deinicializaci driveru
DRV_Done_Rutine Proc
       ; tady sesortuju typove cisla od nejvetsiho po nejmensi

       ; inicializujeme nalezeny drivery
       xor   esi, esi
DRV_Done_Done_loop:
       xor   eax, eax
       lodsw
       or    eax, eax
       jz    short DRV_Done_Rutine_end
      push   esi ecx es ds
       mov   ebx, Drv_Std_DONE
       call  drv_gate
      pop    ds es ecx esi
       dec   ecx
       jnz   short DRV_Done_Done_loop
DRV_Done_Rutine_end:
       ret
DRV_Done_Rutine endp
