;####################################################################################
;# informacni struktura pro fyzickou page
;####################################################################################
Fys_Pages_Info_Table STRUC
        FPIT_Start EQU $
         FP_CR3Coded     dw 0    ;zakodovane CR3 pro danou page
         FP_VirtualPage  dd 0    ;zakodovana virtualni page :)
        FPIT_Size EQU  ($-FPIT_Start)
Fys_Pages_Info_Table ENDS
;####################################################################################
;# informacni struktura pro page mapovane na soubor
;####################################################################################
File_Maped_Pages_Info STRUC
        FMPI_Start EQU $
         VP_VFS          dd 0    ; index do Cached nebo do VFS
         VP_Offset       dd 0    ; offset ukazujici na zacatek 4Kb bloku v souboru
         VP_Flags        db 0    ; flagy - je-li 0 pak dana polozka prazdna jest
         VP_VirtualPage  dd 0    ; ukazatel na virtualni page
         VP_CountUse     db 0    ; pocet pouziti
        FMPI_Size EQU  ($-FMPI_Start)
File_Maped_Pages_Info ENDS
;####################################################################################
;# Struktura pro pageovaci cache
;####################################################################################
Cache_Pages_Info STRUC
        CPI_Start EQU $
         C_FysPage      dd 0    ; urcuje, ktera fyzicka page je Cache
         C_Drv_Type     dd 0    ; typove cislo zarizeni, na kterem je File
         C_Drv_Inode    dd 0    ; inoda souboru na zarizeni VP_Drv_Type
        CPI_Size EQU  ($-CPI_Start)
Cache_Pages_Info ENDS
;####################################################################################
PagesMainFlags                   equ 000000011111b
PagesMainFlags_NP                equ 000000001110b
PAGE_CONST_NP_NORMAL_MEMORY      equ 000000001110b
PagesMainFlags_NP_NotSwapable    equ 111000001110b
;####################################################################################
Fys_Page_Age_Mask        equ 00000000000000000000000011111111b
Fys_Page_Age_Mask_N      equ 11111111111111111111111100000000b
Fys_Page_Cache_Mask      equ 00000000000000000000000100000000b
Fys_Page_Cache_Mask_N    equ 11111111111111111111111011111111b
Fys_Page_Used_Mask       equ 00000000000000000000001000000000b
Fys_Page_Used_Mask_File  equ 00000000000000000000011000000000b
Fys_Page_Used_MaskN      equ 11111111111111111111110111111111b
Fys_Page_Ptr_Info        equ 00000000000000000000110000000000b
Fys_Page_Pointer         equ 11111111111111111111000000000000b
;####################################################################################
_PAGE_NotAssign     equ 000b  ;stranky, ktere jeste nebyly asimilovany :)
_PAGE_NormalSwap    equ 001b  ;stranky ktere se swaple
_PAGE_ReadFromFile  equ 010b  ;stranky ktere jeste nenatazene ze souboru jsou
_PAGE_FileSwap      equ 011b  ;stranky ktere jsou swaple pres soubor
_PAGE_NonSwapable   equ 111b  ;stranky ktere se neswapuji
;####################################################################################
_PAGE_FileSwap_All12   equ 010000001110b
;####################################################################################
VP_Flag_Normal      equ 1     ;nenatahla page
VP_Flag_Accesed     equ 2     ;page je natazena ze souboru
VP_Flag_Swap        equ 3     ;page je swapla
;####################################################################################
;# Promenne
;####################################################################################
Free_FysPages_Count        dd  1024 ;pocet volnych page, ktere jsou k dostani pres momentalne namapovane podadresare
Free_FysPages_Count_Limit  equ 512  ;bude-li Free_FysPages_Count mensi nezli tato hodnota, pak se pokusime namapovat dalsi podadresar do fyslin pameti
Pages_Used                 dd  0    ;pocet pouzitych page
Pages_Used_Limit           dd  0    ;bude-li pages_used vetsi nezli tato hodnota, pak se vyvola swap demon, ktery trosku zaradi
FysPageInfoItem_Size       dd  0    ;velikost struktury FysPageInfoItem ve 4kb blocich
FMPI_Selector              dd  0    ;selector na File Maped Pages Info
FMPI_LastRec               dd  0    ;cislo posledniho zaznamu
FMPI_LastRecOfs            dd  0    ;cislo posledniho zaznamu offsetova hodnota
;####################################################################################



;##########################################################################################
;# FPIT structure
;##########################################################################################
;# 0-7   - AGE page - osm bitu AGE
;# 8     - 0 = page je normalni
;#         1 = page je cache - obrazem casti disku
;# 9     - 1 = page je pouzivana, kdyz se ji chceme zbavit, musime ji swapnout
;#       - 0 = page je neni, muzeme ji klidne smazat a nic nemusime swapovat
;# 10-11 - 00 = FP_VirtualPage ukazuje na jednu Virtualni page
;#       - 01 = FP_VirtualPage ukazuje do sruktury File_Maped_Pages_Info - mapovany soubor
;#       - 10 = FP_VirtualPage neukazuje nikam - pozustatek po pameti tasku
;#       - 11 = reserved - pro pripad kdybychom chteli sdilet samotnou pamet jinym zpusobem nez pres File_Maped_Pages_Info
;# 12-32 - Samotny ukazatel nekam... budto virtualni page nebo do nejake z tabulek
;##########################################################################################
;# bity 8 a 9 : 00 - page muzeme pri uvolnovani klidne vyhodit
;#              01 - page muzeme pri uvolnovani klidne vyhodit, je to cache
;#              10 - page musime pri uvolnovani swapnout
;#              11 - page musime swapnout, ale snazime se o to co nejmene - je to cache
;##########################################################################################
