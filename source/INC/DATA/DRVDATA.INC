;############################################################################
;# Datove structury a promenne pro praci s drivery
;############################################################################

; Standartni funkce driveru
Drv_Std_INIT        equ 0
Drv_Std_DONE        equ 1
Drv_Std_READ        equ 2
Drv_Std_WRITE       equ 3
Drv_Std_SEEK        equ 4

; typove cisla pouzivane standartnimi drivery NothingOS
Consoles_driver     equ 4010h      ;znakove zarizeni, nechame 15 spodnich jako rezervu
Keyboard_driver     equ 4020h      ;znakove zarizeni
IDE_driver          equ 0010h      ;blokove zarizeni, nechame 15 spodnich jako rezervu
EBF_EXEC_driver     equ 8000h      ;softwareove zarizeni
Char_Encode_driver  equ 8100h      ;softwareove zarizeni
EXT2_Driver_driver  equ 8101h      ;softwarove zarizeni

; pozice jednotlivych promennych v polozce ovladace v pameti
DRV_Device Struc
       DRV_Device_Start   equ $
       Device_Type        dw 0 ; pokud typ=0 pak je tato polozka volna
       Device_Semafor     dw 0
       Device_Func_Count  dw 0
       Device_Sel_Code    dw 0
       Device_Sel_Data    dw 0
       DRV_Device_End     equ $
DRV_Device Ends

; Device_Semafor :
; 15             0
; 000000000000000X
; 0.bit - X = 0 - Driver jeste nebyl inicializovan
;           = 1 - Drivej je inicializovany

; pozice jednotlivych promennych v hlavicce driveru
DRV_File_Header Struc
       DRV_File_Header_Start equ $
       drv_id           dd 0
       drv_Device_Type  dw 0
       drv_Data_Size    dw 0
       drv_Code_Size    dd 0
       drv_Start_Data   dd 0
       drv_Start_Code   dd 0
       drv_Func_Count   dw 0
       DRV_File_Header_End equ $
DRV_File_Header Ends

DRV_Device_Struc_Size      equ DRV_Device_End - DRV_Device_Start
DRV_File_Header_Struc_Size equ DRV_File_Header_End - DRV_File_Header_Start

Virtual_Drivers equ 1

; inoda na /drv
Max_DRVS           equ 1024
drivers_count      dd 0 ; kolik driveru je nacteno do pameti
sel_drivers_info   dw 0 ; selector na informacni strukturu driveru

DRV_Signature      equ 000565244h  ; 'DRV',0 - signatura driveru

drv_conf_file     db '/etc/drivers.conf',0