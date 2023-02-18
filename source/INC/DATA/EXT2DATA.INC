EXT2_Super_Block Struc
         EXT2_Super_Block_Start  equ $
         s_inodes_count      dd 0       ; Inodes count ;
         s_blocks_count      dd 0       ; Blocks count ;
         s_r_blocks_count    dd 0       ; Reserved blocks count ;
         s_free_blocks_count dd 0       ; Free blocks count ;
         s_free_inodes_count dd 0       ; Free inodes count ;
         s_first_data_block  dd 0       ; First Data Block ;
         s_log_block_size    dd 0       ; Block size ;
         s_log_frag_size     dd 0       ; Fragment size +/- hodnoty;
         s_blocks_per_group  dd 0       ; # Blocks per group ;
         s_frags_per_group   dd 0       ; # Fragments per group ;
         s_inodes_per_group  dd 0       ; # Inodes per group ;
         s_mtime             dd 0       ; Mount time ;
         s_wtime             dd 0       ; Write time ;
         s_mnt_count         dw 0       ; Mount count ;
         s_max_mnt_count     dw 0       ; Maximal mount count +/- hodnoty;
         s_magic             dw 0       ; Magic signature ;
         s_state             dw 0       ; File system state ;
         s_errors            dw 0       ; Behaviour when detecting errors ;
         s_pad               dw 0
         s_lastcheck         dd 0       ; time of last check ;
         s_checkinterval     dd 0       ; max. time between checks ;
         s_creator_os        dd 0       ; OS ;
         s_rev_level         dd 0       ; Revision level ;
         s_def_resuid        dw 0       ; Default uid for reserved blocks ;
         s_def_resgid        dw 0       ; Default gid for reserved blocks ;
         s_reserved          dd 235 dup(0)
         EXT2_Super_Block_End  equ $
EXT2_Super_Block Ends

; Group Descriptor Structure ;
EXT2_Group Struc
         EXT2_Group_Start         equ $
         bg_block_bitmap      dd 0      ; Blocks bitmap block ;
         bg_inode_bitmap      dd 0      ; Inodes bitmap block ;
         bg_inode_table       dd 0      ; Inodes table block ;
         bg_free_blocks_count dw 0      ; Free blocks count ;
         bg_free_inodes_count dw 0      ; Free inodes count ;
         bg_used_dirs_count   dw 0      ; Directories count ;
         bg_pad               dw 0
         bg_reserved          dd 3 dup(0)
         EXT2_Group_End           equ $
EXT2_Group Ends

; Inode Structure
EXT2_Inode Struc
         EXT2_Inode_Start equ $
         i_mode  dw 0                   ; File mode ;
         i_uid   dw 0                   ; Owner Uid ;
         i_size  dd 0                   ; Size in bytes ;
         i_atime dd 0                   ; Access time ;
         i_ctime dd 0                   ; Creation time ;
         i_mtime dd 0                   ; Modification time ;
         i_dtime dd 0                   ; Deletion Time ;
         i_gid   dw 0                   ; Group Id ;
         i_links_count dw 0             ; Links count ;
         i_blocks      dd 0             ; pocet diskovych sektoru, ktere soubor zabira
         i_flags       dd 0             ; File flags ;
         i_reserved1   dd 0             ; Reserved 1 ;
         i_block       dd 15 dup(0)     ; Pointers to blocks ;
         i_version     dd 0             ; File version (for NFS) ;
         i_file_acl    dd 0             ; File ACL ;
         i_dir_acl     dd 0             ; Directory ACL ;
         i_faddr       dd 0             ; Fragment address ;
         i_frag        db 0             ; Fragment number ;
         i_fsize       db 0             ; Fragment size ;
         i_pad1        dw 0
         i_reserved2   dd 2 dup(0)
         EXT2_Inode_End equ $
EXT2_Inode Ends

; Directory Structure
EXT2_Dir Struc
      EXT2_Dir_Start    equ $
      d_inode_num       dd 0
      d_rec_len         dw 0
      d_name_len        db 0
      d_file_type       db 0
      d_Dir_name        db 256 dup(0)
      EXT2_Dir_End      equ $
EXT2_Dir Ends

; promenne, ktere budou ulozeny v cache pameti v mount tabulce
EXT2_Cache_Variables Struc
       EXT2_Cache_Variables_Start      equ $
       ; Start - tu musi zustat kompatibilita i u driveru pro dalsi FS
       Ext2_Super_Block_Sel            dw      0
       Ext2_Block_Size                 dd      (1024 shl 0)
       Ext2_First_Data_Block           dd      0
       ; End
       Ext2_Groups_Sel                 dw      0
       Ext2_Count_Groups               dd      0
       EXT2_Cache_Variables_End        equ $
EXT2_Cache_Variables Ends

;############################################################################
; Procedury pro praci s Ext2 FS
;############################################################################

; Konstanty
EXT2_Super_Block_Size           equ     EXT2_Super_Block_End - EXT2_Super_Block_Start
EXT2_Group_Size                 equ     EXT2_Group_End - EXT2_Group_Start
EXT2_Inode_Size                 equ     EXT2_Inode_End - EXT2_Inode_Start
EXT2_Dir_End                    equ     EXT2_Dir_End - EXT2_Dir_Start
EXT2_Cache_Variables_Size       equ     (EXT2_Cache_Variables_End - EXT2_Cache_Variables_Start)

;Shiftovaci tabulka
krat_1                          equ     0
krat_2                          equ     1
krat_4                          equ     2
krat_8                          equ     3
krat_16                         equ     4
krat_32                         equ     5
krat_64                         equ     6
krat_128                        equ     7
krat_256                        equ     8

; typy adresarovych v ext2
EXT2_FT_UNKNOWN                 equ     0
EXT2_FT_REG_FILE                equ     1
EXT2_FT_DIR                     equ     2
EXT2_FT_CHRDEV                  equ     3
EXT2_FT_BLKDEV                  equ     4
EXT2_FT_FIFO                    equ     5
EXT2_FT_SOCK                    equ     6
EXT2_FT_SYMLINK                 equ     7
EXT2_FT_MAX                     equ     8 ; zahrnuje vsechny predesle polozky, pouziva se, kdyz mame jen jmeno
                                          ; souboru a chceme zjistit jeho typ

; typy inod
EXT2_S_IFSOCK equ 0C000h ; socket
EXT2_S_IFLNK  equ 0A000h ; symbolicky link
EXT2_S_IFFIL  equ 08000h ; soubor
EXT2_S_IFBLK  equ 06000h ; blokove zarizeni
EXT2_S_IFDIR  equ 04000h ; adresar
EXT2_S_IFCHR  equ 02000h ; znakove zarizeni
EXT2_S_IFIFO  equ 01000h ; FIFO

; struktura i_flags
; 0.bit - Secure Deletion
;       - 1 = kdyz se bude inoda mazat, tak se v jeji strukture vynuluji i struktura i_block
;       - 0 = adresovaci struktura popisujici datove bloky souboru zustane => i_block se nebude nulovat
; 1.bit - Undelete
;       - Nepodporovano
; 2.bit - Commpressed file
;       - Nepodporovano
; 3.bit - Synchronous Update
;       - nepodporovano
; 4.bit - Immunable file
;       - 0 = vypnuto
;       - 1 = Je zakazana jakakoliv prace s inodou
; 5.bit - Append only file
;       - 0 = vypnuto
;       - 1 = do souboru se muze papisovat pouze na konec
; 6.bit - Do not dump this file
;       - 0 = vypnuto
;       - 1 = Soubor nemuze byt dumpnuty na disk

;struktura s_state
; 0.bit - Mount status
;       - 0 = FS je odmontovany
;       - 1 = FS je namontovany
; 1.bit - Is there any Error in FS
;       - 0 = False
;       - 1 = True

;vyznam hodnot v promenne s_errors
; 0 = zadna chyba nebyla nalezena
; 1 = kernel bude chybu ignorovat a pokracovat dal
; 2 = kernel premontuje zarizeni jen pro cteni
; 3 = vyvola se kernel panic

; hodnota s_creator_os = 0B1h
