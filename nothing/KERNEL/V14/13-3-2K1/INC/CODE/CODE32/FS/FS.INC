; Inicializuje prostredi pro FS
; Inicializuje prostredi pro FS
FS_Init Proc
        mov   ax, Data_descriptor
        mov   ds, ax
        ; alokuje pamet pro FS_Infos
        mov   eax, Max_VFS_FS_Infos*VFS_FS_Infos_Size
        call  kAlloc
        mov   word ptr ds:[Sel_VFS_FS_Infos], ax

        ; alokuje pamet pro FS_Operations
        mov   eax, Max_FS_Operations*FS_Operations_Size
        call  kAlloc
        mov   word ptr ds:[Sel_FS_Operations], ax

        Print  FS_Init_txt
        ret
FS_Init Endp

; DeInicializuje prostredi pro FS
FS_Done Proc
       push   ds eax
        mov   ax, Data_descriptor
        mov   ds, ax
       pop    eax ds
        ret
FS_Done Endp

; najde volnou polozku ve strukture FS_Operations
; Vstup:  -
; Vystup:  CF = 0 - OK
;                 - ES:EDI - ukazatel na volnou polozku
;          CF = 1 - Error
FS_Find_Free_Register_Operations Proc
       push  ecx ds edi es
        mov  ecx, data_descriptor
        mov  ds, ecx
        mov  es, word ptr ds:[Sel_FS_Operations]
        ; posune ukazatel na 1. volnou pozici
        mov  ecx, Max_FS_Operations
        xor  edi, edi
FS_Find_Free_Register_Operations_loop:
        cmp  word ptr es:[edi+FS_Driver_Type_Number], 0
        jz   short FS_Find_Free_Register_Operations_true
        add  edi, FS_Operations_Size
        dec  ecx
        jnz  short FS_Find_Free_Register_Operations_loop
FS_Find_Free_Register_Operations_false:
       pop   es edi
        stc  ;CF=1
        jmp  short FS_Find_Free_Register_Operations_end
FS_Find_Free_Register_Operations_true:
       pop   ds ecx ; es edi
        clc  ;CF=0
FS_Find_Free_Register_Operations_end:
       pop   ds ecx
        ret
FS_Find_Free_Register_Operations Endp

; najde volnou polozku ve strukture VFS_FS_Infos
; Vstup:  -
; Vystup:  CF = 0 - OK
;                 - ES:EDI - ukazatel na volnou polozku
;          CF = 1 - Error
FS_Find_Free_Register_Structures Proc
       push  ecx ds edi es
        mov  ecx, data_descriptor
        mov  ds, ecx
        mov  es, word ptr ds:[Sel_VFS_FS_Infos]
        ; posune ukazatel na 1. volnou pozici
        mov  ecx, Max_VFS_FS_Infos
        xor  edi, edi
FS_Find_Free_Register_Structures_loop:
        cmp  dword ptr es:[edi+VFS_FS_Name], 0
        jz   short FS_Find_Free_Register_Structures_true
        add  edi, VFS_FS_Infos_Size
        dec  ecx
        jnz  short FS_Find_Free_Register_Structures_loop
FS_Find_Free_Register_Structures_false:
       pop   es edi
        stc  ;CF=1
        jmp  short FS_Find_Free_Register_Structures_end
FS_Find_Free_Register_Structures_true:
       pop   ds ecx ; es edi
        clc  ;CF=0
FS_Find_Free_Register_Structures_end:
       pop   ds ecx
        ret
FS_Find_Free_Register_Structures Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Zaregistrovani funkci souboroveho systemu
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Vstup:  DS:ESI - ukazatel na strukturu Driver_FS_Operations_Struc
;         AX     - selector na code segment driveru, kde je ulozen kod fci
;         BX     - typove cislo driveru, ve kterem jsou ulozny tyto fce
; Vystup: CF = 0 - OK
;                - ES:EDI = ukazatel na polozku, kterou prave zalokoval
;         CF = 1 - Error
FS_Register_Operations Proc
       push  eax ebx ecx edx esi ds edi es
        ; najdeme volnou polozku
        call FS_Find_Free_Register_Operations
        jc   short FS_Register_Operations_False
       push  edi ebx
        mov  ebx, eax
        mov  ecx, Driver_FS_Operations_Count
FS_Register_Operations_Loop:
        lodsd
        stosd ; zapise offset
        mov  eax, ebx
        stosw ; zapise selector
        dec  ecx
        jnz  short FS_Register_Operations_Loop
       pop   eax edi
        mov  word ptr es:[edi+FS_Driver_Type_Number], ax
        ; zvysime pocitadlo alokovanych polozek
        mov  ax, data_descriptor
        mov  ds, ax
        inc  dword ptr ds:[Count_FS_Operations]
       pop   ds esi ; es edi
        clc ;CF=0
        jmp  short FS_Register_Operations_End
FS_Register_Operations_False:
       pop   es edi
        stc ; CF=1
FS_Register_Operations_End:
       pop   ds esi edx ecx ebx eax
        ret
FS_Register_Operations Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Zaregistrovani struktur souboroveho systemu
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Vstup:  DS:ESI - ukazatel na jmeno FS, zakoncene na 0
;         FS:EBX - ukazatel na polozku FS_Operations prirazene danemu FS
; Vystup: CF = 0 - OK
;         CF = 1 - Error
FS_Register_Structures Proc
       push  es
       pushad
       push  fs ebx
        call FS_Find_Free_Register_Structures
       pop   ebx fs
        jc   FS_Register_Structures_false
        ; prekopiruje nazev FS a do konce retezce vyplni mezerama
       push  edi
        add  edi, VFS_FS_Name
        mov  ecx, Max_VFS_FS_Name_Length
        cld
FS_Register_Structures_Loop:
        lodsb
        or   al, al
        jz   short FS_Register_Structures_Loop_End
        stosb
        dec  ecx
        jnz  short FS_Register_Structures_Loop
        jmp  short FS_Register_Structures_Loop_End_2
FS_Register_Structures_Loop_End:
        mov  al, ' '
        rep  stosb
FS_Register_Structures_Loop_End_2:
       pop   edi

        ; vyplnujeme polozku VFS_FS_Infos
        ; flagy FS (zatim neurceno)
        mov  dword ptr es:[edi+VFS_FS_Flags], 0
        mov  dword ptr es:[edi+VFS_FS_Count_of_Use], 0
        ; FS:EBX - ukazatele na prirazenou FS_Operations polozku

        ; ukazatel na tabulku funkci pro praci se super blokem
        mov  ecx, ebx
        add  ecx, FS_Super_Ops
        mov  dword ptr es:[edi+VFS_FS_Super_Ops], ecx
        mov  word ptr  es:[edi+VFS_FS_Super_Ops+4], fs
        ; ukazatel na tabulku fukci pro praci s inodama
        mov  ecx, ebx
        add  ecx, offset FS_Inode_Ops
        mov  dword ptr es:[edi+VFS_FS_Inode_Ops], ecx
        mov  word ptr  es:[edi+VFS_FS_Inode_Ops+4], fs
        ; ukazatel na tabulku pro praci se souborama
        mov  ecx, ebx
        add  ecx, offset FS_File_Ops
        mov  dword ptr es:[edi+VFS_FS_File_Ops], ecx
        mov  word ptr  es:[edi+VFS_FS_File_Ops+4], fs

        ; zvysime pocitadlo alokovanych polozek
        mov  ax, data_descriptor
        mov  es, ax
        inc  dword ptr es:[Count_VFS_FS_Infos]
FS_Register_Structures_true:
       clc   ;CF=0
       jmp   FS_Register_Structures_end
FS_Register_Structures_false:
       stc   ;CF=1
FS_Register_Structures_end:
       popad
       pop   es
        ret
FS_Register_Structures Endp

; Zaregistrovani souboroveho systemu
; Vstup:  DS:ESI - data o strukture Driver_FS_Operations_Struc
;         AX     - selector na code segment driveru, kde je ulozen kod fci
;         BX     - typove cislo driveru, ve kterem jsou ulozny tyto fce
;         ES:EDI - ukazatel na jmeno FS, zakoncene na 0
; Vystup: CF = 0 - OK
;         CF = 1 - Error
FS_Register Proc
       pushad
       push   ds es fs
       push   es edi
        call  FS_Register_Operations
       pop    esi ds
       push   es
       pop    fs
        jc    short FS_Register_false
        mov   ebx, edi
        call  FS_Register_Structures
        jc    short FS_Register_false
FS_Register_true:
        clc   ;CF=0
        jmp   short FS_Register_End
FS_Register_false:
        stc   ;CF=1
FS_Register_End:
       pop    fs es ds
       popad
        ret
FS_Register Endp

; Odregistrovani souboroveho systemu
; Vstup:  EAX    - typove cislo driveru, ve kterem jsou fce pro FS
; Vystup: CF = 0 - OK
;         CF = 1 - Error
FS_UnRegister Proc
      push   es edi
       call  FS_Find_Register_Structures
       jc    short FS_UnRegister_false
       cmp   word ptr es:[edi+VFS_FS_Count_of_Use], 0
       jz    short FS_UnRegister_0
       dec   word ptr es:[edi+VFS_FS_Count_of_Use]
       jmp   short FS_UnRegister_true
FS_UnRegister_0:
       mov   dword ptr es:[edi+VFS_FS_Name], 0
       lds   esi, fword ptr es:[edi+VFS_FS_Operations_Addr]
       mov   word ptr ds:[esi+FS_Driver_Type_Number], 0
       jmp   short FS_UnRegister_true
FS_UnRegister_true:
       clc   ;CF=0
       jmp   short FS_UnRegister_end
FS_UnRegister_false:
       stc   ;CF=1
FS_UnRegister_end:
      pop    edi es
       ret
FS_UnRegister Endp

; najde polozku zastupujici registrovany FS podle typoveho cisla driveru
; ktery tento FS zaregistroval
; Vstup:  EAX    - typove cislo driveru, ve kterem jsou fce pro FS
; Vystup: CF = 0 - OK
;                - ES:EDI = ukazatel na hledanou polozku VFS_FS_Infos
;         CF = 1 - Error
FS_Find_Register_Structures Proc
       push  eax ebx ecx edx ds esi edi es
        mov  ecx, data_descriptor
        mov  ds, ecx
        mov  es, word ptr ds:[Sel_VFS_FS_Infos]
        ; posune ukazatel na 1. volnou pozici
        mov  ecx, Max_VFS_FS_Infos
FS_Find_Register_Structures_loop:
        cmp  dword ptr es:[edi+VFS_FS_Name], 0
        jz   short FS_Find_Register_Structures_loop_end
        lds  esi, fword ptr es:[edi+VFS_FS_Operations_Addr]
        cmp  word ptr ds:[esi+FS_Driver_Type_Number], ax
        jz   short FS_Find_Register_Structures_true
FS_Find_Register_Structures_loop_end:
        add  edi, VFS_FS_Infos_Size
        dec  ecx
        jnz  short FS_Find_Register_Structures_loop
FS_Find_Register_Structures_false:
       pop   es edi
        stc  ;CF=1
        jmp  short FS_Find_Register_Structures_end
FS_Find_Register_Structures_true:
       pop   ds ecx ; es edi
        clc  ;CF=0
FS_Find_Register_Structures_end:
       pop   esi ds edx ecx ebx eax
        ret
FS_Find_Register_Structures Endp

; Hleda informacni polozku pro dany FS
; Vstup:  DS:ESI - nazev souboroveho systemu, max delka je 10 znaku, podporuje se ukonceni na 0(napr. pri delce nazvu 9 znaku a nize)
; Vystup: CF  = 0 - na tuto inodu je namontovano zarizeni
;             = DS:ESI - adresa ukazujici na zacatek dane polozky v FS_Infos tabulce
;         CF  = 1 - tento FS neni podporovan
FS_Find_FS Proc
       push   es
       push   edi
       push   ebx
       push   esi

        xor   ebx, ebx
        mov   bx, data_descriptor
        mov   es, bx
        mov   ebx, dword ptr es:[Count_VFS_FS_Infos]

        or    ebx, ebx
        jz    FS_Find_FS_error

        mov   es, word ptr es:[Sel_VFS_FS_Infos]
        mov   edi, VFS_FS_Name

        mov   ebx, Max_VFS_FS_Infos

        ;zjisti delku hledaneho nazvu
       push   ebx
       push   esi
        mov   ebx, Max_VFS_FS_Name_Length
FS_Find_FS_check_len:
        cmp   byte ptr ds:[esi], 0
        jz    short FS_Find_FS_len_checked
        cmp   byte ptr ds:[esi], ' '
        jz    short FS_Find_FS_len_checked
        inc   esi
        dec   ebx
        jnz   short FS_Find_FS_check_len
FS_Find_FS_len_checked:
        mov   ecx, Max_VFS_FS_Name_Length
        sub   ecx, ebx
       pop    esi
       pop    ebx
        ; ECX = delka hledaneho nazvu

FS_Find_FS_find:
        cmp  dword ptr es:[edi], 0
        jz   short FS_Find_FS_find_end
       push  edi
       push  esi
       push  ecx
        cld
        rep  cmpsb
       pop   ecx
       pop   esi
       pop   edi
        jz   short FS_Find_FS_found
FS_Find_FS_find_end:
        add  edi, VFS_FS_Infos_Size
        dec  ebx
        jnz  short FS_Find_FS_find
        jmp  short FS_Find_FS_error
FS_Find_FS_found:
       pop   ebx ; esi
        mov  esi, edi
        mov  bx, es
        mov  ds, bx
       pop   ebx
       pop   edi
       pop   es
        clc ; CF=0
        ret
FS_Find_FS_error:
       pop   ebx
       pop   edi
       pop   esi
       pop   es
        stc ; CF=1
        ret
FS_Find_FS Endp
