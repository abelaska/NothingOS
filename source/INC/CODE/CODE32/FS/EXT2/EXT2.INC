;########################################################################
;# OPERACE EXT2 FS
;########################################################################

; namontuje zarizeni s timto FS na danou VFS inodu
; Vstup:  DS:ESI - ukazatele na mount_tab polozku, ktera je rezervovana pro toto montovani
; Vystup: Zadny registr zmenen
;        CF = 0 - OK
;        CF = 1 - Error
EXT2_Super_Read_Super   Proc ;  nacte superblock
      pushad
      push ds es fs
       cmp   word ptr ds:[esi+VFS_MS_Cache_Area], 0
       jz    short EXT2_Super_Read_Super_0
       ; odalokujeme pamet se staryma strukturama FS
       mov   ax, word ptr ds:[esi+VFS_MS_Cache_Area]
       call  DeAlloc
EXT2_Super_Read_Super_0:
       ; alokujeme pomocnou pamet pro nacteni super bloku
       mov   eax, 1024
       call  Alloc
       jc    EXT2_Super_Read_Super_false
       mov   es, ax

       ; nacteme superblock
       xor   edi, edi
       mov   ecx, 2
       mov   edx, ecx
       call  Read_Disk_Sectors
       jc    EXT2_Super_Read_Super_false_dealloc_ES

       ; zkontrolujeme, jestli se jedna o EXT2
       cmp   word ptr es:[s_magic], 0EF53h
       jc    EXT2_Super_Read_Super_false_dealloc_ES

       ; alokujeme pamet pro promenne ext2 FS
       mov   eax, EXT2_Cache_Variables_Size
       call  Alloc
       jc    EXT2_Super_Read_Super_false_dealloc_ES
       mov   word ptr ds:[esi+VFS_MS_Cache_Area], ax
       mov   fs, ax
       mov   word ptr FS:[Ext2_Super_Block_Sel], es

       ; prepocteme novou hodnotu velikosti bloku
       mov   ecx, dword ptr ES:[s_log_block_size]
       mov   eax, 1024
       shl   eax, cl
       mov   dword ptr FS:[Ext2_Block_Size], eax
       ; nacteme cislo 1. datoveho bloku na zarizeni (v FS)
       mov   ecx, dword ptr ES:[s_first_data_block]
       mov   dword ptr FS:[Ext2_first_data_block], ecx
       ; vypocteme pocet group v FS
       mov   eax, dword ptr ES:[s_blocks_count]
       sub   eax, ecx
       add   eax, dword ptr ES:[s_blocks_per_group]
       dec   eax
       xor   edx, edx
       div   dword ptr ES:[s_blocks_per_group]
       mov   dword ptr FS:[Ext2_Count_Groups], eax
       ; vypocteme velikost group struktur
       shl   eax, 5
       ; alokujeme pamet pro
      push   eax
       call  Alloc
      pop    ecx
       jc    short EXT2_Super_Read_Super_false_dealloc_ES_FS
       mov   word ptr FS:[Ext2_Groups_Sel], ax
       mov   es, ax
       mov   eax, ecx
       xor   edx, edx
       div   dword ptr FS:[Ext2_Block_Size]
       or    edx, edx
       jz    short EXT2_Super_Read_Super_1
       inc   eax
EXT2_Super_Read_Super_1:
       mov   ecx, eax
       ; nacteme groupy
       xor   edi, edi
       mov   eax, 1
EXT2_Super_Read_Super_load_group:
      push   ecx eax
       call  Read_disk_block
      pop    eax ecx
       jc    short EXT2_Super_Read_Super_false_total
       inc   eax
       dec   ecx
       jnz   short EXT2_Super_Read_Super_load_group
EXT2_Super_Read_Super_true:
      pop    fs es
       clc   ;CF=0
       jmp   EXT2_Super_Read_Super_end
EXT2_Super_Read_Super_false_total:
       movzx edx, word ptr FS:[Ext2_Super_Block_Sel]
       mov   eax, fs
      pop    fs
       call  DeAlloc
       mov   eax, es
      pop    es
       call  DeAlloc
       mov   eax, edx
       call  DeAlloc
       stc   ;CF=1
       jmp   short EXT2_Super_Read_Super_end
EXT2_Super_Read_Super_false_dealloc_ES:
       mov   eax, es
      pop    fs es
       call  DeAlloc
       stc   ;CF=1
       jmp   short EXT2_Super_Read_Super_end
EXT2_Super_Read_Super_false_dealloc_ES_FS:
       mov   eax, fs
      pop    fs
       call  DeAlloc
       mov   eax, es
      pop    es
       call  DeAlloc
       stc   ;CF=1
       jmp   short EXT2_Super_Read_Super_end
EXT2_Super_Read_Super_false:
      pop    fs es
       stc   ;CF=1
EXT2_Super_Read_Super_end:
      pop    ds
      popad
       retf
EXT2_Super_Read_Super   Endp

; namontuje zarizeni s timto FS na danou VFS inodu
; Vstup:  DS:ESI - ukazatele na mount_tab polozku, ktera je rezervovana pro toto montovani
; Vystup: Zadny registr zmenen
;        CF = 0 - OK
;        CF = 1 - Error
EXT2_Super_Write_Super  Proc ;  zapise superblock na disk
      pushad
      push   ds es fs
       cmp   word ptr ds:[esi+VFS_MS_Cache_Area], 0
       jz    short EXT2_Super_Write_Super_false
       mov   fs, word ptr ds:[esi+VFS_MS_Cache_Area]
       ; zapise superblock
       cmp   word ptr fs:[Ext2_Super_Block_Sel], 0
       jz    short EXT2_Super_Write_Super_false
       mov   es, word ptr fs:[Ext2_Super_Block_Sel]
       xor   edi, edi
       mov   ecx, 2
       mov   edx, 2
       call  Write_Disk_Sectors
       jc    short EXT2_Super_Write_Super_false
       ; zapiseme groupy
       cmp   word ptr fs:[Ext2_Groups_Sel], 0
       jz    short EXT2_Super_Write_Super_false
       mov   es, word ptr fs:[Ext2_Groups_Sel]
       xor   edi, edi
       mov   eax, 1
       call  Write_disk_block
       jc    short EXT2_Super_Write_Super_false
EXT2_Super_Write_Super_true:
       clc   ;CF=0
       jmp   EXT2_Super_Write_Super_end
EXT2_Super_Write_Super_false:
       stc   ;CF=1
EXT2_Super_Write_Super_end:
      pop    fs es ds
      popad
       retf
EXT2_Super_Write_Super  endp

EXT2_Super_FSstat       Proc ;  vrati informace o FS
        retf
EXT2_Super_FSstat       endp

EXT2_Super_ReMount      Proc
        retf
EXT2_Super_ReMount      endp

EXT2_Super_Lock         Proc ;  uzamce zarizeni
        retf
EXT2_Super_Lock         endp

EXT2_Super_UnLock       Proc ;  odemkne zarizeni
        retf
EXT2_Super_UnLock       endp

; Vstup:  DS:ESI - ukazatel na VFS_Inode
;         ES:EDI - vystupni buffer
;         FS     - pamet pro nacteni indirect bloku
;         EAX    - cislo prvni nacitane adresovaci polozky
;         ECX    - pocet adresovacih polozek, ktere se maji nacist
; Vystup: EDI - ukazatel na konec dat ve vystupnim bufferu
Inode_Read_One_Indirect Proc
       push  ds es esi eax edi
       push  es edi eax ecx
       push  fs
       pop   es
        xor  edi, edi
        mov  eax, dword ptr ds:[esi+VFS_I_Inode_Blocks+12*4]
        lds  esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
        call read_disk_block
       pop   ecx esi edi es
        jc   short Inode_Read_One_Indirect_false
       push  fs
       pop   ds
        shl  esi, 2
        cld
        rep  movsd
       pop   eax ; edi
        clc ;CF=0
        jmp  short Inode_Read_One_Indirect_End
Inode_Read_One_Indirect_false:
       pop   edi
        stc ;CF=1
Inode_Read_One_Indirect_End:
       pop   eax esi es ds
        ret
Inode_Read_One_Indirect Endp

; Vstup:  DS:ESI - ukazatel na VFS_Inode
;         ES:EDI - vystupni buffer
;         FS     - pamet pro nacteni indirect bloku
;         EAX    - cislo prvni nacitane adresovaci polozky
;         ECX    - pocet adresovacih polozek, ktere se maji nacist
; Vystup: EDI - ukazatel na konec dat ve vystupnim bufferu
Inode_Read_Double_Indirect Proc
       push  ebp es ds esi ebx edx eax edi
       push  eax edi es esi ds
       push  fs
       pop   es
        xor  edi, edi
        mov  eax, dword ptr ds:[esi+VFS_I_Inode_Blocks+13*4]
        lds  esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
        call read_disk_block
       pop   ds esi es edi eax
        jc   Inode_Read_Double_Indirect_false
        ; nacteme 0. podadresar bouble indirect bloku
        shl  eax, 2
        mov  ebx, eax
        ; spocteme kolik se toho ma vlastne nacist
        shl  ecx, 2
       push  eax
        mov  eax, ecx
        shl  eax, 2
        xor  edx, edx
        div  dword ptr ds:[esi+VFS_I_Block_Size]
        mov  ecx, eax
       pop   eax
        ; ECX - kolik bloku se ma nacist
        ; EDX - kolik bajtu se ma nacist v poslednim bloku
        ; EBX - linearni offsetova adresa prvni adresovaci polozky

       push  eax edx
        mov  eax, ebx
        xor  edx, edx
        div  dword ptr ds:[esi+VFS_I_Block_Size]
        mov  ebp, edx
        mov  ebx, eax
       pop   edx eax
        ; EBP - od kolikateho bytu se maji brat data z 1. nacitaneho bloku
        ; EBX - kolikatym blokem se maji zacit nacitat adresacni data
        or   ebp, ebp
        jz   short Inode_Read_Double_Indirect_1
        ; nacteme potrebne data z 1. bloku
        mov  eax, dword ptr FS:[ebx]
        add  ebx, 4
       push  edi esi ds
        lds  esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
        call read_disk_block
       pop   ds esi edi
        jc   short Inode_Read_Double_Indirect_false
Inode_Read_Double_Indirect_2:
       push  ecx esi ds
        ; prekopirujeme EDX bytu
       push  es
       pop   ds
        mov  esi, 4096
        sub  esi, ebp
        mov  ecx, ebp
        shr  ecx, 2
        cld
        rep  movsd
       pop   ds esi ecx
Inode_Read_Double_Indirect_1:
        or   ecx, ecx
        jz   short Inode_Read_Double_Indirect_last
Inode_Read_Double_Indirect_loop:
        mov  eax, dword ptr FS:[ebx]
        call read_disk_block
        jc   short Inode_Read_Double_Indirect_false
        add  ebx, 4
        dec  ecx
        jnz  Inode_Read_Double_Indirect_loop
Inode_Read_Double_Indirect_last:
        or   edx, edx
        jz   short Inode_Read_Double_Indirect_true
        ; nacteme posledni blok
        mov  eax, dword ptr FS:[ebx]
       push  es edi esi es
       push  fs
       pop   es
        xor  edi, edi
        lds  esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
        call read_disk_block
       pop   es esi edi es
        jc   short Inode_Read_Double_Indirect_false
        xor  esi, esi
       push  fs
       pop   ds
        ; DS:ESI - ukazatel do indirect pameti, ve ktere je ulozeny posledni nacitany blok
        ; ES:EDI - vystupni buffer
        shr  edx, 2
        mov  ecx, edx
        cld
        rep  movsd
Inode_Read_Double_Indirect_true:
       pop   eax ; edi
        clc  ;CF=0
        jmp  short Inode_Read_Double_Indirect_end
Inode_Read_Double_Indirect_false:
       pop   edi
        stc  ;CF=1
Inode_Read_Double_Indirect_end:
       pop   eax edx ebx esi ds es ebp
        ret
Inode_Read_Double_Indirect endp

; Vstup:  DS:ESI - ukazatel na VFS_Inode
;         ES:EDI - vystupni buffer
;         FS     - pamet pro nacteni indirect bloku
;         GS     - pamet pro nacteni bouble indirect bloku
;         EAX    - cislo prvni nacitane adresovaci polozky
;         ECX    - pocet adresovacih polozek, ktere se maji nacist
;!!!! dodelat !!!!!
Inode_Read_Triple_Indirect Proc
        ret
Inode_Read_Triple_Indirect endp

; navrati pole indexu bloku obsahujicich data souboru
; !!!! funguje jen s 4k bloky
; Vstup:  DS:ESI - ukazatel na polozku VFS_Inode
;         EAX    - pocatecni 4k blok souboru (od 0)
;         EBX    - posledni 4k blok souboru
;                - pokud EBX=0FFFFFFFFh pak se namapuje az do konce souboru
; Vystup: CF = 0 - OK
;                - ES = selector na pamet, kde jsou ulozeny cisla bloku pro dany soubor
;                - EAX = pocet nactenych adresovacich polozek
;         CF = 1 - Error
EXT2_File_Get_Map proc
        call kernel_lock
       push ebx ecx edx ebp ds esi gs fs eax es edi
        mov   ecx, dword ptr ds:[esi+VFS_I_Number_Pages]
        or    ecx, ecx
        jz    EXT2_File_Get_Map_false
        xor   ebp, ebp
        cmp   eax, ebx
        jl    EXT2_File_Get_Map_false
        ; zjistime, jestli pozadovany blok nezasahuje mimo soubor
        cmp   ebx, ecx
        jge   short EXT2_File_Get_Map_0
        mov   ebx, ecx
        dec   ebx
EXT2_File_Get_Map_0:
       ; alokujeme pamet pro adresovaci cisla bloku
       ; mem_size = (ebx-eax)*4
       push  eax ebx
        sub  ebx, eax
        inc  ebx
        mov  ecx, ebx
        shl  ebx, 2
        mov  eax, ebx
        call Alloc
       pushfd
        mov  es, eax
       popfd
       pop   ebx eax
        jc   EXT2_File_Get_Map_false

        ; ES - vystupni descriptor
        ; ECX - kolik adresovacich polozek se ma nacist
        ; alokujeme pamet pro indirect block
       push  eax
        mov  eax, 4096
        call Alloc
        mov  ebx, eax
       pop   eax
        jnc  short EXT2_File_Get_Map_10
        ; odalokujeme vystupni buffer
        mov  eax, es
        mov  ebx, data_descriptor
        mov  es, ebx
        call dealloc
        jmp  EXT2_File_Get_Map_false
EXT2_File_Get_Map_10:
        mov  fs, ebx

        ; FS - pamet pro indirect block
        ; indirect block, ve kterem jsou ukazatele na double indirect bloky
        ; double indirect bloky se budou nacitat primo do vystupniho bufferu
        ; pokud samozrejme budeme nacitat jen double indirect bloky

        ; alokujeme pamet pro double indirect block
       push  eax
        mov  eax, 4096
        call Alloc
        mov  ebx, eax
       pop   eax
        jnc  short EXT2_File_Get_Map_11
        ; odalokujeme vystupni buffer
        mov  eax, es
        mov  ebx, data_descriptor
        mov  es, ebx
        call dealloc
        ; odalokujeme 1. pomocny buffer
        mov  eax, fs
        mov  ebx, data_descriptor
        mov  fs, ebx
        call dealloc
        jmp  EXT2_File_Get_Map_false
EXT2_File_Get_Map_11:
        mov  gs, ebx

        xor  edi, edi
        ; GS - pamet pro double indirect block
        ; ECX - kolik adresovacich polozek se ma nacist
        ; ES:EDI - vystupni buffer
        ; FS - indirect blok
        ; GS - double indirect block

        cmp  eax, 12
        jg   Inode_Load_direct_1
       ; prekopirujeme do pomocneho bufferu prvnich 12 odkazu na bloky
       push  esi ecx eax
        cmp  ecx, 12
        jle  short Inode_Load_direct_0
        mov  ecx, 12
        sub  ecx, eax
Inode_Load_direct_0:
        shl  eax, 2
        add  esi, eax
        add  esi, VFS_I_Inode_Blocks
        mov  edx, ecx
        cld
        rep  movsd
       pop   eax ecx esi
        cmp  ecx, edx
        jge  short Inode_Load_direct_0_0
        xor  ecx, ecx
        jmp  short Inode_Load_direct_0_1
Inode_Load_direct_0_0:
        sub  ecx, edx
Inode_Load_direct_0_1:
        add  ebp, edx
        xor  eax, eax
        jmp  short Inode_Load_direct_1_X
; vetsi nez 12 bloku
Inode_Load_direct_1:
        sub  eax, 12
Inode_Load_direct_1_X:
        cmp  eax, 1024
        jg   Inode_Load_direct_2
        or   ecx, ecx
        jz   EXT2_File_Get_Map_true
        ; ECX - kolik polozek se ma jeste nacist
        ; EAX - pozice ve 4k od ktere na ktere se maji nacist adresy
        mov  edx, ecx
        cmp  ecx, 1024
        jle  short Inode_Load_direct_1_1
        mov  edx, 1024
        sub  edx, eax
Inode_Load_direct_1_1:
        ; EDX - kolik bloku se ma nacist
       push  eax ecx edx
        mov  ecx, edx
        call Inode_Read_One_Indirect
       pop   edx ecx eax
        cmp  ecx, edx
        jge  short Inode_Load_direct_2_0
        xor  ecx, ecx
        jmp  Inode_Load_direct_2_1
Inode_Load_direct_2_0:
        sub  ecx, edx
Inode_Load_direct_2_1:
        add  ebp, edx
        xor  eax, eax
        jmp  short Inode_Load_direct_2_X
; vetsi nez (EBP div 4)+12 bloku
Inode_Load_direct_2:
        sub  eax, 1024
Inode_Load_direct_2_X:
        cmp  eax, 1024*1024
        jg   Inode_Load_direct_3
        or   ecx, ecx
        jz   short EXT2_File_Get_Map_true
        mov  edx, ecx
        cmp  ecx, 1024*1024
        jle  short Inode_Load_direct_2_3
        mov  edx, 1024*1024
        sub  edx, eax
Inode_Load_direct_2_3:
        ; EDX - kolik polozek se ma nacist
       push eax ecx edx
        mov  ecx, edx
        call Inode_Read_Double_Indirect
       pop  edx ecx eax
        cmp  ecx, edx
        jge  short Inode_Load_direct_3_0
        xor  ecx, ecx
        jmp  short Inode_Load_direct_3_1
Inode_Load_direct_3_0:
        sub  ecx, edx
Inode_Load_direct_3_1:
        add  eax, edx
        add  ebp, edx
        jmp  short Inode_Load_direct_3_X
; vetsi nez (EBP div 4)*(EBP div 4)+(EBP div 4)+12 bloku
Inode_Load_direct_3:
        sub  eax, 1024*1024
Inode_Load_direct_3_X:
        or   ecx, ecx
        jz   short EXT2_File_Get_Map_true
        mov  edx, ecx
        cmp  ecx, 1024*1024*1024
        jle  short Inode_Load_direct_3_3
        mov  edx, 1024*1024*1024
        sub  edx, eax
Inode_Load_direct_3_3:
        shl  eax, 10
        mov  ecx, edx
        call Inode_Read_Triple_Indirect
        add  ebp, edx
EXT2_File_Get_Map_True:
        mov  eax, fs
       pop   ebx fs ebx ; edi es eax
       pop   fs
        call dealloc
        mov  eax, gs
       pop   gs
        call dealloc
        mov  eax, ebp
        clc  ;CF=0
        jmp  short EXT2_File_Get_Map_end
EXT2_File_Get_Map_False_memory:
       pop   edi
        mov  eax, es
       pop   es ebx ; eax
        call dealloc
        mov  eax, fs
       pop   fs
        call dealloc
        mov  eax, gs
       pop   gs
        call dealloc
        mov  eax, ebx
        stc  ;CF=1
        jmp  short EXT2_File_Get_Map_end
EXT2_File_Get_Map_false:
       pop  edi es eax fs gs
        stc  ;CF=1
EXT2_File_Get_Map_end:
       pop  esi ds ebp edx ecx ebx
        call kernel_unlock
        retf
EXT2_File_Get_Map Endp

; cteni bloku dat ze souboru
; Vstup:  EAX    - kolikaty 4k blok souboru se ma nacist
;         DS:ESI - ukazatel na polozku VFS_Inode
;         ES:EDI = vystupni buffer pro nacitane data
; Vystup: CF = 0 - OK
;         CF = 1 - Error
EXT2_File_read          Proc
        retf
EXT2_File_read          endp

; universalni fce pro cteni ze souboru
EXT2_File_write         Proc ;  zapis do souboru
        retf
EXT2_File_write         endp

; nepotrebna fce, za ni to bude delat VFS
EXT2_File_seek          Proc ;  posouvani ukazatele v souboru
        retf
EXT2_File_seek          endp

;  navraci buffer, ve kterem je zachycen obraz obsahu adresare
; Vstup:  EAX - cislo inody souboru, ktery otevirame
;         EBX - cislo nadrazene inody
;         DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni, ze ktereho prave cteme
; Vystup: CF = 0 - Ok
;                - ES:EDI - vystupni buffer
;                - Struktura vystupniho bufferu
;                - DD cislo inody ktera je prirazena nazvu polozky
;                - pokud je toto cislo inody ve vyst. buf. nulova
;                  pak se jedna o ukonceni, cili tato polozka je posledni
;                  a uz neplatna !
;                - retezec s nazvem adresarove polozky ukonceny na 0
;         CF = 1 - Error
EXT2_File_read_dir      Proc
      push   eax ebx ecx edx esi ds edi es
       ; otevreme inodu "adresar"
       mov   edx, 1
       call  VFS_Open_Main
       jc    EXT2_File_read_dir_false
       ; testneme, jestli je to opravdu adresar
       call  VFS_Is_Inode_Directory
       jc    EXT2_File_read_dir_false_close
       ; namapujeme cely soubor do pameti
       xor   ebx, ebx
       mov   ecx, 0FFFFFFFFh
       call  VFS_Map_File
       jc    EXT2_File_read_dir_false_close
       ; zjistime potrebnou velikost vystupniho bufferu
      push   edi ds esi eax edx
       mov   ds, edx
       lsl   edx, edx
       sub   edx, d_Dir_name
       xor   esi, esi
       xor   ecx, ecx
       ; DS:ESI - namapovany adresar
EXT2_File_read_dir_Loop_2:
       movzx eax, byte ptr ds:[esi+d_name_len]
       add   ecx, eax
       movzx ebx, word ptr ds:[esi+d_rec_len]
       add   esi, ebx
       cmp   esi, edx
       jl    short EXT2_File_read_dir_loop_2
      pop    edx eax esi ds edi
       ; ECX - minimalni velikost vytupniho bufferu
       ; alokujeme vystupni buffer
      push   edx
       mov   edx, ecx
       call  TSKMM_Alloc_B
      push   edx
      pop    ecx
      pop    edx
       jnc   short EXT2_File_read_0
       jmp   EXT2_File_read_dir_false_close
EXT2_File_read_0:
       mov   es, ecx
       xor   edi, edi
       ; prekopiruji dir
      push   edi ds esi eax edx
       mov   ds, edx
       lsl   edx, edx
       sub   edx, d_Dir_name
       xor   esi, esi
       ; DS:ESI - namapovany adresar
EXT2_File_read_dir_Loop:
       mov   eax, dword ptr ds:[esi+d_inode_num]
       stosd
      push   esi
       movzx ecx, byte ptr ds:[esi+d_name_len]
       add   esi, d_Dir_name
       cld
       rep   movsb
      pop    esi
       xor   eax, eax
       stosb
       movzx ebx, word ptr ds:[esi+d_rec_len]
       add   esi, ebx
       cmp   esi, edx
       jl    short EXT2_File_read_dir_loop
       xor   eax, eax
       stosd
      pop    eax
      push   edx
       mov   edx, eax
       call  TSKMM_Dealloc
      pop    edx
      pop    eax esi ds edi
       call  VFS_Close
EXT2_File_read_dir_true:
      pop    ds esi ; es edi
       clc   ;CF=0
       jmp   short EXT2_File_read_dir_end
EXT2_File_read_dir_false:
      pop    es edi
       stc   ;CF=1
       jmp   short EXT2_File_read_dir_end
EXT2_File_read_dir_false_close:
       call  VFS_Close
      pop    es edi
       stc   ;CF=1
EXT2_File_read_dir_end:
      pop    ds esi edx ecx ebx eax
       retf
EXT2_File_read_dir      endp

; Vstup:  EAX - cislo inody souboru, ktery otevirame
;         ES:EDI - ukazatel na polozku VFS inody, ktera je prirazena
;                  teto inode
; Vystup: CF = 0 - Ok
;         CF = 1 - Error
;                - EAX - error kod
;                      - 0 = Nedostatek pameti
;                      - 1 = chybne cislo inody
;                      - 2 = chyba pri cteni ze zarizeni
EXT2_File_open          Proc
       push   ebx ecx edx edi esi ds fs eax es
        ; nactu inodu
       push   es
       pop    ds
        mov   esi, edi
        ; DS:ESI - ukazatel na VFS inodu
        ; alokujeme pamet pro nacteni inody
       push   eax
        mov   eax, 128
        call  Alloc
       pushfd
        mov   ebx, eax
       popfd
       pop    eax
        jc    EXT2_File_open_False
        mov   es, ebx
        xor   edi, edi
        ; ES:EDI - temp buffer

       push   ds esi
        lds   esi, fword ptr ds:[esi+VFS_I_Mount_Struc]
        lfs   ebx, fword ptr ds:[esi+VFS_MS_FS_Infos]
        lfs   ebx, fword ptr fs:[ebx+VFS_FS_Inode_Ops]
        call  fword ptr fs:[ebx+FS_Inode_Ops_Load]
       pop    esi ds
        jc    EXT2_File_open_False_memory

        ; prekopiruji cisla datovych bloku inody(souboru)
        mov   ecx, 15
       push   edi esi
        mov   edi, i_block
        add   esi, VFS_I_Inode_Blocks
EXT2_File_open_2:
        mov    eax, dword ptr es:[edi]
        mov    dword ptr ds:[esi], eax
        add    edi, 4
        add    esi, 4
        dec    ecx
        jnz    short EXT2_File_open_2
       pop    esi edi

        mov   eax, dword ptr es:[i_mode]
        mov   dword ptr ds:[esi+VFS_I_MODE], eax
        ; zjistime jestli se muze nastavit sdileni
       push   eax
        and   eax, EXT2_S_IFDIR
        cmp   eax, EXT2_S_IFDIR
       pop    eax
        jz    short EXT2_File_open_20
        and   eax, EXT2_S_IFFIL
        cmp   eax, EXT2_S_IFFIL
        jz    short EXT2_File_open_20
        ; pokud se nejedna o adresar nebo soubor tak se nastavi sdileni VFS inody
        or    dword ptr ds:[esi+VFS_I_Open_Mode], 100h
EXT2_File_open_20:
        ; zjistime, jestli je inoda nastavena jako append write only
        movzx eax, word ptr es:[i_flags]
        and   eax, 100000b
        cmp   eax, 100000b
        jnz   short EXT2_File_open_21
        or    dword ptr ds:[esi+VFS_I_Open_Mode], 1000h
EXT2_File_open_21:

        ; ID vlastnika souboru
        mov   ax, word ptr es:[i_uid]
        mov   word ptr ds:[esi+VFS_I_UID], ax
        ; ID groupy vlastniku
        mov   ax, word ptr es:[i_gid]
        mov   word ptr ds:[esi+VFS_I_GID], ax
        ; velikost korenoveho adresare
        mov   eax, dword ptr es:[i_size]
       push   eax
        mov   dword ptr ds:[esi+VFS_I_File_Size], eax
        ; pocet potrebnych 512b bloku pro ulozeni celeho souboru
        mov   eax, dword ptr es:[i_blocks]
        mov   dword ptr ds:[esi+VFS_I_Number_Blocks], eax
        ; kolik stranek zabira soubor v pameti
        ; tahle polozka se zvysuje, kdyz se alokuje dalsi fyz. page pro dany soubor
       pop    eax
        or    eax, eax
        jz    short EXT2_File_open_1
        xor   edx, edx
        add   eax, dword ptr ds:[esi+VFS_I_Block_Size]
        dec   eax
        div   dword ptr ds:[esi+VFS_I_Block_Size]
EXT2_File_open_1:
        mov   dword ptr ds:[esi+VFS_I_Number_Pages], eax
EXT2_File_open_True:
        mov   eax, es
       pop    es
        call  DeAlloc
       pop    eax
        clc   ;CF=0
        jmp   short EXT2_File_open_End
EXT2_File_open_False_Memory:
        mov   ecx, eax
        mov   eax, es
       pop    es
        call  DeAlloc
       pop    ebx
        mov   eax, ecx
        stc   ; CF=1
        jmp   short EXT2_File_open_End
EXT2_File_open_False:
       pop    es eax
        xor   eax, eax ; nedostatek pameti
        stc   ; CF=1
EXT2_File_open_End:
       pop    fs ds esi edi edx ecx ebx
        retf
EXT2_File_open          endp

; tato fce je zatim nepotrebna a bez uzitku
EXT2_File_release       Proc ;  tato fce se zavola, kdyz se uzavre posledni rukojet na danou VFS inodu
        retf
EXT2_File_release       endp

; podle me by se soubor mel uzamikat jen ve VFSku, nevim o zadne podpore
; uzamikani souboru v ext2
EXT2_File_poll          Proc ;  zjisti, jestli neni soubor uzamcen
        retf
EXT2_File_poll          endp

; tohle by taky melo delat VFSko
EXT2_File_truncate      Proc ;  usekne soubor od aktualni pozice ukazatele v souboru
        retf
EXT2_File_truncate      endp

; ulozi obraz inody na patricne misto
; Vstup:  EAX    - cislo inody zarizeni
;         DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni
;         ES:EDI - vstupni buffer s obrazem inody
; Vystup: CF = 0 - OK
;         CF = 1 - False
EXT2_Save_Inode Proc
          pushad
          push   ds es fs gs
           or    eax, eax
           jz    EXT2_Save_Inode_false
           mov   FS, word ptr DS:[esi+VFS_MS_Cache_Area]
           mov   gs, word ptr fs:[Ext2_Groups_Sel]
           mov   fs, word ptr fs:[Ext2_Super_Block_Sel]
           cmp   eax, dword ptr FS:[s_inodes_count]
           jg    EXT2_Save_Inode_false
           Call  EXT2_Inode_Exists
           jc    EXT2_Save_Inode_false
           dec   eax
           xor   edx, edx
           div   dword ptr FS:[s_inodes_per_group]
           ; EAX - cislo groupy, ve ktere je inoda ulozena
           ; EDX - index inody v groupe (0..s_inodes_per_group-1)
           shl   eax, 5
           shl   edx, 7 ;sizeof(tInode)=128

          push   eax
           mov   eax, dword ptr FS:[s_inodes_per_group]
           shr   eax, 3
           ; EAX - kolik bajtu bude zabirat bitmapa inodu pro danou groupu
           call  Alloc
          pushfd
           mov   ebx, eax
          popfd
          pop    eax
           jc    EXT2_Save_Inode_false
           mov   gs, ebx
          PUSH EDX
           mov   fs, word ptr DS:[esi+VFS_MS_Cache_Area]
          push   gs
           mov   gs, word ptr fs:[Ext2_Groups_Sel]
           mov   ebx, eax
           mov   eax, dword ptr GS:[bg_inode_table+ebx]
          pop    gs
           mul   dword ptr FS:[Ext2_Block_Size]
          POP EBX
           add   eax, ebx
           adc   edx, 0
           ; EDX:EAX - pozice
           div   dword ptr FS:[Ext2_Block_Size]
          push   es edi
          push   gs
          pop    es
           xor   edi, edi
           mov   ecx, gs
          push   ds
          pop    gs
           mov   ebx, esi
           ; EAX - cislo bloku, ktery nacteme
           ; EDX - pozice zacatku struktury inody, kterou nacitame
           ; ES:EDI - temp buffer
          push   ecx
           call  Read_disk_block
          pop    ecx
          pop    esi ds
           jc    EXT2_Save_Inode_false_dealloc
           ;  GS:EBX - mount struc
           mov   edi, edx
          push   ecx
           mov   ecx, EXT2_Inode_Size/4 ; 128 B
           cld
           rep   movsd
          pop    ecx
           xor   edi, edi
          push   gs
          pop    ds
           mov   esi, ebx
          push   ecx
           call  Write_disk_block
          pop    ecx
           jc    EXT2_Save_Inode_false_dealloc
EXT2_Save_Inode_true:
           ; odalokujeme temp buffer
           mov   eax, ecx
          pop    gs
           call  DeAlloc
           clc   ;CF=0
           jmp   short EXT2_Save_Inode_end
EXT2_Save_Inode_false_dealloc:
           ; odalokujeme temp buffer
           mov   eax, ecx
          pop    gs
           call  DeAlloc
           stc   ;CF=1
           jmp   short EXT2_Save_Inode_end
EXT2_Save_Inode_false:
          pop    gs
           stc   ;CF=1
EXT2_Save_Inode_end:
          pop    fs es ds
          popad
           ret
EXT2_Save_Inode Endp

; specialni fce ext2
; Vstup:  DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni
;         EBX - cislo fce
;             - 0 = ulozi obraz inody na patricne misto
;                 - Vstup:  EAX    - cislo inody zarizeni
;                           ES:EDI - vstupni buffer s obrazem inody
;                 - Vystup: CF = 0 - OK
;                           CF = 1 - False
; Vystup: zalezi na volane fci, ovsem pokud fce neni nalezena tak CF=1
;         jinak CF=0
EXT2_File_io_cmd        Proc ;  vstupne vystupni funkce pro manipulaci s inodou (napr. u zarizeni meni jeho typove cislo, atd.)
        or   ebx, ebx
        jz   short EXT2_File_io_cmd_EXT2_Save_Inode
        jmp  EXT2_File_io_cmd_False

EXT2_File_io_cmd_EXT2_Save_Inode:
        call  EXT2_Save_Inode
        jmp   short EXT2_File_io_cmd_End

EXT2_File_io_cmd_True:
        clc   ;CF=0
        jmp   short EXT2_File_io_cmd_End
EXT2_File_io_cmd_False:
        stc   ;CF=1
EXT2_File_io_cmd_End:
        retf
EXT2_File_io_cmd        endp

; tato fce mozna ani nebude potrebna, protoze to vsechno bude delat VFS a SWAPD
EXT2_File_fsync         Proc ;  zapise data souboru z pameti na disk
        retf
EXT2_File_fsync         endp

; tato fce. by mela byt ve VFS
EXT2_File_lock          Proc ;  uzamkne VFS inodu, na kterou ukazuje rukojet
        retf
EXT2_File_lock          endp

; Najde volnou inodu na zarizeni
; Vstup:  DS:ESI - ukazatele na mount_tab polozku danho zarizeni
; Vystup: CF = 0 - OK
;                - EAX - cislo inody
;                - EBX - cislo groupy, ve ktere byla inoda alokovana
;                - ES  - buffer s nactenym blokem s bitmapou
;                      - je pripraveny k zapsani na disk, cili inoda
;                        je uz i zalokovana
;         CF = 1 - Error
EXT2_Find_Free_Inode Proc
      push   ebp ds fs gs edx esi edi ecx ebx eax es
       mov   gs, word ptr ds:[esi+VFS_MS_Cache_Area]
       mov   ecx, dword ptr gs:[Ext2_Count_Groups]
       ; ECX - pocet group na zarizeni
       mov   fs, word ptr gs:[Ext2_Super_Block_Sel]
       ; pocet volnych inod na zarizeni
       cmp   dword ptr fs:[s_free_inodes_count], 0
       jz    EXT2_Find_Free_Inode_False
       ; pocet volnych bloku na zarizeni
       cmp   dword ptr fs:[s_free_blocks_count], 0
       jz    EXT2_Find_Free_Inode_False

       mov   eax, dword ptr gs:[Ext2_Block_Size]
       call  Alloc
       jc    EXT2_Find_Free_Inode_False
       mov   es, eax
       xor   edi, edi
       ; ES:EDI - temp buffer

       mov   edx, dword ptr fs:[s_inodes_per_group]
       xor   eax, eax ; cislo inody
       xor   ebx, ebx
       mov   fs, word ptr gs:[Ext2_Groups_Sel]
       ; najdeme groupu do ktere alokujeme inodu
      push   esi
       xor   edi, edi
       xor   ebp, ebp
       xor   esi, esi
EXT2_Find_Free_Inode_Loop:
       cmp   dword ptr fs:[ebx+bg_free_blocks_count], 8
       jl    short EXT2_Find_Free_Inode_Loop_End
       cmp   dword ptr fs:[ebx+bg_free_inodes_count], 0
       jz    short EXT2_Find_Free_Inode_Loop_End

       cmp   edi, dword ptr fs:[ebx+bg_free_inodes_count]
       jg    short EXT2_Find_Free_Inode_Loop_End

       mov   edi, dword ptr fs:[ebx+bg_free_inodes_count]
       mov   ebp, ecx
       mov   esi, ebx

EXT2_Find_Free_Inode_Loop_End:
       dec   ecx
       jz    EXT2_Find_Free_Inode_Loop_End_total
       add   eax, edx
       add   ebx, EXT2_Group_Size
       jmp   EXT2_Find_Free_Inode_Loop
EXT2_Find_Free_Inode_Loop_End_total:
       or    edi, edi
       jz    EXT2_Find_Free_Inode_0
       mov   ebx, esi
       jmp   EXT2_Find_Free_Inode_1
EXT2_Find_Free_Inode_0:
      pop    esi
       jmp   EXT2_Find_Free_Inode_False_memory
EXT2_Find_Free_Inode_1:
      pop    esi

; najdeme volnou inodu v ramci nalezene groupy
      push   eax
       mov   eax, dword ptr fs:[ebx+bg_inode_bitmap]
       xor   edi, edi
       ; nacteme blok s inode bitmapou
       call  Read_disk_block
      pop    eax
       jc    EXT2_Find_Free_Inode_False
       ; najdeme volnou inodu
       xor   edi, edi
       mov   edx, dword ptr gs:[Ext2_Block_Size]
EXT2_Find_Free_Inode_Loop_100:
      push   eax
       mov   al, byte ptr es:[edi]
       cmp   al, 0FFh
       jz    EXT2_Find_Free_Inode_Loop_100_next
      push   ebx
       xor   ebx, ebx
EXT2_Find_Free_Inode_Loop_101:
       ror   al, 1
       jnc   short EXT2_Find_Free_Inode_Loop_102
       inc   ebx
       cmp   ebx, 8
       jnz   short EXT2_Find_Free_Inode_Loop_101
      pop    ebx eax
       jmp   EXT2_Find_Free_Inode_Loop_End
; byla nalezena volna inoda
EXT2_Find_Free_Inode_Loop_102:
      push   eax ecx ebx
       movzx eax, byte ptr es:[edi]
       mov   ecx, 1
       xchg  ebx, ecx
       shl   ebx, cl
       or    eax, ebx
       mov   byte ptr es:[edi], al
      pop    ebx ecx eax
       shl   edi, 3
       add   edi, ebx
      pop    ebx eax
       add   eax, edi
       jmp   EXT2_Find_Free_Inode_True
EXT2_Find_Free_Inode_Loop_100_next:
      pop    eax
       inc   edi
       dec   edx
       jnz   EXT2_Find_Free_Inode_Loop_100
EXT2_Find_Free_Inode_False_memory:
       ;odalokujeme temp buffer
       mov   eax, es
      pop    es
       call  DeAlloc
      pop    eax ebx
       stc   ;CF=1
       jmp   short EXT2_Find_Free_Inode_End
EXT2_Find_Free_Inode_False:
      pop    es eax ebx
       stc   ;CF=1
       jmp   short EXT2_Find_Free_Inode_End
EXT2_Find_Free_Inode_True:
       mov   ebx, dword ptr fs:[ebx+bg_inode_bitmap]
      pop    fs ; es
      pop    edx edx ; eax ebx
       mov   ebx, ebp
       dec   ebx
       inc   eax
       clc   ;CF=0
EXT2_Find_Free_Inode_End:
      pop    ecx edi esi edx gs fs ds ebp
       ret
EXT2_Find_Free_Inode Endp

; Najde volne bloky na zarizeni
; Vstup:  EAX - Inoda, pro kterou alokujeme bloky
;         ECX - pocet bloku, ktere maji byt alokovany
;         DS:ESI - ukazatele na mount_tab polozku daneho zarizeni
; Vystup: CF = 0 - OK
;                - ES  - buffer s cisly volnych bloku
;         CF = 1 - Error
EXT2_Find_Free_Blocks Proc
       ret
EXT2_Find_Free_Blocks Endp

; Alokujeme inodu v groupe
; Vstup:  DS:ESI - ukazatele na mount_tab polozku daneho zarizeni
;         ES     - buffer s blokokem s bitmapu inod
;         EBX    - cislo groupy, ve ktere je inoda
; Vystup: CF = 0 - OK
;            ES  - je odalokovan
;         CF = 1 - Error
;            ES  - nebyl odalokovan
EXT2_Allocate_Inode Proc
      pushad
      push   fs ds edi esi es
EXT2_Allocate_Inode_true:
      push   es
       mov   es, word ptr ds:[esi+VFS_MS_Cache_Area]
       mov   fs, word ptr es:[Ext2_Super_Block_Sel]
       ; pocet volnych inod na zarizeni
       dec   dword ptr fs:[s_free_inodes_count]
       mov   fs, word ptr es:[Ext2_Groups_Sel]
       ; pocet volnych inod v groupe
       mov   eax, EXT2_Group_Size
       mul   ebx
       mov   ebx, eax
      pop    es
       mov   eax, dword ptr fs:[ebx+bg_inode_bitmap]
       xor   edi, edi
       ; zapiseme blok s inode bitmapou
       call  Write_disk_block
       jc    short EXT2_Allocate_Inode_false
       ; odalokujeme buffer
       mov   eax, es
      pop    es
      push   data_descriptor
      pop    es
       call  DeAlloc
       ; upravime groupy
       dec   dword ptr fs:[ebx+bg_free_inodes_count]
       les   edi, fword ptr ds:[esi+VFS_MS_FS_Infos]
       les   edi, fword ptr es:[edi+VFS_FS_Super_Ops]
       ; zapise superblock a groupy na disk
       Call  fword ptr es:[edi+FS_Super_Ops_Write_Super]
       jc    short EXT2_Allocate_Inode_false
       clc   ;CF=0
       jmp   short EXT2_Allocate_Inode_end
EXT2_Allocate_Inode_false:
      pop    es
       stc   ;CF=1
EXT2_Allocate_Inode_end:
      pop    esi edi ds fs
      popad
       ret
EXT2_Allocate_Inode Endp

; Alokujeme bloky v groupe
; Vstup:  DS:ESI - ukazatele na mount_tab polozku daneho zarizeni
;         ES     - buffer s blokokem s bitmapu inod
;         EAX    - cislo bloku s bitmapou inod pro danou groupu
;         EBX    - cislo groupy, ve ktere je inoda EAX
; Vystup: CF = 0 - OK
;         CF = 1 - Error
EXT2_Allocate_Blocks Proc
      push   edi
       xor   edi, edi
       ; zapiseme blok s inode bitmapou
       call  Write_disk_block
       jc    short EXT2_Allocate_Blocks_false
EXT2_Allocate_Blocks_true:
       clc   ;CF=0
       jmp   short EXT2_Allocate_Blocks_end
EXT2_Allocate_Blocks_false:
       stc   ;CF=1
EXT2_Allocate_Blocks_end:
      pop    edi
       ret
EXT2_Allocate_Blocks Endp

; zjisti, jestli inoda existuje
; vstup: EAX    - inoda
;        DS:ESI - ukazatel na mount struc polozku
; vystup: CF  = 0 - inoda existuje
;         CF  = 1 - inoda neexistuje
;         EAX = pokud neni stejna hodnota jako vstupni EAX, pak nastala chyba
EXT2_Inode_Exists Proc
          push   ebx ecx edx esi edi ebp ds es fs gs eax
           mov   FS, word ptr DS:[esi+VFS_MS_Cache_Area]
           mov   gs, word ptr fs:[Ext2_Groups_Sel]
           mov   fs, word ptr fs:[Ext2_Super_Block_Sel]
           dec   eax
           xor   edx, edx
           div   dword ptr FS:[s_inodes_per_group]
           ; EAX - cislo groupy, ve ktere je inoda ulozena
           ; EDX - index inody v groupe (0..s_inodes_per_group-1)
           ; provede se kontrola, jestli inoda existuje
           mov   ebx, eax
           shl   ebx, 5
           mov   eax, dword ptr FS:[s_inodes_per_group]
           shr   eax, 3
           ; EAX - kolik bajtu bude zabirat bitmapa inodu pro danou groupu
           call  Alloc
           jc    Ext2_Inode_Exists_error
           mov   es, eax
           xor   edi, edi
           ; pozdeji se sem musi dat kontrola, jestli se nema nacist vic nez
           ; jen 1 blok
           mov   eax, dword ptr gs:[ebx+bg_inode_bitmap]
           call  Read_disk_block
           jc    EXT2_Inode_Exists_error_dealloc
           ; ES:EDI - buffer s nactenou inodovou bitmapou
           ; EDX - index nacitane inody v bitmape
           mov   eax, edx
           xor   edx, edx
           mov   ebx, 8
           div   ebx
           ; EAX - byte ve kterem se nachazi bit inody, kterou nacitame
           ; EDX - index inody v bytu (0..7)
           mov   ecx, edx
           mov   ebx, eax
           movzx eax, byte ptr es:[ebx]
           mov   ebx, 1
           shl   ebx, cl
           and   eax, ebx ;skoci jestli neni shoda (jestli je inoda volna)
           cmp   eax, ebx
           jnz   Ext2_Inode_Exists_false_dealloc
EXT2_Inode_Exists_true:
           ; odalokujeme temp buffer
           mov   eax, es
           mov   ebx, data_descriptor
           mov   es, ebx
           call  DeAlloc
          pop    eax
           clc   ;CF=0
           jmp   EXT2_Inode_Exists_end
EXT2_Inode_Exists_error:
          pop    eax
           xor   eax, eax
           stc   ;CF=0
           jmp   short EXT2_Inode_Exists_end
EXT2_Inode_Exists_error_dealloc:
           ; odalokujeme temp buffer
           mov   eax, es
           mov   ebx, data_descriptor
           mov   es, ebx
           call  DeAlloc
          pop    eax
           xor   eax, eax
           stc   ;CF=0
           jmp   short EXT2_Inode_Exists_end
EXT2_Inode_Exists_false_dealloc:
           ; odalokujeme temp buffer
           mov   eax, es
           mov   ebx, data_descriptor
           mov   es, ebx
           call  DeAlloc
          pop    eax
           xor   eax, eax
           stc   ;CF=0
           jmp   short EXT2_Inode_Exists_end
EXT2_Inode_Exists_false:
          pop    eax
           stc   ;CF=0
EXT2_Inode_Exists_end:
          pop    gs fs es ds ebp edi esi edx ecx ebx
           ret
EXT2_Inode_Exists Endp

;##########################################################
; Vytvori inodu
;##########################################################
; Vstup:  DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni, na kterem chceme vytvori inodu
; Vystup: CF = 0 - OK
;                - EAX = Cislo nove vytvorene inody
;         CF = 1 - Error
EXT2_Inode_create       Proc ;  vytvori inodu
       push  es ebx eax
        call EXT2_Find_Free_Inode
        jc   EXT2_Inode_create_false
        call EXT2_Allocate_Inode
        jc   EXT2_Inode_create_false
EXT2_Inode_create_true:
       pop   ebx
        clc  ;CF=1
        jmp  short EXT2_Inode_create_end
EXT2_Inode_create_false:
       pop   eax
        stc  ;CF=1
EXT2_Inode_create_end:
       pop   ebx es
        retf
EXT2_Inode_create       endp

;##########################################################
; Precte data ulozene v dane inode
;##########################################################
; Vstup:  EAX    - cislo inody
;         DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni, ze ktereho prave cteme
;         ES:EDI - vystupni buffer
; Vystup: CF = 0 - OK
;         CF = 1 - Error
;                = EAX - Error kod
;                    0 = Nedostatek pameti
;                    1 = inoda nenalezena
;                    2 = chyba pri cteni ze zarizeni
Ext2_Inode_Load Proc
         pushad
          push   ds es fs gs
           or    eax, eax
           jz    Ext2_Inode_Load_Error_1
           mov   FS, word ptr DS:[esi+VFS_MS_Cache_Area]
           mov   gs, word ptr fs:[Ext2_Groups_Sel]
           mov   fs, word ptr fs:[Ext2_Super_Block_Sel]
           cmp   eax, dword ptr FS:[s_inodes_count]
           jle   Ext2_Inode_Load_Run
Ext2_Inode_Load_Error_1:
          pop    gs fs es ds
          popad
           mov   eax, 1
           stc   ;CF=1
           retf
Ext2_Inode_Load_Error_0:
          pop    gs fs es ds
          popad
           xor   eax, eax
           stc   ;CF=1
           retf
Ext2_Inode_Load_Error_2:
           ; odalokujeme temp buffer
           mov   eax, gs
           mov   ebx, data_descriptor
           mov   gs, ebx
           call  DeAlloc
          pop    gs fs es ds
          popad
           mov   eax, 2
           stc   ;CF=1
           retf
Ext2_Inode_Load_Run:
           Call  EXT2_Inode_Exists
           jc    Ext2_Inode_Load_Error_1
           dec   eax
           xor   edx, edx
           div   dword ptr FS:[s_inodes_per_group]
           ; EAX - cislo groupy, ve ktere je inoda ulozena
           ; EDX - index inody v groupe (0..s_inodes_per_group-1)
           shl   eax, 5
           shl   edx, 7 ;sizeof(tInode)=128

          push   eax
           mov   eax, dword ptr FS:[s_inodes_per_group]
           shr   eax, 3
           ; EAX - kolik bajtu bude zabirat bitmapa inodu pro danou groupu
           call  Alloc
          pushfd
           mov   ebx, eax
          popfd
          pop    eax
           jc    Ext2_Inode_Load_Error_0
           mov   gs, ebx

          PUSH EDX
           mov   fs, word ptr DS:[esi+VFS_MS_Cache_Area]
          push   gs
           mov   gs, word ptr fs:[Ext2_Groups_Sel]
           mov   ebx, eax
           mov   eax, dword ptr GS:[bg_inode_table+ebx]
          pop    gs
           mul   dword ptr FS:[Ext2_Block_Size]
          POP EBX
           add   eax, ebx
           adc   edx, 0
           ; EDX:EAX - pozice
           div   dword ptr FS:[Ext2_Block_Size]
          push   es edi
          push   gs
          pop    es
           xor   edi, edi
           ; EAX - cislo bloku, ktery nacteme
           ; EDX - pozice zacatku struktury inody, kterou nacitame
           ; ES:EDI - temp buffer
           call  Read_disk_block
          pop    edi es
           jc    Ext2_Inode_Load_Error_2
          push   gs
          pop    ds
           mov   esi, edx
           mov   ecx, EXT2_Inode_Size/4 ; 128 B
           cld
           rep   movsd
           ; odalokujeme temp buffer
           mov   eax, gs
           mov   ebx, data_descriptor
           mov   gs, ebx
           call  DeAlloc
          pop    gs fs es ds
          popad
           clc   ;CF=0
           retf
Ext2_Inode_Load Endp

EXT2_Inode_rename       Proc ;  prejmenuje inodu
        retf
EXT2_Inode_rename       endp

; podle cesty najde inodu a vrati jeji cislo
; Vstup:  DS:ESI - cesta k souboru,adresari,... zakonceny na 0
;         EAX    - VFS inoda, na kterou je namontovane zarizeni,
;                  ktere bude lookup prohledavat
; Vystup: EAX - cislo inody
;         EBX - cislo nadrazene inody
;         ECX - vysledek dledani cesty
;             - 0 = Cesta byla nalezena
;             - 1 = Byla nalezena jen cast cesty, mozna pokracuje
;                   na dalsim zarizeni
;             - 2 = Nastala chyba
;         ESI - ukazuje na zacatek v ceste, kde tento lookup zkoncil
;               Napr. puvodni cesta je "/mnt/dos/bp",0 a na adresar
;               "/mnt/dos" je namontovane zarizeni, v tom pripade
;               lookup navrati ECX="1", v EAX je inoda adresare "dos",
;               v EBX je inoda adresare "mnt" a DS:ESI ukazuje na "/bp",0
; Technicky dodatek:
;               Pokud je na pozici DS:ESI="0" pak jsou EAX a EBX
;               platne pro hledany soubor(adr.), pokud je DS:ESI="/"
;               tak pokud neni pro inodu EAX vytvorena VFS inoda, cesta
;               nebyla nalezena, pokud ale je, pak se zjisti, jestli
;               na danou VFS inodu je namontovane nejake zarizeni,
;               pokud je, pak se pouzije lookup pro FS, ktery je
;               na danem zarizeni, v opacnem pripade cesta nebyla
;               nalezena => je spatna
EXT2_Inode_lookup       Proc
       push   fs edx ebp es edi ecx ebx eax ds esi
        ; zjisti, jestli je v ceste obsazen koren
       push   ds
       pop    es
        mov   edi, esi
        ; ES:EDI - cesta
        cmp   byte ptr es:[edi], '/'
        jnz   EXT2_Inode_lookup_False
        ; nejdriv najdeme root daneho zarizeni a zjistime ukazatel na jeho
        ; polozku v mounttab
        call  VFS_Find_Mounted_Inode
        jc    EXT2_Inode_lookup_False
        ; DS:ESI - ukazatel na mounttab polozku
        mov   eax, 2
        xor   ebx, ebx
        cmp   byte ptr es:[edi+1], 0
        jz    EXT2_Inode_lookup_True_00
        jmp   EXT2_Inode_lookup_Loop_test_end

EXT2_Inode_lookup_Loop:
        inc   edi
        ; DS:ESI - ukazatel na mounttab polozku
        ; es:edi - ukazatel do path
        ; eax - inoda
        ; ebx - nadrazena inoda hledane inody

       ;zjistime info o nazvu hledaneho adresare(souboru)
       ; edi musi vzdy ukazovat na zacatek nazvu souboru, cili musi ukazovat za "/"
       push  edi eax
        mov  ecx, es
        lsl  ecx, ecx
        sub  ecx, edi
        xor  edx, edx
EXT2_Inode_lookup_Loop_100:
        cmp  byte ptr es:[edi], '/'
        jz   short EXT2_Inode_lookup_Loop_101
        cmp  byte ptr es:[edi], 0
        jz   short EXT2_Inode_lookup_Loop_101
        inc  edx
        inc  edi
        dec  ecx
        jnz  short EXT2_Inode_lookup_Loop_100
       pop   eax edi
        jmp  EXT2_Inode_lookup_False
EXT2_Inode_lookup_Loop_101:
        mov  ecx, edx
        mov  ebp, edi
       pop   eax edi
        ; ECX - delka nazvu hledaneho souboru;)
        ; EBP - nova hodnota edi v path
       push   eax
        ; otevreme VFS inodu
        mov   edx, 1
        call  VFS_Open_Main
       pushfd
        mov   edx, eax
       popfd
       pop    eax
        jc    EXT2_Inode_lookup_False
       push   edx ; VfS inoda

        ; zkontrolujeme jestli se jedna o adresar
       push   eax edx ds esi
        mov   eax, edx
        call  VFS_Check_Inode
        ; kontrolovat nemusime, protoze VFS inoda byla spravne otevrena
        movzx eax, word ptr ds:[esi+VFS_I_MODE]
        ; EAX - mod inody
        and   eax, EXT2_S_IFDIR
        cmp   eax, EXT2_S_IFDIR
       pop    esi ds edx eax
       pop    ebx
        jnz   EXT2_Inode_lookup_False
       push   ebx ; VfS inoda
       push   eax
        mov   eax, edx
       push   ecx
        xor   ebx, ebx
        mov   ecx, 0FFFFFFFFh
        call  VFS_Map_File
       pop    ecx
       pop    eax
       pop    ebx
        jc    EXT2_Inode_lookup_False
        mov   fs, edx
      push    ebx ; VfS inoda
       ; EDX - descriptor s namapovanym souborem
       ; FS - descriptor s namapovanym souborem
      push   ecx edi ds esi eax
       mov   edx, fs
       mov   ds, edx
       lsl   edx, edx
       sub   edx, d_Dir_name
       sub   edx, ecx
       xor   esi, esi
EXT2_Inode_lookup_Loop_201:
       cmp   eax, dword ptr ds:[esi+d_inode_num]
       jz    short EXT2_Inode_lookup_Loop_202
       movzx ebx, byte ptr ds:[esi+d_name_len]
       cmp   ecx, ebx
       jnz   short EXT2_Inode_lookup_Loop_202
      push   ecx edi esi
       add   esi, d_Dir_name
       cld
       rep   cmpsb
      pop    esi edi ecx
       jz    short EXT2_Inode_lookup_Loop_210
EXT2_Inode_lookup_Loop_202:
       movzx ebx, word ptr ds:[esi+d_rec_len]
       add   esi, ebx
       cmp   esi, edx
       jle   short EXT2_Inode_lookup_Loop_201
      pop    eax esi ds ecx edi
       ; odalokujeme soubor
       pop   edx ;vfs inoda
        ; zavreme otevrenou VFS inodu
       push  eax
        mov  eax, edx
        call VFS_Close
       pop   eax
        xor  ebx, ebx
        mov  edi, ebp
        jmp  EXT2_Inode_lookup_false
EXT2_Inode_lookup_Loop_210:
       ; nazev byl nalezen
       ; DS:ESI - ukazatel na nalezenou adresarovou polozku
       mov   edx, dword ptr ds:[esi+d_inode_num]
      pop    eax esi ds edi ecx
       mov   edi, ebp
       mov   ebx, eax
       mov   eax, edx
       pop    edx
       push   eax ebx
        mov   eax, edx
        ; zavreme otevrenou VFS inodu
        call  VFS_Close
       pop    ebx eax
EXT2_Inode_lookup_Loop_test_end:
        cmp   byte ptr es:[edi], 0
        jnz   EXT2_Inode_lookup_Loop
        jmp   short EXT2_Inode_lookup_True_00
; navrat true
EXT2_Inode_lookup_True:
        cmp   byte ptr es:[edi], '/'
        jnz   short EXT2_Inode_lookup_True_0
        mov   ecx, 1
        jmp   short EXT2_Inode_lookup_True_1
EXT2_Inode_lookup_True_0:
        cmp   byte ptr es:[edi], 0
        jnz   short EXT2_Inode_lookup_False
EXT2_Inode_lookup_True_00:
        mov   ecx, 0
EXT2_Inode_lookup_True_1:
       pop    esi ds edx edx edx ; esi ds eax ebx ecx
        mov   esi, edi
        jmp   short EXT2_Inode_lookup_End
EXT2_Inode_lookup_False:
       pop    esi ds eax ebx ecx
        mov   ecx, 2
EXT2_Inode_lookup_End:
       pop    edi es ebp edx fs
        retf
EXT2_Inode_lookup       endp

; vytvori v adresari zastupce inody
; Vstup:  EAX - cislo nove inody
;         EBX - cislo inody adresare, kam se ma vytvorit soubor
;         ES:EDI - buffer s nazvem souboru
;         DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni, na kterem chceme vytvori inodu
; Vystup: CF = 0 - OK
;         CF = 1 - Error
EXT2_Make_Inode_In_Directory Proc
       pushad
       push   ds es fs gs
        ; zjistime delku nazvu noveho souboru
       push   ecx edi edx
        mov   ecx, es
        lsl   ecx, ecx
        sub   ecx, edi
        xor   ebp, ebp
EXT2_Make_Inode_In_Directory_0:
        cmp   byte ptr es:[edi], 0
        jz    short EXT2_Make_Inode_In_Directory_1
        inc   ebp
        inc   edi
        dec   ecx
        jnz   short EXT2_Make_Inode_In_Directory_0
       pop    edx edi ecx
        jmp   EXT2_Make_Inode_In_Directory_false
EXT2_Make_Inode_In_Directory_1:
        ; EBP - delka nazvu
       pop    edx edi ecx
        or    ebp, ebp
        jz    EXT2_Make_Inode_In_Directory_false
        ; otevreme VFS_Inodu
        xchg  eax, ebx
        Call  VFS_Open_main
        jc    EXT2_Make_Inode_In_Directory_false
        ; zjistime, jestli se jedna o adresar
        Call  VFS_Is_Inode_Directory
        jc    EXT2_Make_Inode_In_Directory_false_close
        ; namapujeme adresar do pameti
       push   ebx eax
        xor   ebx, ebx
        mov   ecx, -1
        call  VFS_Map_File
       pushfd
        mov   ecx, edx
       popfd
       pop    eax edx
        jc    EXT2_Make_Inode_In_Directory_false_close
        mov   fs, ecx
        ; FS - namapovany soubor
        ; EAX - otevrena VFS inoda
        ; EDX - nova inoda
        ; EBP - delka nazvu souboru ulozeneho v ES:EDI
        ; ES:EDI - nazev souboru,0
        ; zjistime, jestli uz tam neni zastoupena polozka se stejnym nazvem
      push   ecx edi ds esi eax edx
       mov   edx, fs
       mov   ds, edx
       lsl   edx, edx
       sub   edx, d_Dir_name
       sub   edx, ebp
       xor   esi, esi
EXT2_Make_Inode_In_Directory_2:
       movzx ebx, byte ptr ds:[esi+d_name_len]
       cmp   ebp, ebx
       jnz   short EXT2_Make_Inode_In_Directory_3
      push   ecx edi esi
       mov   ecx, ebp
       add   esi, d_Dir_name
       cld
       rep   cmpsb
      pop    esi edi ecx
       jnz   short EXT2_Make_Inode_In_Directory_3
      pop    edx eax esi ds edi ecx
       jmp   EXT2_Make_Inode_In_Directory_false_close_dealloc
EXT2_Make_Inode_In_Directory_3:
       movzx ebx, word ptr ds:[esi+d_rec_len]
       add   esi, ebx
       cmp   esi, edx
       jle   short EXT2_Make_Inode_In_Directory_2
       sub   esi, ebx
       mov   ebx, esi
      pop    edx eax esi ds edi ecx
       ; FS:EBX - ukazatel na posledni pouzivanou polozku v adresari
       ; EAX - otevrena VFS inoda
       ; EDX - nova inoda
       ; EBP - delka nazvu souboru ulozeneho v ES:EDI
       ; ES:EDI - nazev souboru,0
       push   eax ecx
        movzx eax, byte ptr fs:[ebx+d_name_len]
        add   eax, 8
        shr   eax, 2
        inc   eax
        shl   eax, 2
        movzx ecx, word ptr fs:[ebx+d_rec_len]
        sub   ecx, eax
        ; ECX - delka nove alokovane polozky

        ; zjistime, jestli je aktualni velikost inody dostatecna pro zapsani
        ; teto nove polozky
       push   ebp
        add   ebp, 8
        cmp   ecx, ebp
       pop    ebp
        jge   EXT2_Make_Inode_In_Directory_4
        ; alokujeme pro inodu adresare dalsi blok
        ; soubor se musi premapovat
EXT2_Make_Inode_In_Directory_4:
        mov   word ptr fs:[ebx+d_rec_len], ax
        add   ebx, eax
        ; ECX - delka nove alokovane polozky
        ; FS:EBX - ukazuje na nove alokovanou polozku
        mov   word ptr fs:[ebx+d_rec_len], cx
        mov   dword ptr fs:[ebx+d_inode_num], edx
       push   edx
        mov   edx, ebp
        mov   byte ptr fs:[ebx+d_name_len], dl
       pop    eax
       push   ebx edx
        xor   ebx, ebx
        xor   edx, edx
        call  VFS_Open_main
       pop    edx ebx
        jnc   short EXT2_Make_Inode_In_Directory_10
       pop    ecx eax
        jmp   EXT2_Make_Inode_In_Directory_false_close_dealloc
EXT2_Make_Inode_In_Directory_10:
       push   ds esi eax
        call  VFS_Check_Inode
        mov   ecx, dword ptr ds:[esi+VFS_I_MODE]
       pop    eax esi ds
        call  VFS_Close
        mov   eax, ecx
        xor   ecx, ecx
        mov   cl, EXT2_FT_REG_FILE
       push   eax
        and   eax,EXT2_S_IFFIL
        cmp   eax,EXT2_S_IFFIL
       pop    eax
        jz    EXT2_Make_Inode_In_Directory_20
        mov   cl, EXT2_FT_DIR
       push   eax
        and   eax,EXT2_S_IFDIR
        cmp   eax,EXT2_S_IFDIR
       pop    eax
        jz    EXT2_Make_Inode_In_Directory_20
        mov   cl, EXT2_FT_CHRDEV
       push   eax
        and   eax,EXT2_S_IFCHR
        cmp   eax,EXT2_S_IFCHR
       pop    eax
        jz    EXT2_Make_Inode_In_Directory_20
        mov   cl, EXT2_FT_BLKDEV
       push   eax
        and   eax,EXT2_S_IFBLK
        cmp   eax,EXT2_S_IFBLK
       pop    eax
        jz    EXT2_Make_Inode_In_Directory_20
        mov   cl, EXT2_FT_SYMLINK
       push   eax
        and   eax,EXT2_S_IFLNK
        cmp   eax,EXT2_S_IFLNK
       pop    eax
        jz    EXT2_Make_Inode_In_Directory_20
        mov   cl, EXT2_FT_FIFO
       push   eax
        and   eax,EXT2_S_IFIFO
        cmp   eax,EXT2_S_IFIFO
       pop    eax
        jz    EXT2_Make_Inode_In_Directory_20
        mov   cl, EXT2_FT_SOCK
       push   eax
        and   eax,EXT2_S_IFSOCK
        cmp   eax,EXT2_S_IFSOCK
       pop    eax
EXT2_Make_Inode_In_Directory_20:
        mov   byte ptr fs:[ebx+d_file_type], cl
        add   ebx, d_Dir_name
EXT2_Make_Inode_In_Directory_5:
        mov   al, byte ptr es:[edi]
        mov   byte ptr fs:[ebx], al
        inc   ebx
        inc   edi
        dec   ebp
        jnz   EXT2_Make_Inode_In_Directory_5
       pop    ecx eax
       push   eax
        Call  VFS_Sync
       pop    eax
        Call  VFS_Close
EXT2_Make_Inode_In_Directory_true:
        clc   ;CF=0
        jmp   short EXT2_Make_Inode_In_Directory_end
EXT2_Make_Inode_In_Directory_false_close_dealloc:
EXT2_Make_Inode_In_Directory_false_close:
        Call  VFS_Close
        stc   ;CF=1
        jmp   short EXT2_Make_Inode_In_Directory_end
EXT2_Make_Inode_In_Directory_false:
        stc   ;CF=1
EXT2_Make_Inode_In_Directory_end:
       pop    gs fs es ds
       popad
        ret
EXT2_Make_Inode_In_Directory Endp

; Vstup:  DS:ESI - ukazatele na mount_tab polozku, ktera oznacuje
;                  namontovane zarizeni, na kterem chceme vytvori inodu
;         EAX    - typove cislo
;         EBX    - cislo inody adresare, do ktereho vytvarime soubor
;         ECX    - mode nove vytvarene inody (EXT2_S_IFBLK,EXT2_S_IFCHR,EXT2_S_IFIFO,EXT2_S_IFSOCK)
;         ES:EDI - ukazatel na nazev nove vytvareneho souboru, zakonceny na 0
; Vystup: CF = 0 - OK
;                - EAX = cislo nove inody
;         CF = 1 - Error
EXT2_Inode_mknod        Proc ;  vytvori specialni soubor (socket,fifo,device)
       push   ebx ecx edx ebp edi esi ds fs eax gs es
        ; alokujeme inodu
       push   eax ebx ecx
        lfs   ebx, fword ptr ds:[esi+VFS_MS_FS_Infos]
        lfs   ebx, fword ptr fs:[ebx+VFS_FS_Inode_Ops]
        call  fword ptr fs:[ebx+FS_Inode_Ops_create]
       pop    ecx ebx edx
        jc    EXT2_Inode_mknod_false
        ; EAX - cislo nove inody
        ; ebx - cislo inody adresare
        ; ecx - mode nove inody
        ; edx - typove cislo noveho zarizeni

       push   eax
        mov   eax, 128
        call  alloc
       pushfd
        mov   ebp, eax
       popfd
       pop    eax
        jc    EXT2_Inode_mknod_false_delete
       push   es edi
        mov   es, ebp
        mov   gs, ebp
        ; es  - temp buffer
        ; EAX - cislo inody
        ; ebx - cislo inody adresare
        ; ecx - mode nove inody
        ; edx - typove cislo noveho zarizeni

        mov   dword ptr ES:[i_block], edx
        mov   word ptr ES:[i_mode], cx
        ; pozdeji pridat podle vlastnika procesu, ktery vola tuto fci
        mov   word ptr ES:[i_uid], 0
        mov   word ptr ES:[i_gid], 0
        mov   word ptr ES:[i_links_count], 1
        mov   dword ptr es:[i_version], 0E2F1B088h

        xor   edi, edi
        ; uloz inodu
       push   ebx eax
        xor   ebx, ebx
        lfs   edx, fword ptr ds:[esi+VFS_MS_FS_Infos]
        lfs   edx, fword ptr fs:[edx+VFS_FS_File_Ops]
        call  fword ptr fs:[edx+FS_File_ops_io_cmd]
       pop    eax ebx
       pop    edi es
        jc    EXT2_Inode_mknod_false_delete_dealloc

        ; vytvorime polozku v adresari
       push   eax
        call  EXT2_Make_Inode_In_Directory
       pop    ebx
        jc    EXT2_Inode_mknod_false_delete_dealloc

EXT2_Inode_mknod_true:
        mov   eax, gs
       pop    es gs
        call  DeAlloc
       pop    ecx
        mov   eax, ebx
        clc   ;CF=1
        jmp   short EXT2_Inode_mknod_end
EXT2_Inode_mknod_false_delete_dealloc:
        mov   eax, gs
       pop    es gs
        call  DeAlloc
       pop    eax
        jmp   short EXT2_Inode_mknod_false_delete_0
EXT2_Inode_mknod_false_delete:
       pop    es gs eax
EXT2_Inode_mknod_false_delete_0:
        ; call ext2_delete_inode
        stc   ;CF=1
        jmp   short EXT2_Inode_mknod_end
EXT2_Inode_mknod_false:
       pop    es gs eax
        stc   ;CF=1
EXT2_Inode_mknod_end:
       pop    fs ds esi edi ebp edx ecx ebx
        retf
EXT2_Inode_mknod        endp

EXT2_Inode_mkdir        Proc ;  vytvori adresar
        retf
EXT2_Inode_mkdir        endp

EXT2_Inode_rmdir        Proc ;  smaze adresar
        retf
EXT2_Inode_rmdir        endp

EXT2_Inode_permission   Proc ;  nastavi prava a attributy inody
        retf
EXT2_Inode_permission   endp

EXT2_Inode_link         Proc ;  vytvori link na inodu
        retf
EXT2_Inode_link         endp

EXT2_Inode_unlink       Proc ;  z linku udela zase inodu, jaka byla pred tim, nez se z ni udelal link
        retf
EXT2_Inode_unlink       endp

EXT2_Inode_symlink      Proc ;  vytvori z inody symbolicky odkaz
        retf
EXT2_Inode_symlink      endp

EXT2_Inode_readlink     Proc ;  navrati inodu na kterou ukazuje hardlink(link)
        retf
EXT2_Inode_readlink     endp

EXT2_Inode_follow_link  Proc ;  navrati inodu, na kterou ukazuje symlink
        retf
EXT2_Inode_follow_link  endp

EXT2_Inode_delete  Proc ; odalokuje inodu a jeji datove bloky
        retf
EXT2_Inode_delete  endp

