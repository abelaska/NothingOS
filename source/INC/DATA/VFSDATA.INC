;############################################################################
;# Datove structury a promenne VFS
;############################################################################

; Ukazatel do dalekeho ciziho selectoru
Far_Pointer Struc
        FP_Offset       dd 0 ; offset
        FP_Selector     dw 0 ; selector
Far_Pointer Ends

; Struktura obsahujici informace o datumu a case
Date_Time Struc
        DT_Date         dd 0 ; komprimovany Datum
        DT_Time         dd 0 ; komprimovany Cas
Date_Time Ends

; Polozka, obsahujici informace o FS
; Size=32B

Max_VFS_FS_Name_Length          equ 10

VFS_FS_Infos Struc
        VFS_FS_Infos_Start      equ $
        VFS_FS_Name             db Max_VFS_FS_Name_Length dup(' ') ; jmeno zastupujici souborovy system
        ; pokud je prvni DD of VFS_FS_name 0 pak je tato polozka volna
        VFS_FS_Flags            dd 0           ; flagy FS (zatim neurceno)
        VFS_FS_Operations_Addr  Far_Pointer <> ; ukazatel na tabulku obsahujici adresy na funkce FS
        VFS_FS_Super_Ops        Far_Pointer <> ; ukazatel na tabulku funkci pro praci se super blokem
        VFS_FS_Inode_Ops        Far_Pointer <> ; ukazatel na tabulku fukci pro praci s inodama
        VFS_FS_File_Ops         Far_Pointer <> ; ukazatel na tabulku pro praci se souborama
        VFS_FS_Count_of_Use     dd 0
        VFS_FS_Infos_End        equ $
VFS_FS_Infos Ends

; Polozka, obsahujici informace o pripojenem zarizeni
; Size=32B
VFS_Mount_Struc Struc
        VFS_Mount_Struc_Start   equ $
        VFS_MS_VFS_Inode        dd 0 ; inoda adresare, na ktery bylo zarizeni namontovano
        VFS_MS_Device           dd 0 ; hi(cislo zarizeni)+lo(typove cislo driveru)
        VFS_MS_Attributes       dw 0 ; attributy primontovaneho zarizeni
        VFS_MS_Mount_Date_Time  Date_Time <>   ; datum a cas primontovani
        VFS_MS_FS_Infos         Far_Pointer <> ; ukazatel na informacni polozku FS
        VFS_MS_Count_Use        dd 0 ; kolikrat je toto namontovane zarizeni pouzivano (otevrenych inod,...)
        VFS_MS_Cache_Area       dw 0 ; selector na pamet(v gdt), ktera slouzi driveu FS jako cachovaci
        VFS_Mount_Struc_End     equ $
VFS_Mount_Struc Ends

; pri pouziti fce open, se navraci selektor na pamet, ktera ma takovouto strukturu
; Size=72+128B
; alokace volne polozky VFS_inode probyha tak, ze se nejdriv snazime nalezt polozku, ve ktere jsou
; promenne VFS_I_Inode a VFS_I_Use_Count nulove, pokud se takova nenalezne, hleda se jen podle nulove
; hodnoty promenne VFS_I_Use_Count
VFS_Inode Struc
        VFS_Inode_Start          equ $
        VFS_I_Inode              dd 0 ; cislo inody v namontovanem FS
                                      ; pokud je 0 pak je polozka volna
        VFS_I_Inode_Blocks       dd 16 dup(0) ; misto pro ulozeni dat inody, kdyz se inoda inicializuje nacte se z disku info o inode
        VFS_I_Parent_Inode       dd 0 ; cislo nadrazene inody(adresare, ve ktere se tato inoda nachazi)
        VFS_I_Mount_Struc        Far_Pointer <> ; ukazatel na "rodicovskou" polozku mount_Struc, pod kterou tato inoda patri
        VFS_I_Open_Date_Time     Date_Time <> ; datum a cas otevreni inody
        VFS_I_Modify_Date_Time   Date_Time <> ; datum a cas posledni modifikace inody (read,write,poll,..)
        VFS_I_UID                dw 0 ; ID vlastnika souboru
        VFS_I_GID                dw 0 ; ID groupy vlastniku
        VFS_I_MODE               dd 0 ; mod inody (soubor,adr.,zarizeni,...) standart jako ma ext2
        VFS_I_Mapped_Sel         dw 0 ; jestli se namapujou data inody do pameti
                                      ; pak se sem ulozi selector na descriptor, do ktereho
                                      ; byla namapovana
        VFS_I_Use_Count          dd 0 ; kolik rukojeti je na tuto VFS inodu vytvoreno
                                      ; pokud se hledaji volne polozky VFS_inode, tato promenna je klicova
        VFS_I_Write_Count        dd 0 ; kolikrat byla na inodu pouzita fce write
        VFS_I_Open_Mode          dd 0 ; attributy oznacujici zpusob prvniho otevreni souboru
        VFS_I_File_Size          dd 0 ; velikost souboru
        VFS_I_Block_Size         dd 0 ; velikost bloku na kterych je soubor ulozen
        VFS_I_Number_Blocks      dd 0 ; pocet potrebnych bloku pro ulozeni celeho souboru
        VFS_I_Number_Pages       dd 0 ; kolik stranek zabira soubor v pameti
                                      ; tahle polozka se zvysuje, kdyz se alokuje dalsi fyz. page pro dany soubor
        VFS_I_File_Lock_Atr      dd 0 ; attributy oznacujici typ uzamceni souboru
        VFS_Inode_End            equ $
VFS_Inode Ends

; Rukojek k otevrenem souboru
File_Handle Struc
        File_Handle_Start     equ $
        FH_VFS_Inode          Far_Pointer <> ; ukazatel na polozku VFS inody,
                                             ; na kterou ukazuje tato rukojet
                                             ; pokud je adresa nil, polozka je volna
        FH_Open_Mode          dd 0 ; attributy oznacujici zpusob otevreni souboru
                                   ; hodnota je totozna s VFS inodou, pro kterou
                                   ; byla otevrena tato rukojet
        FH_Position           dd 0 ; ukazatel na aktualni pozici v souboru
        FH_Flags              dd 0 ;
        File_Handle_End       equ $
File_Handle Ends

; velikosti struktur
VFS_FS_Infos_Size       equ VFS_FS_Infos_End    - VFS_FS_Infos_Start     ;32B
VFS_Mount_Struc_Size    equ VFS_Mount_Struc_End - VFS_Mount_Struc_Start  ;32B
VFS_Inode_Size          equ VFS_Inode_End       - VFS_Inode_Start        ;72B
File_Handle_Size        equ File_Handle_End     - File_Handle_Start

; do data segmentu kernelu
Sel_VFS_Temp_Buffer     dw    0    ; tempovaci buffer, pro praci s blokovyma zarizenima

Max_VFS_Mount_Struc     equ   512
Sel_VFS_Mount_Struc     dw    0    ; selector pro ulozeni mount_strucs
Count_VFS_Mount_Struc   dd    0    ; kolik jiz bylo namontovano zarizeni

Max_VFS_Inode           equ   16384
Sel_VFS_Inode           dw    0    ; selector pro ulozeni struktur VFS_inod
Count_VFS_inode         dd    0    ; pocitadlo otevrenych VFS inod

Max_VFS_FS_Infos        equ   128
Sel_VFS_FS_Infos        dw    0    ; selector pro ulozeni struktur FS_Infos
Count_VFS_FS_Infos      dd    0    ; kolik jiz bylo registrovano ovladacu FS

; rukojeti se ukladaji do descriptoru rukojeti, ktery je v LDT daneho tasku
Max_File_Handle         equ   1024

; VFS_I_Open_Mode
; 31                             0
; 0000000000000000000000000000ASWR
; R - 0.bit = 0 - cteni zakazano
;           = 1 - cteni povoleno
; W - 1.bit = 0 - zapis zakazan
;           = 1 - zapis povolen
;           - pokud je 0. a 1. bit v nule je inoda blokovana pro jakakoliv
;             prace s ni
; S - 2.bit = 0 - inoda je neni otevrena pro sdileni
;           = 1 - inoda je otevrena pro sdileni
; A - 3.bit = 0 - zapis neni appendovany
;           = 1 - inoda je otevrena pro zpusob zapisu append
;           - jakekoliv data zapisovane do teto VFS inody se pripojuji na konec
;             => kdyz naseekujete napr. na zacatek souboru a pokusite
;             se neco zapsat, tak se to zapise od konce souboru => append write
;           - urceno napr. pro logovaci soubory

; VFS_MS_Attributes
; 31                             0
; 00000000000000000000000000000SWR
; R - 0.bit = 0 - cteni zakazano
;           = 1 - cteni povoleno
; W - 1.bit = 0 - zapis zakazan
;           = 1 - zapis povolen
;           pokud je 0. a 1. bit v nule je zarizeni blokovane pro jakoukoliv
;           praci s nim
; S - 2.bit = 0 - zarizeni neni otevrene pro sdileni
;           = 1 - zarizeni je otevrene pro sdileni

; attributy urcene pro montovani zarizeni
VFS_Use_Mode_R          equ 01b
VFS_Use_Mode_W          equ 10b
VFS_Use_Mode_RW         equ 11b

; lock attributy
VFS_File_Lock_On        equ 1 ; Pristup k zarizeni je zakazan
VFS_File_Lock_Off       equ 0 ; Pristup k zarizeni je povolen

; oznaceni typoveho cisla driveru pro primarni master disk
VFS_Primary_Master_Disk equ 01000000dh
VFS_Partition_Num_1     equ 000010000h

; DULEZITE PRO NAMONTOVANI ROOTU
; root
Main_root_directory  db '/',0
; ext2 jmeno fs
FS_EXT2_NAME         db 'EXT2',0

