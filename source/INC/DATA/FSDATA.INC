;############################################################################
;# Datove structury a promenne pro praci s file systemy
;############################################################################

; Struktura ulozeni adres funkci v driveru
Driver_FS_Operations_Struc Struc
        ; tu jsou ulozeny offsetove pozice procedur v code segmentu driveru
        Driver_FS_Operations_Struc_Start equ $
        Super_Read_Super         dd 0 ; nacte superblock
        Super_Write_Super        dd 0 ; zapise superblock na disk
        Super_FSstat             dd 0 ; vrati informace o FS
        Super_ReMount            dd 0 ; remountuje zarizeni
        Super_Lock               dd 0 ; uzamce zarizeni
        Super_UnLock             dd 0 ; odemkne zarizeni
        File_read                dd 0 ; cteni ze souboru
        File_write               dd 0 ; zapis do souboru
        File_seek                dd 0 ; posouvani ukazatele v souboru
        File_Get_Map             dd 0 ; navrati mapu adresovacich bloku na datove struktury souboru
        File_read_dir            dd 0 ; navraci buffer, ve kterem je zachycen obraz obsahu adresare
        File_open                dd 0 ; otevre soubor a naplni promenne ve strukture VFS inoda, ktere pro tento soubor byla otevrena
        File_release             dd 0 ; tato fce se zavola, kdyz se uzavre posledni rukojet na danou VFS inodu
        File_poll                dd 0 ; zjisti, jestli neni soubor uzamcen
        File_truncate            dd 0 ; usekne soubor od aktualni pozice ukazetele v souboru
        File_io_cmd              dd 0 ; vstupne vystupni funkce pro manipulaci s inodou (napr. u zarizeni meni jeho typove cislo, atd.)
        File_fsync               dd 0 ; zapise data souboru z pameti na disk
        File_lock                dd 0 ; uzamkne VFS inodu, na kterou ukazuje rukojet
        Inode_create             dd 0 ; vytvori inodu
        Inode_Load               dd 0 ; nacte informacni strukturu inody
        Inode_rename             dd 0 ; prejmenuje inodu
        Inode_lookup             dd 0 ; podle cesty najde inodu a vrati jeji cislo
        Inode_mknod              dd 0 ; vytvori specialni soubor (socket,fifo,...)
        Inode_mkdir              dd 0 ; vytvori adresar
        Inode_rmdir              dd 0 ; smaze adresar
        Inode_permission         dd 0 ; nastavi prava a attributy inody
        Inode_link               dd 0 ; vytvori link na inodu
        Inode_unlink             dd 0 ; z linku udela zase inodu, jaka byla pred tim, nez se z ni udelal link
        Inode_symlink            dd 0 ; vytvori z inody symbolicky odkaz
        Inode_readlink           dd 0 ; navrati inodu na kretou ukazuje hardlink(link)
        Inode_follow_link        dd 0 ; navrati inodu, na kterou ukazuje symlink
        Driver_FS_Operations_Struc_End equ $
Driver_FS_Operations_Struc Ends

Driver_FS_Operations_Struc_Size  equ Driver_FS_Operations_Struc_End - Driver_FS_Operations_Struc_Start
Driver_FS_Operations_Count       equ (Driver_FS_Operations_Struc_Size/4)

; Struktura operaci se super blokem
FS_Super_Operations Struc
        FS_Super_Ops_Read_Super         Far_Pointer <> ; nacte superblock
        FS_Super_Ops_Write_Super        Far_Pointer <> ; zapise superblock na disk
        FS_Super_Ops_FSstat             Far_Pointer <> ; vrati informace o FS
        FS_Super_Ops_ReMount            Far_Pointer <> ; znovunamontuje zarizeni
        FS_Super_Ops_Lock               Far_Pointer <> ; uzamce zarizeni
        FS_Super_Ops_UnLock             Far_Pointer <> ; odemkne zarizeni
FS_Super_Operations Ends

; Struktura operaci se souborem
FS_File_Operations Struc
        FS_File_ops_read                Far_Pointer <> ; cteni ze souboru
        FS_File_ops_write               Far_Pointer <> ; zapis do souboru
        FS_File_ops_seek                Far_Pointer <> ; posouvani ukazatele v souboru
        FS_File_Get_Map                 Far_Pointer <> ; navrati mapu adresovacich bloku na datove struktury souboru
        FS_File_ops_read_dir            Far_Pointer <> ; navraci buffer, ve kterem je zachycen obraz obsahu adresare
        FS_File_ops_open                Far_Pointer <> ; otevre soubor v tomto FS
        FS_File_ops_release             Far_Pointer <> ; tato fce se zavola, kdyz se uzavre posledni rukojet na danou VFS inodu
        FS_File_ops_poll                Far_Pointer <> ; zjisti, jestli neni soubor uzamcen
        FS_File_ops_truncate            Far_Pointer <> ; usekne soubor od aktualni pozice ukazetele v souboru
        FS_File_ops_io_cmd              Far_Pointer <> ; vstupne vystupni funkce pro manipulaci s inodou (napr. u zarizeni meni jeho typove cislo, atd.)
        FS_File_ops_fsync               Far_Pointer <> ; zapise data souboru z pameti na disk
        FS_File_ops_lock                Far_Pointer <> ; uzamkne VFS inodu, na kterou ukazuje rukojet
FS_File_Operations Ends

; Struktura operaci s inodou
FS_Inode_Operations Struc
        FS_Inode_Ops_create             Far_Pointer <> ; vytvori inodu
        FS_Inode_Ops_Load               Far_Pointer <> ; precte info struc. inody
        FS_Inode_Ops_rename             Far_Pointer <> ; prejmenuje inodu
        FS_Inode_Ops_lookup             Far_Pointer <> ; podle cesty najde inodu a vrati jeji cislo
        FS_Inode_Ops_mknod              Far_Pointer <> ; vytvori specialni soubor (socket,fifo,...)
        FS_Inode_Ops_mkdir              Far_Pointer <> ; vytvori adresar
        FS_Inode_Ops_rmdir              Far_Pointer <> ; smaze adresar
        FS_Inode_Ops_permission         Far_Pointer <> ; nastavi prava a attributy inody
        FS_Inode_Ops_link               Far_Pointer <> ; vytvori link na inodu
        FS_Inode_Ops_unlink             Far_Pointer <> ; z linku udela zase inodu, jaka byla pred tim, nez se z ni udelal link
        FS_Inode_Ops_symlink            Far_Pointer <> ; vytvori z inody symbolicky odkaz
        FS_Inode_Ops_readlink           Far_Pointer <> ; navrati inodu na kretou ukazuje hardlink(link)
        FS_Inode_Ops_follow_link        Far_Pointer <> ; navrati inodu, na kterou ukazuje symlink
FS_Inode_Operations Ends

; Struktura obsahujici vsechny mozne operace v FS
FS_Operations Struc
        FS_Operations_Start             equ                     $
        FS_Super_Ops                    FS_Super_Operations     <> ; tabulka funkci pro zpravu superbloku
        FS_File_Ops                     FS_File_Operations      <> ; tabulka funkci pro praci se soubory
        FS_Inode_Ops                    FS_Inode_Operations     <> ; tabulka funkci pro praci s inodami
        FS_Driver_Type_Number           dw 0 ; pokud bude 0 pak je tato polozka
                                             ; FS_Operations volna pro dalsi pouziti
        FS_Operations_End               equ                     $
FS_Operations Ends

FS_Operations_Size      equ     FS_Operations_End - FS_Operations_Start

Max_FS_Operations       equ     Max_VFS_FS_Infos
Sel_FS_Operations       dw      0    ; selector pro ulozeni mount_strucs
Count_FS_Operations     dd      0    ; kolik jiz bylo namontovano zarizeni