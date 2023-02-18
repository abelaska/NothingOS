;############################################################################
;# Datove structury a promenne pro zobrazovani chybovych hlasek
;############################################################################

; error 0 -> nesmi existovat !!

; error chybove cisla:
;  rezerved - 0 (konec tabulky)
;  KER      - 0001h .. 0FFFh - kernel
;  LIB      - 1000h .. 1FFFh - libraries
;  DRV      - 2000h .. 2FFFh - drivers
;  FS       - 3000h .. 3FFFh - file systems
;  VFS      - 4000h .. 4FFFh - virtual file system
;  TSK      - 5000h .. 5FFFh - task man
;  MM       - 6000h .. 6FFFh - memory manager

er_KER  equ 00001h
er_LIB  equ 01000h
er_DRV  equ 02000h
er_FS   equ 03000h
er_VFS  equ 04000h
er_TSK  equ 05000h
er_MM   equ 06000h
er_Swap equ 07000h

; tabulka adres na texty chybovych hlasek
Error_MSG_Table:
 dd er_KER+0000h, KERNEL_Error_0_MSG
 dd er_KER+0001h, KERNEL_Error_1_MSG
 dd er_KER+0002h, KERNEL_Error_2_MSG
 dd er_KER+0003h, KERNEL_Error_3_MSG

 dd er_LIB+0000h, LIB_Error_0_MSG
 dd er_LIB+0001h, LIB_Error_1_MSG
 dd er_LIB+0002h, LIB_Error_2_MSG

 dd er_DRV+0000h, DRV_Error_0_MSG
 dd er_DRV+0001h, DRV_Error_1_MSG
 dd er_DRV+0002h, DRV_Error_2_MSG
 dd er_DRV+0003h, DRV_Error_3_MSG

 dd er_FS +0000h, FS_Error_0_MSG

 dd er_VFS+0000h, VFS_Error_0_MSG
 dd er_VFS+0001h, VFS_Error_1_MSG
 dd er_VFS+0002h, VFS_Error_2_MSG

 dd er_MM +0000h, MM_Error_0_MSG
 dd er_MM +0001h, MM_Error_1_MSG
 dd er_MM +0002h, MM_Error_2_MSG
 dd er_MM +0003h, MM_Error_3_MSG
 dd er_MM +0004h, MM_Error_4_MSG

 dd er_TSK +0000h, TSK_Error_0_MSG
 dd er_TSK+0001h,  TSK_Error_1_MSG

 dd er_Swap +0000h, Swap_Error_0_MSG

 ; tab. musi byt vzdy zakoncena errorem cislo "0"
 dd 0           , Error_Error

Error_Error db 'Error message not found',0

; Error hlasky kernelu
KERNEL_Error_0_MSG db 'Sorry, but detected procesor is less than 80386!',0
KERNEL_Error_1_MSG db 'Error while initializing A20 Gate...',0
KERNEL_Error_2_MSG db 'Error when loading partitions',0
KERNEL_Error_3_MSG db 'Partition with NOTHING native not found',0

; Error hlasky memory manageru
MM_Error_0_MSG db 'Insufficient of memory',0
MM_Error_1_MSG db 'You wanted to allocate 0 bytes of memory',0
MM_Error_2_MSG db 'Error when initializing paging, system halted',0
MM_Error_3_MSG db 'Swaping error... system halted :(',0
MM_Error_4_MSG db 'Error when creating LDT',0

; Error hlasky fci pro praci s knihovnama
LIB_Error_0_MSG db 'Error when loading libraries - not enought memory',0
LIB_Error_1_MSG db 'Directory "/lib" not found',0
LIB_Error_2_MSG db 'Error when loading "/lib/lib.conf"',0

; Error hlasek fci pro praci s drivery
DRV_Error_0_MSG db 'Error when loading drivers',0
DRV_Error_1_MSG db 'Directory "/drv" not found',0
DRV_Error_2_MSG db 'File "/drv/drivers.conf" not found',0
DRV_Error_3_MSG db 'Error when loading "/drv/drivers.conf"',0

; error hlasky fci pro praci s FS
FS_Error_0_MSG  db 'Error when loading FS informations',0

;hlasky TSK
TSK_Error_0_MSG db 'Error when init. TaskMan, not enought memory',0
TSK_Error_1_MSG db 'Error - task not found',0

; error hlasky fci pro praci s VFS
VFS_Error_0_MSG db 'Error when loading EXT2 library',0
VFS_Error_1_MSG db 'Error when mounting ROOT',0
VFS_Error_2_MSG db 'EXT2 FS is not supported',0

; error hlasky fci swap deamona
Swap_Error_0_MSG db 'Error, page not found in swap partition',0

; seznam konstant chybovych hlasek
Er_Nedostatek_Pameti           equ er_MM+0
Er_allocate_0_bytes            equ er_MM+1
Er_directory_drv_not_found     equ er_DRV+1
Er_file_drivers_conf_not_found equ er_DRV+2
Er_loading_drivers_conf        equ er_DRV+3
Er_root_partition_not_found    equ er_KER+3
Er_A20_error                   equ er_KER+1
Er_bad_procesor                equ er_KER+0
Er_lib_error                   equ er_LIB+0
Er_loading_ext2_lib            equ er_VFS+0
Er_mounting_root               equ er_VFS+1
Er_ext2_fs_not_found           equ er_VFS+2

;###########################################################################
;# GP
;###########################################################################
Error_n1    dd   0
Error_n2    dd   0
Error_Info  db  '\nPress any key to view fault point',0

GP         db   'ERROR ---> General Protection Fault at ',0
GP_cons    db   '\n\nERROR ---> General Protection Fault at %h4'
GP_cons_o1 dd   offset Error_n1
           db   ':%h4'
GP_cons_o2 dd   offset Error_n2
           db   0
GP_Task       db   '\nGeneral Protection Fault at '
GP_Task_size  equ  $ - Offset GP_Task;

DE         db   'ERROR ---> Divide error at ',0
DE_cons    db   '\n\nERROR ---> Divide error at %h4'
DE_cons_o1 dd   offset Error_n1
           db   ':%h4'
DE_cons_o2 dd   offset Error_n2
           db   0

UD         db   'ERROR ---> Invalide Opcode at ',0
UD_cons    db   '\n\nERROR ---> Invalide Opcode  at %h4'
UD_cons_o1 dd   offset Error_n1
           db   ':%h4'
UD_cons_o2 dd   offset Error_n2
           db   0

NM         db   'ERROR ---> Device Not Avaible at ',0
NM_cons    db   '\n\nERROR ---> Device Not Avaible  at %h4'
NM_cons_o1 dd   offset Error_n1
           db   ':%h4'
NM_cons_o2 dd   offset Error_n2
           db   0

DDF         db   'ERROR ---> Double Fault at ',0
DDF_cons    db   '\n\nERROR ---> Double Fault  at %h4'
DDF_cons_o1 dd   offset Error_n1
           db   ':%h4'
DDF_cons_o2 dd   offset Error_n2
           db   0

CO         db   'ERROR ---> Coprocessor Segment Overrun at ',0
CO_cons    db   '\n\nERROR ---> Coprocessor Segment Overrun  at %h4'
CO_cons_o1 dd   offset Error_n1
           db   ':%h4'
CO_cons_o2 dd   offset Error_n2
           db   0

TS         db   'ERROR ---> Invalid TSS at ',0
TS_cons    db   '\n\nERROR ---> Invalid TSS at %h4'
TS_cons_o1 dd   offset Error_n1
           db   ':%h4'
TS_cons_o2 dd   offset Error_n2
           db   0

NP         db   'ERROR ---> Segment not present at ',0
NP_cons    db   '\n\nERROR ---> Segment not present at %h4'
NP_cons_o1 dd   offset Error_n1
           db   ':%h4'
NP_cons_o2 dd   offset Error_n2
           db   0

SSS         db   'ERROR ---> Stack Fault at ',0
SSS_cons    db   '\n\nERROR ---> Stack Fault at %h4'
SSS_cons_o1 dd   offset Error_n1
            db   ':%h4'
SSS_cons_o2 dd   offset Error_n2
           db   0

Error_Addr      dd 0
Error_Addr_Txt  db '\nError address KERNEL:%h4'
                dd offset Error_Addr
                db 'h',0
Error_Addr_Old_Txt db 'Error address KERNEL:',0
Error_Mem db '\nMemory Error',0

Double_Trouble db ':'