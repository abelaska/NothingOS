;############################################################################
;# Rutiny pro praci s VFS
;############################################################################

; Inicializuje VFS
VFS_Init Proc
        mov   ax, Data_descriptor
        mov   ds, ax
        mov   es, ax
        ; alokuje pamet pro mount_strucs
        mov   eax, Max_VFS_Mount_Struc*VFS_Mount_Struc_Size
        call  kAlloc
        mov   word ptr ds:[Sel_VFS_Mount_Struc], ax

        ; alokuje pamet pro VFS_Inode
        mov   eax, Max_VFS_Inode*VFS_Inode_Size
        call  kAlloc
        mov   word ptr ds:[Sel_VFS_Inode], ax

        ; alokuje pamet pro pomocny buffer pro praci s blokovyma zarizenima
        mov   eax, 65536
        call  kAlloc
        mov   word ptr ds:[Sel_VFS_Temp_Buffer], ax

        Print VFS_Init_txt
        ret
VFS_Init Endp

; namontuje root partition na koren
VFS_Mount_Root Proc
       pushad
       push ds es fs
        mov   ax, Data_Descriptor
        mov   ds, ax
        ; cislo typu driveru+cislo zarizeni
        mov   ebx, dword ptr DS:[Root_Partition_Num]
        ; montujeme jen pro cteni
        mov   ecx, 1000b ; Read only+Pristup povolen+Remount zakazan
        ; cislo VFS inody, na kterou montujeme
        mov   eax, 0
        ; ukazatel na jmeno FS
        mov   esi, offset FS_EXT2_NAME
        ; DS:ESI -> 'EXT2',0
        call  VFS_Mount_Device_Core
        jc    VFS_Mount_Root_cant_mount_root
        ; otevre VFS inodu 0 pro inodu 2 na korenove partition
        xor   eax, eax
        call  VFS_Find_Mounted_Inode
        jc    short VFS_Mount_Root_cant_mount_root
        mov   eax, 2
        xor   ebx, ebx
        mov   edx, 1
        call  VFS_Open_Main
        jc    short VFS_Mount_Root_cant_mount_root
       pop fs es ds
       popad
        Print  Root_Mounted_txt
        ret
VFS_Mount_Root_cant_mount_root:
        mov   eax, Er_mounting_root
        Call  Fatal_Error
VFS_Mount_Root Endp

; Zjisti, jestli je na dane inode (na rootu) namontovano nejake zarizeni
; Vstup:  EAX - VFS inoda
; Vystup: CF  = 0 - na tuto inodu je namontovano zarizeni
;             = DS:ESI - adresa ukazujici na zacatek dane polozky v mount tabulce
;         CF  = 1 - inoda v tabulce mount nebyla nalezena
;         EAX - nezmeneno
VFS_Find_Mounted_Inode Proc
       push gs ebx esi ds
        xor   ebx, ebx
        mov   bx, data_descriptor
        mov   ds, bx
        mov   ebx, dword ptr ds:[Count_VFS_Mount_Struc]
        ; testneme, jestli je vubec neco namontovano
        or    ebx, ebx
        jz    short VFS_Find_Mounted_Inode_not_found
        mov   ds, word ptr ds:[Sel_VFS_Mount_Struc]
        xor   esi, esi
VFS_Find_Mounted_Inode_loop:
        cmp   dword ptr ds:[esi+VFS_MS_VFS_Inode], eax
        jz    short VFS_Find_Mounted_Inode_found
        add   esi, VFS_Mount_Struc_Size
        dec   ebx
        jnz   short VFS_Find_Mounted_Inode_loop
VFS_Find_Mounted_Inode_not_found:
       pop    ds esi
        stc   ;CF=1
        jmp   short VFS_Find_Mounted_Inode_end
VFS_Find_Mounted_Inode_found:
       pop    gs ebx ;ds esi
        clc ;CF=0
VFS_Find_Mounted_Inode_end:
       pop    ebx gs
        ret
VFS_Find_Mounted_Inode Endp

; namontuje na inodu zarizeni (pro pouziti v hlavni procedure Mount_Device)
; Vstup: EAX - VFS Inoda, na kterou se ma zarizeni namontovat
;        EBX - cislo typu zarizeni potrebneho pro praci se zarizenim
;        ECX - Attributy namontovani
;        DS:ESI - nazev souboroveho systemu, max delka je 10 znaku, podporuje se ukonceni na 0(napr. pri delce nazvu 9 znaku a nize)
; Vystup:
;        CF = 0 - OK
;           = DS:ESI - ukazatel na alokovanou polozku mount struc
;        CF = 1 - Error
;           = EAX - Error kod
;                 = 0 - Vice zarizeni nemuze byt namontovano
;                 = 1 - Na teto inode jiz je nejake zarizeni namontovane
;                 = 2 - potrebny FS nenalezen
;                 = 3 - chyba pri cteni superbloku
VFS_Mount_Device_Core Proc
       push   es gs ebx ecx edx edi ds esi eax
        ; zjisti, jestli uz inoda nebyla pouzita pro namontovani jineho zarizeni
       push   ds esi
        call  VFS_Find_Mounted_Inode
       pop    esi ds
        jnc   VFS_Mount_Device_Core_Error_2
        ; najdeme volnou polozku v mount tab.
       push   ecx
        mov   cx, data_descriptor
        mov   es, cx
        mov   es, word ptr es:[Sel_VFS_Mount_Struc]

        xor   edi, edi
        mov   ecx, Max_VFS_Mount_Struc
VFS_Mount_Device_Core_find_free_mount_struc:
        cmp   dword ptr es:[edi+VFS_MS_VFS_Inode], 0
        jz    short VFS_Mount_Device_Core_free_mount_struc_found
        add   edi, VFS_Mount_Struc_Size
        dec   ecx
        jnz   short VFS_Mount_Device_Core_find_free_mount_struc
       pop    ecx
        jmp   VFS_Mount_Device_Core_Error_1
VFS_Mount_Device_Core_free_mount_struc_found:
       pop    ecx
        ; vyplnime potrebne udaje...
        ; inoda adresare, na ktery bylo zarizeni namontovano
        mov   dword ptr es:[edi+VFS_MS_VFS_Inode], eax
        ; cislo zarizeni driveru
        mov   dword ptr es:[edi+VFS_MS_Device], ebx
        ; montujeme pro ECX pristup
        mov   dword ptr es:[edi+VFS_MS_Attributes], ecx
        ; kolika jednotlivymi tasky je dane zarizeni pouzivano(otevreno)
        mov   dword ptr es:[edi+VFS_MS_Count_Use], 0
        ; datum a cas primontovani
       push   edi
        add   edi, VFS_MS_Mount_Date_Time
        Call  CMOS_Get_Date_Time
       pop    edi
        ; najdeme ukazatel na informacni polozku FS
       push   es edi
        call  FS_Find_FS
       pushfd
       push   ds
       pop    fs
        mov   ebx, esi
       popfd
       pop    esi ds
        jc    VFS_Mount_Device_Core_Error_3
        ; zvysime pocitadlo pouziti FS
        inc   word ptr fs:[ebx+VFS_FS_Count_of_Use]

        mov   dword ptr ds:[esi+VFS_MS_FS_Infos], ebx
        mov   word ptr ds:[esi+VFS_MS_FS_Infos+4], fs

        ; zavolame fci FS driveru REMOUNT, cimz docilime nacteni datovych struktur FS pro root
        lfs   ebx, fword ptr fs:[ebx+VFS_FS_Super_Ops]
       push   ds esi
        call  fword ptr fs:[ebx+FS_Super_Ops_Read_Super]
       pop    esi ds
        jc    VFS_Mount_Device_Core_Error_4
       push   ds
        mov   ax, data_descriptor
        mov   ds, ax
        inc   dword ptr ds:[Count_VFS_Mount_Struc]        ; zvysi citadlo namontovanych zarizeni
       pop    ds
       pop    eax ;eax
       pop    edx ;esi
       pop    es  ;ds
       pop    edi edx ecx ebx gs es
        clc ;CF=0
        ret
VFS_Mount_Device_Core_End:
       pop    esi ds edi edx ecx ebx gs es
        ret
VFS_Mount_Device_Core_Error_1: ;nedostatek mount struc
       pop    eax
        xor   eax, eax
        stc ;CF=1
        jmp   short VFS_Mount_Device_Core_End
VFS_Mount_Device_Core_Error_2: ;inoda neni volna
       pop    eax
        mov   eax, 1
        stc ;CF=1
        jmp   short VFS_Mount_Device_Core_End
VFS_Mount_Device_Core_Error_3: ;FS nenalezen
       pop    eax
        mov   eax, 2
        stc ;CF=1
        jmp   short VFS_Mount_Device_Core_End
VFS_Mount_Device_Core_Error_4: ;chyba cteni super blocku
       pop    eax
        mov   eax, 3
        stc ;CF=1
        jmp   VFS_Mount_Device_Core_End
VFS_Mount_Device_Core Endp

; namontuje na inodu zarizeni, funguje az po namontovani korene
; Vstup:  DS:ESI - cesta k adresari, na ktery se bude montovat (napr. '/mnt/floppy',0)
;         ES:EDI - nazev souboroveho systemu, max delka je 10 znaku, podporuje se
;                  ukonceni na 0(napr. pri delce nazvu 9 znaku a nize) - (napr. 'EXT2',0)
;         FS:EBX - cesta k zarizeni (napr. '/dev/fd0',0)
;         EAX    - attributy pro montovani (napr. 1 -> RW pristup)
; Vystup: CF = 0 - OK
;         CF = 1 - Error
;            = EAX - Error Kod
;                  = 0 - montovaci cesta nenalezena
;                  = 1 - free
;                  = 2 - zarizeni nenalezeno
;                  = 3 - Vice zarizeni nemuze byt namontovano
;                  = 4 - Na teto inode jiz je nejake zarizeni namontovane
;                  = 5 - Montovani selhalo
;                  = 6 - Zarizeni, ktere chceme montovat neni zarizeni
;                  = 7 - Adresar, na ktery chcete namontovat zarizeni neni adresar
VFS_Mount_Device Proc
       push   ebx ecx edx ebp esi edi es ds fs eax
       push   eax
        mov   eax, 1 ; read only
        call  VFS_Open
       pop    ecx ; attr. pro montovani
        jc    VFS_Mount_Device_error_0

        call  VFS_Is_Inode_Directory
        jc    short VFS_Mount_Device_Error_7

        ; ukazatel na jmeno FS
       push   es
       pop    ds
        mov   esi, edi
        ; cislo typu driveru+cislo zarizeni
       push   eax ds esi
       push   fs
       pop    ds
        mov   esi, ebx
        mov   edx, 1
        Call  VFS_Open
        jc    VFS_Mount_Device_Error_2

       push   ds esi eax
        call  VFS_Check_Inode
        mov   eax, dword ptr ds:[esi+VFS_I_MODE]
        ; EAX - mod souboru
       push   eax
        and   eax, EXT2_S_IFBLK
        cmp   eax, EXT2_S_IFBLK
       pop    eax
        jz    short VFS_Mount_Device_11
VFS_Mount_Device_10:
        and   eax, EXT2_S_IFCHR
        cmp   eax, EXT2_S_IFCHR
        jz    short VFS_Mount_Device_11
       pop    eax esi ds
        jmp   short VFS_Mount_Device_Error_6
VFS_Mount_Device_11:
        mov   ebx, dword ptr ds:[esi+VFS_I_Inode_Blocks]
        ; EBX - typove cislo zarizeni
       pop    eax esi ds
        Call  VFS_Close
       pop    esi ds eax
        call  VFS_Mount_Device_Core
        jc    VFS_Mount_Device_Error_5
VFS_Mount_Device_True:
       pop    eax
        clc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_Error_7:
        Call  VFS_Close
       pop    eax
        mov   eax, 7
        stc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_Error_6:
       pop    esi ds eax
        Call  VFS_Close
       pop    eax
        mov   eax, 6
        stc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_Error_5:
        Call  VFS_Close
       pop    eax
        mov   eax, 5
        stc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_Error_4:
        Call  VFS_Close
       pop    eax
        mov   eax, 4
        stc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_Error_3:
        Call  VFS_Close
       pop    eax
        mov   eax, 3
        stc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_Error_2:
       pop    esi ds eax
        Call  VFS_Close
       pop    eax
        mov   eax, 2
        stc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_Error_0:
       pop    eax
        mov   eax, 0
        stc   ;CF=0
        jmp   short VFS_Mount_Device_end
VFS_Mount_Device_end:
       pop    fs ds es edi esi ebp edx ecx ebx
        ret
VFS_Mount_Device Endp


; Odmontuje zarizeni
; Vstup:  DS:ESI - cesta k adresari, na ktery je neco namontovane
; Vystup: CF = 0 - OK
;         CF = 1 - ERROR
;            = EAX - 0 - byla zadana spatna cesta
;                  - 1 - na tomto adresari neni nic namontovane
VFS_UMount_Device Proc
        ; nezapomen snizit pocitadlo ve VFS_FS_Infos
        ret
VFS_UMount_Device Endp

; prevede cestu na koncovou inodu
; Vstup:  DS:ESI - cesta k souboru,adresari,... zakonceny na 0
; Vystup: CF=0 - OK
;              - EAX = cislo inody
;              - EBX = cislo nadrazene inody
;              - DS:ESI = ukazatel na mount struc polozku
;         CF=1 - Error
VFS_Trace_Path Proc
       push   gs es edi edx ecx eax ebx esi ds
        xor   eax, eax
VFS_Trace_Path_Loop:
       push   ds esi
        call  VFS_Find_Mounted_Inode
       pushfd
        mov   ebx, ds
        mov   edi, esi
       popfd
       pop    esi ds
        jc    VFS_Trace_Path_false
        ; ES:EDI - ukazatel na polozku v mounttab pro zarizeni namontovane
        ; na root
        mov  es, ebx
        lfs  ebx, fword ptr es:[edi+VFS_MS_FS_Infos]
        lfs  ebx, fword ptr fs:[ebx+VFS_FS_Inode_Ops]
        call fword ptr fs:[ebx+FS_Inode_Ops_lookup]
        or   ecx, ecx
        jz   short VFS_Trace_Path_True
        cmp  ecx, 2
        jz   short VFS_Trace_Path_False
        ; cesta pokracuje na jinem zarizeni, mozna ...
        ; zjistime, jestli je pro EAX inodu otevrena VFS inoda
       push  ds esi
       push  es
       pop   ds
        mov  esi, edi
        call VFS_Check_Inode_In_VFS_Inode
       pop   esi ds
        jc   short VFS_Trace_Path_False
        ; EAX - VFS inoda, ktera byla otevrena pro EAX inodu
        jmp  VFS_Trace_Path_Loop

VFS_Trace_Path_True:
       pop   ds esi ecx ecx
       push  es
       pop   ds
        mov  esi, edi
        clc  ;CF=0
        jmp  short VFS_Trace_Path_End
VFS_Trace_Path_False:
       pop   ds esi ebx eax
        stc  ;CF=1
VFS_Trace_Path_End:
       pop   ecx edx edi es gs
        ret
VFS_Trace_Path Endp

; provede dir v adresari
; Vstup:  DS:ESI - cesta k adresari,... zakonceny na 0
; Vystup: CF=0 - OK
;              - ES:EDI - vystupni buffer
;              - Struktura vystupniho bufferu
;              - DD cislo inody ktera je prirazena nazvu polozky
;              - pokud je toto cislo inody ve vyst. buf. nulova
;                pak se jedna o ukonceni, cili tato polozka je posledni
;                a uz neplatna !
;              - retezec s nazvem adresarove polozky ukonceny na 0
;         CF=1 - Error
VFS_Dir Proc
      push   fs ds esi eax ebx es edi
       call  VFS_Trace_Path
       jc    short VFS_Dir_false
       les   edi, fword ptr ds:[esi+VFS_MS_FS_Infos]
       les   edi, fword ptr es:[edi+VFS_FS_File_Ops]
       call  fword ptr es:[edi+FS_File_ops_read_dir]
VFS_Dir_true:
      pop    esi ds
       clc   ;CF=0
       jmp   short VFS_Dir_end
VFS_Dir_false:
      pop    edi es
       stc   ;CF=1
VFS_Dir_end:
      pop    ebx eax esi ds fs
       ret
VFS_Dir Endp

; otevre soubor a vrati rukojet
; Vstup:  EAX    - typ otevreni
;         DS:ESI - cesta k souboru,adresari,... zakonceny na 0
; Vystup: CF     = 0 - vse probehlo OK
;                = EAX - cislo VFS inody
;         CF     = 1 - nekde nastal ERROR
;                = EAX - error kod
;                      = 0 - soubor,adresar,... nebyl nalezen
;                      = 1 - soubor,adresar,... pristup k sourobu je blokovan
;                      = 2 - vic souboru nemuze byt z duvodu nedostatku rukojeti otevreno
;                      = 3 - Nemuzu nacist koncovou inodu
; fce VFS_Open otevre soubor tim, ze pro dany FS vytvori rukojet do LDT tasku, ktery o otevreni zazadal
VFS_Open Proc
        push  ebx ecx edx esi edi ebp ds es fs
        call  kernel_lock
        mov   edx, eax
        ; prevede cestu na koncovou inodu
        call  VFS_Trace_Path
        jc    VFS_Open_Error_0
        ; zjistime, jestli uz inoda neni otevrena pro zapis
;       push   edx
;        mov   edx, 2 ; otevreni pro zapis
;        call  VFS_Check_Open_Mode
;       pop    edx
;        jnc   short VFS_Open_Main_100
;        jmp   VFS_Open_Error_1
;VFS_Open_Main_100:
        jmp   VFS_Open_Main_00

; Vstup: eax - cislo koncove inody
;        ebx - cislo nadrazene inody
;        edx - attributy
;        ds:esi - ukazatel na mount struc polozku
VFS_Open_Main:
        ; musi byt preskocene, jen pokud se vola vfs_open
        ; pokud se vola vfs_open_main tak ne...
        push  ebx ecx edx esi edi ebp ds es fs
VFS_Open_Main_0:
        call  kernel_lock
VFS_Open_Main_00:
       push   eax
        call  VFS_Check_Inode_In_VFS_Inode
       push   eax
       pop    ecx
       pop    eax
        jc    VFS_Open_Main_000
        mov   eax, ecx
       push   eax
        call  VFS_Check_Inode
       pop    eax
        jc    VFS_Open_Main_000
        inc   dword ptr ds:[esi+VFS_I_Use_Count]
        jmp   VFS_Open_End
VFS_Open_Main_000:
        ; najdeme volnou inodu
        call  VFS_Find_Free
        jc    VFS_Open_Error_2
        ; es:edi - ukazatel na danou volnou polozku
        ; attributy oznacujici zpusob prvniho otevreni souboru
        mov   dword ptr es:[edi+VFS_I_Open_Mode], edx
        ; cislo inody v namontovanem FS
        mov   dword ptr es:[edi+VFS_I_Inode], eax
       push   eax
        ; cislo nadrazene inody(adresare, ve ktere se tato inoda nachazi)
        mov   dword ptr es:[edi+VFS_I_Parent_Inode], ebx
        ; ukazatel na "rodicovskou" polozku mount_Struc, pod kterou tato inoda patri
        mov   word ptr  es:[edi+VFS_I_Mount_Struc+4], ds
        mov   dword ptr es:[edi+VFS_I_Mount_Struc], esi
        ; datum a cas otevreni inody
       push   edi
        add   edi, VFS_I_Open_Date_Time
        Call  CMOS_Get_Date_Time
       pop    edi
        ; datum a cas posledni modifikace inody (read,write,poll,..)
       push   edi
        add   edi, VFS_I_Modify_Date_Time
        Call  CMOS_Get_Date_Time
       pop    edi
        ; zatim nejsou data inody namapovane do pameti
        mov   dword ptr es:[edi+VFS_I_Mapped_Sel], 0
        ; kolikrat byla na inodu pouzita fce write
        mov   dword ptr es:[edi+VFS_I_Write_Count], 0
        ; kolik rukojeti je na tuto VFS inodu vytvoreno
        mov   dword ptr es:[edi+VFS_I_Use_Count], 0
        ; velikost bloku na partitione
        ; attributy oznacujici typ uzamceni souboru
        mov   dword ptr es:[edi+VFS_I_File_Lock_Atr], VFS_File_Lock_Off
        ; ukazatel na tabulku funkci pro praci s touto inodou
       push   fs ebx
        mov   fs, word ptr ds:[esi+VFS_MS_Cache_Area]
        mov   eax, dword ptr fs:[Ext2_Block_Size]
        mov   dword ptr es:[edi+VFS_I_Block_Size], eax
       pop    ebx fs
       pop    eax
        ; ES:EDI - ukazatel na VFS inodu
        ; DS:ESI - ukazatel na mount struc polozku, pro danou inodu
        ; EAX - inoda na zarizeni
        ; otevreme inodu
        lfs   ebx, fword ptr ds:[esi+VFS_MS_FS_Infos]
        lfs   ebx, fword ptr fs:[ebx+VFS_FS_File_Ops]
        call  fword ptr fs:[ebx+FS_File_ops_open]
        jnc   short VFS_Open_Main_10
        mov   dword ptr es:[edi+VFS_I_Inode], 0
        jmp   VFS_Open_Error_3
VFS_Open_Main_10:
        inc   dword ptr ds:[esi+VFS_MS_Count_Use]
        mov   cx, Data_Descriptor
        mov   ds, cx
        inc   dword ptr ds:[Count_VFS_inode] ; zvysime pocet otevrenych VFS inod

        ; vypocteme cislo VFS inody
        mov   eax, edi
        xor   edx, edx
        mov   ebx, VFS_Inode_Size
        div   ebx
        clc   ;CF=0
        jmp   VFS_Open_End

VFS_Open_Error_3: ;nemuzu otevrit inodu
        mov  eax, 3
        stc
        jmp  short VFS_Open_End
VFS_Open_Error_2: ;nedostatek rukojeti
        mov  eax, 2
        stc
        jmp  short VFS_Open_End
VFS_Open_Error_1: ;pristup blokovan
        mov  eax, 1
        stc
        jmp  short VFS_Open_End
VFS_Open_Error_0: ;spatna cesta
        xor  eax, eax
        stc
VFS_Open_End:
       pushfd
        call  kernel_unlock
       popfd
        pop   fs es ds ebp edi esi edx ecx ebx
        ret
VFS_Open Endp

; zavre VFS inodu
; Vstup:  EAX - cislo VFS inody
; Vystup: CF = 0 - OK
;         CF = 1 - Error
VFS_Close Proc
      push  ds esi eax
       call VFS_Check_Inode
       jc   short VFS_Close_false
       cmp  dword ptr ds:[esi+VFS_I_Use_Count], 0
       jz   short VFS_Close_false
       dec  dword ptr ds:[esi+VFS_I_Use_Count]
       cmp  dword ptr ds:[esi+VFS_I_Use_Count], 0
       jnz  short VFS_Close_true
       mov  dword ptr ds:[esi+VFS_I_Inode], 0
       lds  esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
       dec  dword ptr ds:[esi+VFS_MS_Count_Use]
       mov  eax, Data_Descriptor
       mov  ds, eax
       dec  dword ptr ds:[Count_VFS_inode] ; zvysime pocet otevrenych VFS inod
VFS_Close_true:
       clc  ;CF=0
       jmp  short VFS_Close_End
VFS_Close_false:
       stc  ;CF=1
VFS_Close_End:
      pop   eax esi ds
       ret
VFS_Close Endp

; zjistime, jestli uz inoda neni otevrena pro zapis
; Vstup:  EAX - inoda na rootu
;         EDX - attributy otevreni, ktere hledame (maska attributupro ANDovani)
;         DS:ESI - ukazatel na mount tab polozku zarizeni, na kterem je inoda
; Vystup: CF - 0 = inoda neni nikde otevrena pro zapis
;         CF - 1 = inoda je jiz otevrena pro zapis
VFS_Check_Open_Mode Proc
      push  es edi ebx ecx edx esi ds ebp eax
       mov  ebx, ds
       mov  edi, esi
       mov  ecx, data_descriptor
       mov  ds, ecx
       mov  ecx, Max_VFS_Inode ; dword ptr ds:[Count_VFS_inode]
       mov  ds, word ptr ds:[Sel_VFS_Inode]
       xor  esi, esi
       xor  ebp, ebp
VFS_Check_Open_Mode_loop:
       cmp  dword ptr ds:[esi+VFS_I_Inode], 0
       jz   short VFS_Check_Open_Mode_next
       ; ted testneme inodu
       cmp  dword ptr ds:[esi+VFS_I_Inode], eax
       jnz  short VFS_Check_Open_Mode_next
       ; ted testneme adresu na mount tab polozku
       cmp  word ptr ds:[esi+VFS_I_Mount_Struc+4], bx
       jnz  short VFS_Check_Open_Mode_next
       cmp  dword ptr ds:[esi+VFS_I_Mount_Struc], edi
       jnz  short VFS_Check_Open_Mode_next
       ; adresa inody souhlasi
      push  eax
       mov  eax, dword ptr ds:[esi+VFS_I_Open_Mode]
       and  eax, edx
       cmp  eax, edx
       jnz  short VFS_Check_Open_Mode_false_open_mode
       inc  ebp
VFS_Check_Open_Mode_false_open_mode:
      pop   eax
VFS_Check_Open_Mode_next:
       add  esi, VFS_Inode_Size
       dec  ecx
       jnz  short VFS_Check_Open_Mode_loop
       ; zjistime, jestli se vyskytl nejaky pripad
       or   ebp, ebp
       jz   short VFS_Check_Open_Mode_True
       jmp  short VFS_Check_Open_Mode_False
VFS_Check_Open_Mode_True:
      pop  ecx
       clc  ;CF=0
       jmp  short VFS_Check_Open_Mode_end
VFS_Check_Open_Mode_False:
      pop   eax
       stc  ;CF=1
VFS_Check_Open_Mode_end:
      pop ebp ds esi edx ecx ebx edi es
       ret
VFS_Check_Open_Mode endp

; zjisti, jestli VFS inoda existuje
; Vstup : EAX - VFS inoda
; Vystup: CF = 0 - OK
;            = DS:ESI - ukazatel na polozku VFS inode
;         CF = 1 - Error
VFS_Check_Inode Proc
      push  es eax ebx ecx edx esi ds
       mov  bx, data_descriptor
       mov  ds, bx
       cmp  dword ptr ds:[Count_VFS_inode], 0
       jz   short VFS_Check_Inode_False
       cmp  eax, Max_VFS_Inode
       jge  short VFS_Check_Inode_False
       mov  ds, word ptr ds:[Sel_VFS_Inode]
       mov  ecx, VFS_Inode_Size
       mul  ecx
       mov  esi, eax
       cmp  dword ptr ds:[esi+VFS_I_Inode], 0
       jz   short VFS_Check_Inode_False
      pop   es ecx ; ds esi
       clc  ;CF=0
       jmp  short VFS_Check_Inode_End
VFS_Check_Inode_False:
      pop   ds esi
       stc  ;CF=1
VFS_Check_Inode_End:
      pop   edx ecx ebx eax es
       ret
VFS_Check_Inode endp

; zjisti, jestli uz je otevrena VFS inoda pro inodu na danem zarizeni
; Vstup : EAX - Inoda na zarizeni
;         DS:ESI - ukazatel na polozku mounttab
; Vystup: CF = 0 - OK
;            = EAX - cislo VFS inody
;         CF = 1 - Error
VFS_Check_Inode_In_VFS_Inode Proc
      push  es edi ebx ecx edx esi ds ebp eax
       mov  ebx, ds
       mov  edi, esi
       mov  ecx, data_descriptor
       mov  ds, ecx
       cmp  dword ptr ds:[Count_VFS_inode], 0
       jz   short VFS_Check_Inode_In_VFS_Inode_False
       mov  ecx, Max_VFS_Inode
       mov  ds, word ptr ds:[Sel_VFS_Inode]
       xor  esi, esi
       xor  ebp, ebp
VFS_Check_Inode_In_VFS_Inode_loop:
       cmp  dword ptr ds:[esi+VFS_I_Inode], 0
       jz   VFS_Check_Inode_In_VFS_Inode_next
       ; ted testneme inodu
       cmp  dword ptr ds:[esi+VFS_I_Inode], eax
       jnz  VFS_Check_Inode_In_VFS_Inode_next
       ; ted testneme adresu na mount tab polozku
       cmp  word ptr ds:[esi+VFS_I_Mount_Struc+4], bx
       jnz  VFS_Check_Inode_In_VFS_Inode_next
       cmp  dword ptr ds:[esi+VFS_I_Mount_Struc], edi
       jz   short VFS_Check_Inode_In_VFS_Inode_True
VFS_Check_Inode_In_VFS_Inode_next:
       inc  ebp
       add  esi, VFS_Inode_Size
       dec  ecx
       jnz  VFS_Check_Inode_In_VFS_Inode_loop
       jmp  short VFS_Check_Inode_In_VFS_Inode_False
VFS_Check_Inode_In_VFS_Inode_True:
      pop  ecx
       mov  eax, ebp
       clc  ;CF=0
       jmp  short VFS_Check_Inode_In_VFS_Inode_end
VFS_Check_Inode_In_VFS_Inode_False:
      pop   eax
       stc  ;CF=1
VFS_Check_Inode_In_VFS_Inode_end:
      pop ebp ds esi edx ecx ebx edi es
       ret
VFS_Check_Inode_In_VFS_Inode endp

; najdi volnou VFS inodu
; Vstup : -
; Vystup: CF = 0 - OK
;            = ES:EDI - ukazatel na volnou polozku VFS inode
;         CF = 1 - Error
VFS_Find_Free Proc
      push  eax ebx ecx edx edi es
       mov  bx, data_descriptor
       mov  es, bx
       mov  es, word ptr es:[Sel_VFS_Inode]
       xor  edi, edi
       mov  ecx, Max_VFS_Inode
VFS_Find_Free_find:
       cmp  dword ptr es:[edi+VFS_I_Inode], 0
       jz   short VFS_Find_Free_true
       add  edi, VFS_Inode_Size
       dec  ecx
       jnz  short VFS_Find_Free_find
       ; pokusime se najit VFS inodu, ktera sice je otevrena,
       ; ale nikdo ji nepouziva
       xor  edi, edi
       mov  ecx, Max_VFS_Inode
VFS_Find_Free_find_2:
       cmp  dword ptr es:[edi+VFS_I_Use_Count], 0
       jnz  short VFS_Find_Free_find_3
       movzx eax, word ptr es:[edi+VFS_I_Mapped_Sel]
       call DeAlloc
       mov  word ptr es:[edi+VFS_I_Mapped_Sel], 0
       jmp  VFS_Find_Free_true
VFS_Find_Free_find_3:
       add  edi, VFS_Inode_Size
       dec  ecx
       jnz  short VFS_Find_Free_find_2
VFS_Find_Free_false:
      pop   es edi edx ecx ebx eax
       stc  ;CF=1
       ret
VFS_Find_Free_true:
       clc  ;CF=0
      pop   ecx ecx edx ecx ebx eax
       ret
VFS_Find_Free Endp

; nacte z VFS inody 4Kb dat
; Vstup:  EAX - cislo VFS inody
;         EBX - offset v souboru ve 4Kb nasobcich
;         ES:EDI - buffer pro ulozeni 4Kb pameti
; Vystup: CF = 0 - chyba
;            = EAX - error kod
;         CF = 1 - OK
VFS_Inode_Read Proc
      pushad
      push   ds es fs
       call  VFS_Check_Inode
       jc    short VFS_Inode_Read_False
       ; DS:ESI - ukazatel na VFS inodu
       lfs  edx, fword ptr ds:[esi+VFS_I_Mount_Struc]
       lfs  edx, fword ptr fs:[edx+VFS_MS_FS_Infos]
       lfs  edx, fword ptr fs:[edx+VFS_FS_File_Ops]
       ; zavolame fci pro cteni ze souboru
       ; EAX    - kolikaty blok souboru se ma nacist
       ; DS:ESI - ukazatel na polozku VFS_Inode
       ; ES:EDI - buffer, do ktereho se maji ulozit nactene data
       mov  eax, ebx
       call fword ptr fs:[edx+FS_File_ops_read]

       jmp  short VFS_Inode_Read_End
VFS_Inode_Read_False:
       stc ;CF=1
VFS_Inode_Read_End:
      pop    fs es ds
      popad
       ret
VFS_Inode_Read Endp

; Vstup:  EAX - VFS inoda
;         EBX - pocatecni 4k blok souboru (od 0)
;         ECX - posledni 4k blok souboru
;             - pokud EBX=0FFFFFFFFh pak se namapuje az do konce souboru
; Vystup: CF - 0 = OK
;            - EDX - selector na vystupni descriptor
;            - EAX - velikost souboru v bajtech
;         CF - 1 = Error
VFS_Map_File Proc
      push   fs ebx eax edi esi ebp ds ecx edx es
       call  VFS_Check_Inode
       jc    VFS_Map_File_False
       ; ds:esi - ukazatel na polozku VFS inody otevreneho souboru
       cmp   word ptr ds:[esi+VFS_I_Mapped_Sel], 0
       jnz   VFS_Map_File_found
VFS_Map_File_new:
      push   eax
       mov   eax, ebx
       mov   ebx, ecx
       lfs   edx, fword ptr ds:[esi+VFS_I_Mount_Struc]
       lfs   edx, fword ptr fs:[edx+VFS_MS_FS_Infos]
       lfs   edx, fword ptr fs:[edx+VFS_FS_File_Ops]
       call  fword ptr fs:[edx+FS_File_Get_Map]
      pop    ebx
       jc    VFS_Map_File_False
       or    eax, eax
       jz    VFS_Map_File_False
      push   ebx
       mov   edx, eax
       xor   eax, eax
       call  memory_manager_allocate_memory
       ;jc    short VFS_Map_File_False
      pop    ebx
       call  FMPI_Map_File_To_Mem
       mov   eax, es
      pop    es
       call  dealloc
      pop    ecx ecx ;edx ecx
       mov   word ptr ds:[esi+VFS_I_Mapped_Sel], dx
       mov   ecx, dword ptr ds:[esi+VFS_I_File_Size]
       clc   ;CF=0
       jmp   VFS_Map_File_End
VFS_Map_File_found:
       movzx edx, word ptr ds:[esi+VFS_I_Mapped_Sel]
       lsl   edx, edx
       or    edx, edx
       jz    VFS_Map_File_new
      pop    es edx ecx
       inc   dword ptr ds:[esi+VFS_I_Use_Count]
       movzx edx, word ptr ds:[esi+VFS_I_Mapped_Sel]
       movzx ecx, word ptr ds:[esi+VFS_I_File_Size]
       clc   ;CF=0
       jmp   short VFS_Map_File_End
VFS_Map_File_False:
      pop    es edx ecx
       stc   ;CF=1
VFS_Map_File_End:
      pop    ds ebp esi edi eax ebx fs
       ret
VFS_Map_File endp

; Uzamkne zarizeni
; Vstup:  DS:ESI - ukazatel na jakoukoliv VFS Inodu na danem zarizeni
; Vystup: -
;
; Pokud uz je zarizeni uzamkle, tato procedura automaticky rekne taskmanovi, aby prepl na dalsi ulohu, s tim,
; ze po prepnuti zpet na tento task se vrati sem do procedury, ktera opet zkontroluje, jestli je zarizeni uzamkle, ....
VFS_Lock_Device Proc
       pushfd
       push  es edi eax
        mov  es, word ptr ds:[VFS_I_Mount_Struc+FP_Selector]
        mov  edi, dword ptr ds:[VFS_I_Mount_Struc+FP_Offset]

        ; attributy namontovani zarizeni
VFS_Lock_Device_Test_Lock:
        mov  eax, dword ptr es:[edi+VFS_MS_Attributes]
        and  eax, 0100b
        cmp  eax, 0100b
        jnz  short VFS_Lock_Device_Lock

        ; rekni taskmanovi at prepne task

        jmp  short VFS_Lock_Device_Test_Lock
VFS_Lock_Device_Lock:
        or   dword ptr es:[edi+VFS_MS_Attributes], not(0100b)
       pop   eax edi es
       popfd
        ret
VFS_Lock_Device Endp

; Odemkne zarizeni
; Vstup:  ES:EDI - ukazatel na polozku mount struc namontovaneho zarizeni
; Vystup: -
;
; Pokud uz je zarizeni uzamkle, tato procedura automaticky rekne taskmanovi, aby prepl na dalsi ulohu, s tim,
; ze po prepnuti zpet na tento task se vrati sem do procedury, ktera opet zkontroluje, jestli je zarizeni uzamkle, ....
VFS_UnLock_Device Proc
       pushfd
       push  es edi eax
        ; attributy namontovani zarizeni
VFS_UnLock_Device_Test_Lock:
        mov  eax, dword ptr es:[edi+VFS_MS_Attributes]
        and  eax, 0100b
        cmp  eax, 0100b
        jz  short VFS_UnLock_Device_UnLock

        ; rekni taskmanovi at prepne task

        jmp  short VFS_UnLock_Device_Test_Lock
VFS_UnLock_Device_UnLock:
        and   dword ptr es:[edi+VFS_MS_Attributes], not(0100b)
       pop   eax edi es
       popfd
        ret
VFS_UnLock_Device Endp


; Zakaze remount zarizeni
; Vstup:  ES:EDI - ukazatel na polozku mount struc zarizeni
; Vystup: -
;
; Pokud uz je zarizeni uzamkle, tato procedura automaticky rekne taskmanovi, aby prepl na dalsi ulohu, s tim,
; ze po prepnuti zpet na tento task se vrati sem do procedury, ktera opet zkontroluje, jestli je zarizeni uzamkle, ....
VFS_Remount_Device_Off Proc
       pushfd
       push  es edi eax
        ; attributy namontovani zarizeni
VFS_Remount_Device_Off_Test:
        mov  eax, dword ptr es:[edi+VFS_MS_Attributes]
        and  eax, 01000b
        cmp  eax, 01000b
        jnz  short VFS_Remount_Device_Off_Lock

        ; rekni taskmanovi at prepne task

        jmp  short VFS_Remount_Device_Off_Test
VFS_Remount_Device_Off_Lock:
        or   dword ptr es:[edi+VFS_MS_Attributes], not(01000b)
       pop   eax edi es
       popfd
        ret
VFS_Remount_Device_Off Endp

; Povoli remount zarizeni
; Vstup:  ES:EDI - ukazatel na polozku mount struc zarizeni
; Vystup: -
;
; Pokud uz je zarizeni uzamkle, tato procedura automaticky rekne taskmanovi, aby prepl na dalsi ulohu, s tim,
; ze po prepnuti zpet na tento task se vrati sem do procedury, ktera opet zkontroluje, jestli je zarizeni uzamkle, ....
VFS_Remount_Device_On Proc
       pushfd
       push  es edi eax
        ; attributy namontovani zarizeni
VFS_Remount_Device_On_Test:
        mov  eax, dword ptr es:[edi+VFS_MS_Attributes]
        and  eax, 01000b
        cmp  eax, 01000b
        jz  short VFS_Remount_Device_On_UnLock

        ; rekni taskmanovi at prepne task

        jmp  short VFS_Remount_Device_On_Test
VFS_Remount_Device_On_UnLock:
        and   dword ptr es:[edi+VFS_MS_Attributes], not(01000b)
       pop   eax edi es
       popfd
        ret
VFS_Remount_Device_On Endp

; zjisti, jestli je VFS inoda otevrena pro soubor
; Vstup:  EAX - VFS inoda
; Vystup: CF = 0 - True
;         CF = 1 - False
VFS_Is_Inode_File Proc
       push   ds esi eax
        call  VFS_Check_Inode
        jc    short VFS_Is_Inode_File_False
        mov   eax, dword ptr ds:[esi+VFS_I_MODE]
        ; EAX - mod souboru
        and   eax, EXT2_S_IFFIL
        cmp   eax, EXT2_S_IFFIL
        jnz   short VFS_Is_Inode_File_False
VFS_Is_Inode_File_True:
        clc   ;CF=0
        jmp   short VFS_Is_Inode_File_End
VFS_Is_Inode_File_False:
        stc   ;CF=1
VFS_Is_Inode_File_End:
       pop    eax esi ds
        ret
VFS_Is_Inode_File Endp

; zjisti, jestli je VFS inoda otevrena pro adresar
; Vstup:  EAX - VFS inoda
; Vystup: CF = 0 - True
;         CF = 1 - False
VFS_Is_Inode_Directory Proc
       push   ds esi eax
        call  VFS_Check_Inode
        jc    short VFS_Is_Inode_Directory_False
        mov   eax, dword ptr ds:[esi+VFS_I_MODE]
        ; EAX - mod souboru
        and   eax, EXT2_S_IFDIR
        cmp   eax, EXT2_S_IFDIR
        jnz   short VFS_Is_Inode_Directory_False
VFS_Is_Inode_Directory_True:
        clc   ;CF=0
        jmp   short VFS_Is_Inode_Directory_End
VFS_Is_Inode_Directory_False:
        stc   ;CF=1
VFS_Is_Inode_Directory_End:
       pop    eax esi ds
        ret
VFS_Is_Inode_Directory Endp

; zjisti, jestli je VFS inoda otevrena pro adresar
; Vstup:  EAX - VFS inoda
; Vystup: CF = 0 - True
;         CF = 1 - False
VFS_Is_Inode_BLK_Device Proc
       push   ds esi eax
        call  VFS_Check_Inode
        jc    short VFS_Is_Inode_BLK_Device_False
        mov   eax, dword ptr ds:[esi+VFS_I_MODE]
        ; EAX - mod souboru
        and   eax, EXT2_S_IFBLK
        cmp   eax, EXT2_S_IFBLK
        jnz   short VFS_Is_Inode_BLK_Device_False
VFS_Is_Inode_BLK_Device_True:
        clc   ;CF=0
        jmp   short VFS_Is_Inode_BLK_Device_End
VFS_Is_Inode_BLK_Device_False:
        stc   ;CF=1
VFS_Is_Inode_BLK_Device_End:
       pop    eax esi ds
        ret
VFS_Is_Inode_BLK_Device Endp

; Zapise data do VFS inody
; Vstup:  EAX    - VFS inoda
;         EBX    - pozice v zarizeni, odkud se ma zacit zapisovat
;         ECX    - kolik bajtu dat se ma zapsat
;         ES:EDI - vstupni buffer
; Vystup: CF = 0 - OK
;                - ECX = kolik znaku bylo zapsano
;         CF = 1 - Error
;                - EAX = error kod
;                      - 0 - VFS inoda nenalezena
;                      - 1 - Neznamy typ inody
;                      - 2 - driver pro dane zarizeni nebyl nalezen
;                      - 3 - nemuzu precist vstupni data, protoze bych
;                            cetl za pameti prirazene descriptoru
;                            => General Protection Fault
;                      - 4 - Driver vyvolal chybu
VFS_Write Proc
      push   ebx edx edi esi ds es fs gs eax ecx
       call  VFS_Check_Inode
       jc    VFS_Write_false_0
       ; kontrola na GP
      push   eax
       mov   eax, es
       lsl   eax, eax
       sub   eax, edi
       cmp   eax, ecx
      pop    eax
       jl    VFS_Write_false_3
       ; testneme jestli se jedna o znakove zarizeni
       mov   eax, dword ptr ds:[esi+VFS_I_MODE]
       and   eax, EXT2_S_IFCHR
       cmp   eax, EXT2_S_IFCHR
       jnz   VFS_Write_test_blk_device
       ; testneme jestli je driver pro toto zarizeni nacteny
       mov   eax, dword ptr ds:[esi+VFS_I_Inode_Blocks]
      push   es edi
       call  DRV_Find
      pop    edi es
       jc    VFS_Write_false_2
       mov   eax, dword ptr ds:[esi+VFS_I_Inode_Blocks]
       ; pozdeji tu budeme muset pridelat neco jako ze by mel ten
       ; driver nacit, pokud uz nacteny neni
       mov   ebx, Drv_Std_WRITE
VFS_Write_chr_loop:
       mov   dl, byte ptr es:[edi]
      push   ecx ebx es edi eax
       call  DRV_Gate
      pop    eax edi es ebx ecx
       jc    VFS_Write_False_4
       inc   edi
       dec   ecx
       jnz   short VFS_Write_chr_loop
       jmp   short VFS_Write_true

VFS_Write_test_blk_device:
       ; testneme jestli se jedna o blokove zarizeni
       mov   eax, dword ptr ds:[esi+VFS_I_MODE]
       and   eax, EXT2_S_IFBLK
       cmp   eax, EXT2_S_IFBLK
       jnz   short VFS_Write_test_file
       ; testneme jestli je driver pro toto zarizeni nacteny
       mov   eax, dword ptr ds:[esi+VFS_I_Inode_Blocks]
      push   es edi
       call  DRV_Find
      pop    edi es
       jc    short VFS_Write_false_2
       ; pozdeji tu budeme muset pridelat neco jako ze by mel ten
       ; driver nacit, pokud uz nacteny neni

VFS_Write_test_file:
       ; testneme jestli se jedna o soubor
       mov   eax, dword ptr ds:[esi+VFS_I_MODE]
       and   eax, EXT2_S_IFFIL
       cmp   eax, EXT2_S_IFFIL
       jnz   short VFS_Write_false_1
       ; nemusime kontrolovat jestli ten soubor existuje, protoze to za nas
       ; udelala uz VFS_Open

VFS_Write_true:
      pop    eax eax ; ecx eax
       clc   ;CF=0
       jmp   short VFS_Write_end
VFS_Write_false_4:
      pop    ecx ebx ; ecx eax
       mov   eax, 4
       stc   ;CF=1
       jmp   short VFS_Write_end
VFS_Write_false_3:
      pop    ecx ebx ; ecx eax
       mov   eax, 3
       stc   ;CF=1
       jmp   short VFS_Write_end
VFS_Write_false_2: ; driver nenalezen
      pop    ecx ebx ; ecx eax
       mov   eax, 2
       stc   ;CF=1
       jmp   short VFS_Write_end
VFS_Write_false_1: ; neznamy typ inody
      pop    ecx ebx ; ecx eax
       mov   eax, 1
       stc   ;CF=1
       jmp   short VFS_Write_end
VFS_Write_false_0:
      pop    ecx ebx ; ecx eax
       mov   eax, 0
       stc   ;CF=1
VFS_Write_end:
      pop    gs fs es ds esi edi edx ebx
       ret
VFS_Write Endp

; Cte data z VFS inody
; Vstup:  EAX    - VFS inoda
;         ES:EDI - vystupni buffer
;         EBX,ECX - jsou nepovinne vstupni parametry, zalezi na driveru
;                   jestli je potrebuje nebo ne
;         EBX    - pozice v zarizeni, odkud se ma zacit cist
;         ECX    - kolik bajtu dat se ma precist
; Vystup: CF = 0 - OK
;                - ECX = kolik znaku bylo nacteno
;         CF = 1 - Error
;                - EAX = error kod
;                      - 0 - VFS inoda nenalezena
;                      - 1 - Neznamy typ inody
;                      - 2 - driver pro dane zarizeni nebyl nalezen
;                      - 3 - nemuzu zapsat vystupni data, protoze by
;                            vzniklo preteceni pameti prirazene descriptoru
;                            => General Protection Fault
;                      - 4 - Driver vyvolal chybu
VFS_Read Proc
      push   ebx edx edi esi ds es fs gs eax ecx
       call  VFS_Check_Inode
       jc    VFS_Read_false_0
       ; testneme jestli se jedna o znakove zarizeni
       mov   eax, dword ptr ds:[esi+VFS_I_MODE]
       and   eax, EXT2_S_IFCHR
       cmp   eax, EXT2_S_IFCHR
       jnz   VFS_Read_test_blk_device
       ; testneme jestli je driver pro toto zarizeni nacteny
       mov   eax, dword ptr ds:[esi+VFS_I_Inode_Blocks]
      push   es edi
       call  DRV_Find
      pop    edi es
       jc    VFS_Read_false_2
       mov   eax, dword ptr ds:[esi+VFS_I_Inode_Blocks]
       ; pozdeji tu budeme muset pridelat neco jako ze by mel ten
       ; driver nacit, pokud uz nacteny neni
       ; kontrola na GP
      push   eax
       mov   eax, es
       lsl   eax, eax
       sub   eax, edi
      pop    eax
       jl    VFS_Read_false_3
       mov   ebx, Drv_Std_READ
      push   es edi
       call  DRV_Gate
      pop    edi es
       jc    VFS_Read_false_4
       mov   byte ptr es:[edi], dl
       mov   ecx,1
       jmp   short VFS_Read_true

VFS_Read_test_blk_device:
       ; testneme jestli se jedna o blokove zarizeni
       mov   eax, dword ptr ds:[esi+VFS_I_MODE]
       and   eax, EXT2_S_IFBLK
       cmp   eax, EXT2_S_IFBLK
       jnz   short VFS_Read_test_file
       ; testneme jestli je driver pro toto zarizeni nacteny
       mov   eax, dword ptr ds:[esi+VFS_I_Inode_Blocks]
      push   es edi
       call  DRV_Find
      pop    edi es
       jc    short VFS_Read_false_2
       ; pozdeji tu budeme muset pridelat neco jako ze by mel ten
       ; driver nacit, pokud uz nacteny neni

VFS_Read_test_file:
       ; testneme jestli se jedna o soubor
       mov   eax, dword ptr ds:[esi+VFS_I_MODE]
       and   eax, EXT2_S_IFFIL
       cmp   eax, EXT2_S_IFFIL
       jnz   short VFS_Read_false_1
       ; nemusime kontrolovat jestli ten soubor existuje, protoze to za nas
       ; udelala uz VFS_Open

VFS_Read_true:
      pop    eax eax ; ecx eax
       clc   ;CF=0
       jmp   short VFS_Read_end
VFS_Read_false_4:
      pop    ecx ebx ; ecx eax
       mov   eax, 4
       stc   ;CF=1
       jmp   short VFS_Read_end
VFS_Read_false_3:
      pop    ecx ebx ; ecx eax
       mov   eax, 3
       stc   ;CF=1
       jmp   short VFS_Read_end
VFS_Read_false_2: ; driver nenalezen
      pop    ecx ebx ; ecx eax
       mov   eax, 2
       stc   ;CF=1
       jmp   short VFS_Read_end
VFS_Read_false_1: ; neznamy typ inody
      pop    ecx ebx ; ecx eax
       mov   eax, 1
       stc   ;CF=1
       jmp   short VFS_Read_end
VFS_Read_false_0:
      pop    ecx ebx ; ecx eax
       mov   eax, 0
       stc   ;CF=1
VFS_Read_end:
      pop    gs fs es ds esi edi edx ebx
       ret
VFS_Read Endp

; refreshne data namapovaneho souboru na disk
; Vstup:  EAX - VFS inoda
; Vystup: CF = 0 - OK
;         CF = 1 - Error
VFS_Sync Proc
      push   ds esi eax edx
       call  VFS_Check_Inode
       jc    VFS_Sync_false
       movzx edx, word ptr ds:[esi+VFS_I_Mapped_Sel]
       or    edx, edx
       jz    VFS_Sync_false
       ; zavola se swapD aby ulozil dany descriptor na disk
       call  FMPI_Save_File_To_HD
       jc    VFS_Sync_false
VFS_Sync_true:
       clc  ;CF=0
       jmp   short VFS_Sync_end
VFS_Sync_false:
       stc  ;CF=0
VFS_Sync_end:
      pop    edx eax esi ds
       ret
VFS_Sync Endp

; Vstup:  DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni, na kterem chceme vytvori inodu
;         EAX    - typove cislo
;         EBX    - cislo inody adresare, do ktereho vytvarime soubor
;         ECX    - mode nove vytvarene inody (EXT2_S_IFBLK,EXT2_S_IFCHR,EXT2_S_IFIFO,EXT2_S_IFSOCK)
;         ES:EDI - ukazatel na nazev nove vytvareneho souboru, zakonceny na 0
; Vystup: CF = 0 - OK
;                - EAX = cislo nove inody
;         CF = 1 - Error
VFS_MkNod proc
      push  fs edx
       lfs  edx, fword ptr ds:[esi+VFS_MS_FS_Infos]
       lfs  edx, fword ptr fs:[edx+VFS_FS_Inode_Ops]
       call fword ptr fs:[edx+FS_Inode_Ops_mknod]
      pop   edx fs
       ret
VFS_MkNod endp

; zjisti na jakem zarizeni je namontovana VFS inoda
; Vstup:  EAX - VFS inoda
; Vystup: CF = 0 - OK
;            - DS:ESI = ukazatel na mount struc polozku
;         CF = 1 - Error
VFS_Get_Inode_Mount_device Proc
      push   es edi ds esi
       call  VFS_Check_Inode
       jc    short VFS_Get_Inode_Mount_device_false
       lds   esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
VFS_Get_Inode_Mount_device_true:
      pop    edi es
       clc   ;CF=0
       jmp   short VFS_Get_Inode_Mount_device_end
VFS_Get_Inode_Mount_device_false:
      pop    esi ds
       stc   ;CF=1
VFS_Get_Inode_Mount_device_end:
      pop    edi es
       ret
VFS_Get_Inode_Mount_device endp

; navrati mode  VFS inody
; Vstup:  EAX - cislo VFS inody
; Vystup: CF = 0 - OK
;            - EAX = mode inody, pro kterou byla otevrena VFS inoda
;         CF = 1 - Error
VFS_Get_Inode_mode Proc
      push   ds esi ebx eax
       call  VFS_Check_Inode
       jc    short VFS_Get_Inode_Mount_device_false
       mov   eax, dword ptr ds:[esi+VFS_I_MODE]
VFS_Get_Inode_mode_true:
      pop    ebx
       clc   ;CF=1
       jmp   short VFS_Get_Inode_mode_end
VFS_Get_Inode_mode_false:
      pop    eax
       stc   ;CF=1
VFS_Get_Inode_mode_end:
      pop    ebx esi ds
       ret
VFS_Get_Inode_mode Endp

include .\..\inc\code\code32\fs\vfs_file.inc