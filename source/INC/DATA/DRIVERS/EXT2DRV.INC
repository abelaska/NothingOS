;############################################################################
;# Datove structury a promenne driveru EXT2FS
;############################################################################

EXT2_Driver_DATA :
EXT2_Driver_Data_Start :
;################# standardni fce
   DD      Offset EXT2_INIT     ;00h
   DD      Offset EXT2_DONE     ;01h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;06h
   DD      Offset UnImplement   ;07h
   DD      Offset UnImplement   ;08h
   DD      Offset UnImplement   ;09h
   DD      Offset UnImplement   ;0Ah
   DD      Offset UnImplement   ;0Bh
   DD      Offset UnImplement   ;0Ch
   DD      Offset UnImplement   ;0Dh
   DD      Offset UnImplement   ;0Eh
   DD      Offset UnImplement   ;0Fh
;################# rozsirene fce
FS_EXT2_Operations_Struc:
   dd offset EXT2_Super_Read_Super  ; 10h nacte superblock
   dd offset EXT2_Super_Write_Super ; 11h zapise superblock na disk
   dd offset EXT2_Super_FSstat      ; 12h vrati informace o FS
   dd offset EXT2_Super_ReMount     ; 13h remontuje zarizeni
   dd offset EXT2_Super_Lock        ; 14h uzamce zarizeni
   dd offset EXT2_Super_UnLock      ; 15h odemkne zarizeni
   dd offset EXT2_File_read         ; 16h cteni ze souboru
   dd offset EXT2_File_write        ; 17h zapis do souboru
   dd offset EXT2_File_seek         ; 18h posouvani ukazatele v souboru
   dd offset EXT2_File_Get_Map      ; 19h navrati mapu adresovacich bloku na datove struktury souboru
   dd offset EXT2_File_read_dir     ; 20h navraci buffer, ve kterem je zachycen obraz obsahu adresare
   dd offset EXT2_File_open         ; 21h otevre soubor v tomto FS
   dd offset EXT2_File_release      ; 22h tato fce se zavola, kdyz se uzavre posledni rukojet na danou VFS inodu
   dd offset EXT2_File_poll         ; 23h zjisti, jestli neni soubor uzamcen
   dd offset EXT2_File_truncate     ; 24h usekne soubor od aktualni pozice ukazetele v souboru
   dd offset EXT2_File_io_cmd       ; 25h vstupne vystupni funkce pro manipulaci s inodou (napr. u zarizeni meni jeho typove cislo, atd.)
   dd offset EXT2_File_fsync        ; 26h zapise data souboru z pameti na disk
   dd offset EXT2_File_lock         ; 27h uzamkne VFS inodu, na kterou ukazuje rukojet
   dd offset EXT2_Inode_create      ; 28h vytvori inodu
   dd offset EXT2_Inode_Load        ; 29h nacte info struc. inody
   dd offset EXT2_Inode_rename      ; 30h prejmenuje inodu
   dd offset EXT2_Inode_lookup      ; 31h podle cesty najde inodu a vrati jeji cislo
   dd offset EXT2_Inode_mknod       ; 32h vytvori specialni soubor (socket,fifo,...)
   dd offset EXT2_Inode_mkdir       ; 33h vytvori adresar
   dd offset EXT2_Inode_rmdir       ; 34h smaze adresar
   dd offset EXT2_Inode_permission  ; 35h nastavi prava a attributy inody
   dd offset EXT2_Inode_link        ; 36h vytvori link na inodu
   dd offset EXT2_Inode_unlink      ; 37h z linku udela zase inodu, jaka byla pred tim, nez se z ni udelal link
   dd offset EXT2_Inode_symlink     ; 38h vytvori z inody symbolicky odkaz
   dd offset EXT2_Inode_readlink    ; 39h navrati inodu na kretou ukazuje hardlink(link)
   dd offset EXT2_Inode_follow_link ; 40h navrati inodu, na kterou ukazuje symlink

EXT2_Driver_Fce_Count :
   EXT2_Library_Name    DB      'EXT2FS',0
EXT2_Driver_Data_End :

EXT2_Driver_DATA_SIZE equ (((EXT2_Driver_DATA_END - EXT2_Driver_DATA_Start - 1) shr 12) +1)

