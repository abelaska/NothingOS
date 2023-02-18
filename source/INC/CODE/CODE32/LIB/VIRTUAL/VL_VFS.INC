;#############################################################################
;# Virtualni jednotka VFS
;#############################################################################

;#############################################################################
;# Fce      - name
;# 00000000 - VFS_Write
;# 00000001 - VFS_Read
;# 00000002 - VFS_Open
;# 00000003 - VFS_Close
;# 00000004 - VFS_Map_File
;# 00000005 - VFS_Sync
;# 00000006 - VFS_MkNod
;# 00000007 - VFS_Trace_Path
;# 00000008 - VFS_Dir
;# 00000009 - VFS_Mount_Device
;# 0000000A - VFS_UMount_Device
;# 0000000B - VFS_Mount_Device_Core
;# 0000000C - VFS_Find_Mounted_Inode
;# 0000000D - VFS_Get_Inode_Mount_device
;# 0000000E - VFS_Check_Open_Mode
;# 0000000F - VFS_Check_Inode
;# 00000010 - VFS_Check_Inode_In_VFS_Inode
;# 00000011 - VFS_Is_Inode_File
;# 00000012 - VFS_Is_Inode_Directory
;# 00000013 - VFS_Is_Inode_BLK_Device
;# 00000014 - VFS_Get_Inode_mode
;#############################################################################

VL_VFS :
;################ Emulovana structura souboru knihovny
DB   'LIB',0                 ; Signatura
DW   0001h                   ; Version
DB   'VFS             '      ; Library Name
DW   0000h                   ; Unit Number  - not used in Virtual +18
DD   00000000h               ; CodeSeg size - not used in Virtual +20
Jmp VL_VFS_EnterPoint_3  ;+???
;##################################################################################
;# Vstupni bod knihovny
;##################################################################################
VL_VFS_EnterPoint_3 :
          Cmp    EAX,VL_VFS_Num_Of_Fce
          Ja     Short @VL_VFS_End
        PUSH     DS
          PUSH   DATA_Descriptor
          Pop    DS
          Mov    EAX,Dword ptr DS:[VL_VFS_Fce+EAX*4]
        POP      DS
         PUSH    EAX
         Call    Dword ptr SS:[ESP]
         Jc      @VL_VFS_End_C
         Add     ESP,4
         CLC
         Jmp     @VL_VFS_End
   @VL_VFS_End_C :
         ADD     ESP,4
         STC
   @VL_VFS_End :
        RetF

;##################################################################################
;# procedura pro standardni zapis do VFS inody
;##################################################################################
;# Vstup  : EDX    - VFS inoda
;#          EBX    - pozice v zarizeni, odkud se ma zacit zapisovat
;#          ECX    - kolik bajtu dat se ma zapsat
;#          ES:EDI - vstupni buffer
;# Vystup : CF = 0 - OK
;#          CF = 1 - Error
;##################################################################################
VFS_Write_vl PROC
          Mov   EAX,EDX
          Call  VFS_Write
        Ret
VFS_Write_vl ENDP
;##################################################################################
;# procedura pro standardni cteni do VFS inody
;##################################################################################
;# Vstup  : EDX    - VFS inoda
;#          EBX    - pozice v zarizeni, odkud se ma zacit precist
;#          ECX    - kolik bajtu dat se ma precist
;#          ES:EDI - vystupni buffer
;# Vystup : CF = 0 - OK
;#          CF = 1 - Error
;#          ECX - pocet prectenych znaku
;##################################################################################
VFS_Read_vl PROC
          Mov   EAX,EDX
          Call  VFS_Read
        Ret
VFS_Read_vl ENDP

; Zjisti, jestli je na dane inode (na rootu) namontovano nejake zarizeni
; Vstup:  EDX - VFS inoda
; Vystup: CF  = 0 - na tuto inodu je namontovano zarizeni
;             = DS:ESI - adresa ukazujici na zacatek dane polozky v mount tabulce
;         CF  = 1 - inoda v tabulce mount nebyla nalezena
;         EAX - nezmeneno
VFS_Find_Mounted_Inode_vl Proc
        mov eax, edx
        call VFS_Find_Mounted_Inode
        ret
VFS_Find_Mounted_Inode_vl Endp

; namontuje na inodu zarizeni, funguje az po namontovani korene
; Vstup:  DS:ESI - cesta k adresari, na ktery se bude montovat (napr. '/mnt/floppy',0)
;         ES:EDI - nazev souboroveho systemu, max delka je 10 znaku, podporuje se
;                  ukonceni na 0(napr. pri delce nazvu 9 znaku a nize) - (napr. 'EXT2',0)
;         FS:EBX - cesta k zarizeni (napr. '/dev/fd0',0)
;         EDX    - attributy pro montovani (napr. 1 -> RW pristup)
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
VFS_Mount_Device_vl Proc
         mov   eax, edx
         call  VFS_Mount_Device
         ret
VFS_Mount_Device_vl Endp

; Odmontuje zarizeni
; Vstup:  DS:ESI - cesta k adresari, na ktery je neco namontovane
; Vystup: CF = 0 - OK
;         CF = 1 - ERROR
;            = EAX - 0 - byla zadana spatna cesta
;                  - 1 - na tomto adresari neni nic namontovane
VFS_UMount_Device_vl Proc
        call  VFS_UMount_Device
        ret
VFS_UMount_Device_vl Endp

; prevede cestu na koncovou inodu
; Vstup:  DS:ESI - cesta k souboru,adresari,... zakonceny na 0
; Vystup: CF=0 - OK
;              - EAX = cislo inody
;              - EBX = cislo nadrazene inody
;              - DS:ESI = ukazatel na mount struc polozku
;         CF=1 - Error
VFS_Trace_Path_vl Proc
        call  VFS_Trace_Path
        ret
VFS_Trace_Path_vl endp

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
VFS_Dir_vl Proc
        mov   eax, edx
        call  VFS_Dir
        ret
VFS_Dir_vl endp

; otevre soubor a vrati rukojet
; Vstup:  EDX    - typ otevreni
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
VFS_Open_vl Proc
pushad
xor edi, edi
mov ecx, 16
call write_pchar
popad
        call  VFS_Open
wr_reg_nowait eax, 20*160
call waitkey
        ret
VFS_Open_vl Endp

; zavre VFS inodu
; Vstup:  EDX - cislo VFS inody
; Vystup: CF = 0 - OK
;         CF = 1 - Error
VFS_Close_vl Proc
        mov   eax, edx
        call  VFS_Close
        ret
VFS_Close_vl endp

; zjisti, jestli VFS inoda existuje
; Vstup : EDX - VFS inoda
; Vystup: CF = 0 - OK
;            = DS:ESI - ukazatel na polozku VFS inode
;         CF = 1 - Error
VFS_Check_Inode_vl Proc
        mov   eax, edx
        call  VFS_Check_Inode
        ret
VFS_Check_Inode_vl endp

; Vstup:  EDX - VFS inoda
;         EBX - pocatecni 4k blok souboru (od 0)
;         ECX - posledni 4k blok souboru
;             - pokud EBX=0FFFFFFFFh pak se namapuje az do konce souboru
; Vystup: CF - 0 = OK
;            - EDX - selector na vystupni descriptor
;            - EAX - velikost souboru v bajtech
;         CF - 1 = Error
VFS_Map_File_vl Proc
        mov   eax, edx
        call  VFS_Map_File
        ret
VFS_Map_File_vl endp

; zjisti, jestli je VFS inoda otevrena pro soubor
; Vstup:  EDX - VFS inoda
; Vystup: CF = 0 - True
;         CF = 1 - False
VFS_Is_Inode_File_vl Proc
        mov   eax, edx
        call  VFS_Is_Inode_File
        ret
VFS_Is_Inode_File_vl endp

; zjisti, jestli je VFS inoda otevrena pro adresar
; Vstup:  EDX - VFS inoda
; Vystup: CF = 0 - True
;         CF = 1 - False
VFS_Is_Inode_Directory_vl Proc
        mov   eax, edx
        call  VFS_Is_Inode_Directory
        ret
VFS_Is_Inode_Directory_vl endp

; zjisti, jestli je VFS inoda otevrena pro adresar
; Vstup:  EDX - VFS inoda
; Vystup: CF = 0 - True
;         CF = 1 - False
VFS_Is_Inode_BLK_Device_vl Proc
        mov   eax, edx
        call  VFS_Is_Inode_BLK_Device
        ret
VFS_Is_Inode_BLK_Device_vl endp

; zjisti, jestli uz je otevrena VFS inoda pro inodu na danem zarizeni
; Vstup : EDX - Inoda na zarizeni
;         DS:ESI - ukazatel na polozku mounttab
; Vystup: CF = 0 - OK
;            = EAX - cislo VFS inody
;         CF = 1 - Error
VFS_Check_Inode_In_VFS_Inode_vl Proc
        mov   eax, edx
        call  VFS_Check_Inode_In_VFS_Inode
        ret
VFS_Check_Inode_In_VFS_Inode_vl endp

; zjistime, jestli uz inoda neni otevrena pro zapis
; Vstup:  EDX - inoda na rootu
;         EBX - attributy otevreni, ktere hledame (maska attributupro ANDovani)
;         DS:ESI - ukazatel na mount tab polozku zarizeni, na kterem je inoda
; Vystup: CF - 0 = inoda neni nikde otevrena pro zapis
;         CF - 1 = inoda je jiz otevrena pro zapis
VFS_Check_Open_Mode_vl Proc
        mov   eax, edx
        mov   edx, ebx
        call  VFS_Check_Open_Mode
        ret
VFS_Check_Open_Mode_vl endp

; namontuje na inodu zarizeni (pro pouziti v hlavni procedure Mount_Device)
; Vstup: EDX - VFS Inoda, na kterou se ma zarizeni namontovat
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
VFS_Mount_Device_Core_vl Proc
        mov   eax, edx
        call  VFS_Mount_Device_Core
        ret
VFS_Mount_Device_Core_vl endp

; refreshne data namapovaneho souboru na disk
; Vstup:  EDX - VFS inoda
; Vystup: CF = 0 - OK
;         CF = 1 - Error
VFS_Sync_vl Proc
        mov   eax, edx
        call  VFS_Sync
        ret
VFS_Sync_vl endp

; Vstup:  DS:ESI - cesta k adresari, ve kterem chceme vytvorit zarizeni
;         ES:EDI - jmeno nove vytvareneho souboru
;         EDX    - typove cislo
;         ECX    - mode nove vytvarene inody (EXT2_S_IFBLK,EXT2_S_IFCHR,EXT2_S_IFIFO,EXT2_S_IFSOCK)
; Vystup: CF = 0 - OK
;                - EAX = cislo nove inody
;         CF = 1 - Error
VFS_MkNod_vl proc
       push   es edi ds esi edx ecx
       push   edx ecx
        Call  VFS_Trace_Path
       pop    ecx edx
        jc    VFS_MkNod_vl_false
        mov   ebx, eax
        mov   eax, edx
        call  VFS_MkNod
        jc    short VFS_MkNod_vl_false
VFS_MkNod_vl_true:
        clc   ;CF=0
        jmp   short VFS_MkNod_vl_end
VFS_MkNod_vl_false:
        stc   ;CF=1
VFS_MkNod_vl_end:
       pop    ecx edx esi ds edi es
        ret
VFS_MkNod_vl endp

; zjisti na jakem zarizeni je namontovana VFS inoda
; Vstup:  EDX - VFS inoda
; Vystup: CF = 0 - OK
;            - DS:ESI = ukazatel na mount struc polozku
;         CF = 1 - Error
VFS_Get_Inode_Mount_device_vl Proc
        mov   eax, edx
        call  VFS_Get_Inode_Mount_device
        ret
VFS_Get_Inode_Mount_device_vl endp

; navrati mode  VFS inody
; Vstup:  EDX - cislo VFS inody
; Vystup: CF = 0 - OK
;            - EAX = mode inody, pro kterou byla otevrena VFS inoda
;         CF = 1 - Error
VFS_Get_Inode_mode_vl Proc
        mov   eax, edx
        call  VFS_Get_Inode_mode
        ret
VFS_Get_Inode_mode_vl endp
