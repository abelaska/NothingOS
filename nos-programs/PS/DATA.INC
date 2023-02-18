Data Segment
Data_Segment_Start   equ $
PREVOD_Output       db '0123456789ABCDEF'
;#### Texts
prog_name    db '\nps v1.0, compiled ',??DATE,' at ',??TIME,'\n',0
Error        db '\nerror - not enought memory to get all tasks information',0
Family_Info  db 'PID        UID   GID   Parent     Child       Priority    Status\n',0
Go_UID       db '\033[12`',0
Go_GID       db '\033[18`',0
Go_Parent    db '\033[24`',0
Go_Child     db '\033[35`',0
Go_Priority  db '\033[47`',0
Go_Status    db '\033[59`',0

Status_Run     db 'running\n',0
Status_Stop    db 'stopped\n',0
Status_Sleep   db 'sleeping\n',0
Status_Forever db 'immortable\n',0
Status_Zoombie db 'zoombie\n',0
Status_unknown db 'unknown\n',0

Name_kernel    db 'kernel',0
Name_none      db 'none',0

;#### Call Gates na knihovny
Mem_Call_Gate dd 0
              dw 0
TSK_Call_Gate dd 0
              dw 0
VFS_Call_Gate dd 0
              dw 0
;#### VFS inody standarnich vstupu - vystupu
VFS_Standard_In     dd 0
VFS_Standard_Out    dd 0
VFS_Standard_Error  dd 0
;####################################################################################
DEC_Numbers      dd 0000000001
                 dd 0000000010
                 dd 0000000100
                 dd 0000001000
                 dd 0000010000
                 dd 0000100000
                 dd 0001000000
                 dd 0010000000
                 dd 0100000000
                 dd 1000000000
Numbers_Tab      db '0123456789ABCDEF'
;####################################################################################
_TSK_Status_RUN             equ  01h     ;Proces bezi normalne
_TSK_Status_SLEEP           equ  02h     ;Proces spi, ceka na probuzeni vnejsi udalosti
_TSK_Status_STOP            equ  03h     ;Proces je zastaven
_TSK_Forever_Run            equ  04h     ;Proces, ktery nelze zabit
_TSK_Zoombie                equ  0FFFFh  ;Proces se chce nechat zabit :)

TSK_Info_Struc Struc
TSK_Info_start equ $
          TSK_Info_ID             dd 0        ;cislo tasku
          TSK_Info_ParentTask     dd 0        ;cislo rodicovskeho tasku
          TSK_Info_ChildTask      dd 0        ;cislo ditetovskeho tasku
          TSK_Info_OlderBrother   dd 0        ;cislo tasku starsiho tasku
          TSK_Info_YoungerBrother dd 0        ;cislo tasku mladsiho bratra
          TSK_Info_Priority       dw 0        ;priorita
          TSK_Info_Status         dw 0        ;status
          TSK_Info_UID_GID        dd 0        ;UID a GID tasku
          TSK_Info_UID_GID_Eff    dd 0        ;efektivni UID a GID
          TSK_Info_UID_GID_fs     dd 0        ;UID a GID pro filesystem
          TSK_Info_UID_GID_save   dd 0        ;Ulozene UID a GID
          TSK_Info_Variable       dd 0        ;promenna ma ruzna vyuziti
          TSK_Info_Selektor       dw 0        ;ukazatel na TSS tasku
TSK_Info_Size equ ($ - TSK_Info_start)
TSK_Info_Struc ENDS
Data_Segment_Size    equ (($-Data_Segment_Start)Shr 12)+1
Data Ends