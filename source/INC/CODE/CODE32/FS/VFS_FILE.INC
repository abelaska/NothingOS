;############################################################################
;# Rutiny pro uzivatelske rozhrani pro praci se soubory
;############################################################################

; jo tasky musi mit descriptor s rukejetema k souborum s DPL=0
; aby tam nemohli zapisovat !!!

; otevre soubor pro task
; Vstup:  DS:ESI - cesta k souboru, ukoncena na 0
;         EAX    - zpusob otevreni souboru
; Vystup:
VFS_Open_File Proc
; zjisti, jestli je uz soubor otevreny v nejake VFS inode
; if false goto 2
 ; zjisti, jestli neni VFS inoda blokovana
 ; if true goto end_false
 ; zjisti, jestli je nastaven Share bit
 ; if true goto 3
; zjisti, jestli oteviram soubor pro zapis
; if true goto 1
 ; zjisti, jestli je uz soubor otevreny v nejake VFS inode
 ; if false goto 2
  ; zjisti, jestli je vfs inoda otevrena pro cteni
  ; if false goto end_false => je otevrena pro zapis nebo je soubor uzamcen
;3:
   ; zvysi ve VFS inode "count of use" a vytvori rukojet
   ; goto end_true
;1:
 ; zjisti, jestli je uz soubor otevreny v nejake VFS inode
 ; if true goto end_false
;2:
 ; vytvori VFS inodu pro pozadovany soubor v EAX modu
 ; goto end_true
;end_false:
 ; goto end
;end_true:
;end:
       ret
VFS_Open_File Endp

; uzavre soubor pro task
; Vstup:  EAX    - cislo rukojeti
; Vystup: CF = 0 - OK
;         CF = 1 - Error
VFS_Close_File Proc
       ; sniz count ve vfs inode
       ; a oznac handle jako volny
       ret
VFS_Close_File Endp

; najde volnou rukojet v descriptoru rukojeti daneho tasku
; Vstup: -
; Vystup: CF = 0 - OK
;                - ES:EDI = ukazatel na volnou polozku
;         CF = 1 - Error
File_Find_Free_handle Proc
       ret
File_Find_Free_handle endp