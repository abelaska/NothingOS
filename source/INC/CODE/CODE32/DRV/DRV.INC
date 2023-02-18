;####################################################################################
;# hlavni inicializate pamet pro drivers info a registrace internich driveru
;####################################################################################
DRV_main_Init PROC
        mov  ax, Data_Descriptor
        mov  ds, ax
        ; alokuje pamet pro datove struktury zarizeni
        mov  eax, Max_DRVS*DRV_Device_Struc_Size
        call kAlloc
        mov  word ptr ds:[sel_drivers_info], ax
        mov  es, ax

        ; pocet nactenych driveru
        mov  dword ptr ds:[drivers_count], 4

        ;IDE driver
        mov  word ptr ES:[(0*DRV_Device_Struc_Size)+Device_Type], IDE_driver
        mov  word ptr ES:[(0*DRV_Device_Struc_Size)+Device_Semafor], 0
        mov  word ptr ES:[(0*DRV_Device_Struc_Size)+Device_Func_Count], (IDE_Fce_Count - IDE_DataStart)/4
        mov  word ptr ES:[(0*DRV_Device_Struc_Size)+Device_Sel_Code], cs
        mov  word ptr ES:[(0*DRV_Device_Struc_Size)+Device_Sel_Data], IDE_DATA_Descriptor

        ; EXEC driver
        mov  word ptr ES:[(1*DRV_Device_Struc_Size)+Device_Type], EBF_EXEC_driver
        mov  word ptr ES:[(1*DRV_Device_Struc_Size)+Device_Semafor], 0
        mov  word ptr ES:[(1*DRV_Device_Struc_Size)+Device_Func_Count],(EXEC_Fce_Count-EXEC_Data_Start)/4
        mov  word ptr ES:[(1*DRV_Device_Struc_Size)+Device_Sel_Code], cs
        mov  word ptr ES:[(1*DRV_Device_Struc_Size)+Device_Sel_Data], EXEC_DATA_Descriptor

        ; CONS driver
        mov  word ptr ES:[(2*DRV_Device_Struc_Size)+Device_Type], Consoles_driver
        mov  word ptr ES:[(2*DRV_Device_Struc_Size)+Device_Semafor], 0
        mov  word ptr ES:[(2*DRV_Device_Struc_Size)+Device_Func_Count],(Console_Driver_Fce_Count-Console_Driver_Data_Start)/4
        mov  word ptr ES:[(2*DRV_Device_Struc_Size)+Device_Sel_Code], cs
        mov  word ptr ES:[(2*DRV_Device_Struc_Size)+Device_Sel_Data], Cons_DATA_Descriptor

        ; EXT2 driver
        mov  word ptr ES:[(3*DRV_Device_Struc_Size)+Device_Type], EXT2_Driver_driver
        mov  word ptr ES:[(3*DRV_Device_Struc_Size)+Device_Semafor], 0
        mov  word ptr ES:[(3*DRV_Device_Struc_Size)+Device_Func_Count],(EXT2_Driver_Fce_Count-EXT2_Driver_Data_Start)/4
        mov  word ptr ES:[(3*DRV_Device_Struc_Size)+Device_Sel_Code], cs
        mov  word ptr ES:[(3*DRV_Device_Struc_Size)+Device_Sel_Data], EXT2_DATA_Descriptor

       ; KEYB driver
        mov  word ptr ES:[(4*DRV_Device_Struc_Size)+Device_Type], Keyboard_driver
        mov  word ptr ES:[(4*DRV_Device_Struc_Size)+Device_Semafor], 0
        mov  word ptr ES:[(4*DRV_Device_Struc_Size)+Device_Func_Count],(KEYB_Fce_Count-KEYB_Data_Start)/4
        mov  word ptr ES:[(4*DRV_Device_Struc_Size)+Device_Sel_Code], cs
        mov  word ptr ES:[(4*DRV_Device_Struc_Size)+Device_Sel_Data], KEYB_DATA_Descriptor

        ;Inicializace Internich Driveru
        call  DRV_Init

        Print Drv_Internal_init_txt
        RET
DRV_main_Init ENDP

; Nacte drivery podle obsahu souboru /etc/drivers.conf
DRV_Load_Drivers Proc
      push   eax ebp esi ds
       mov   eax, data_descriptor
       mov   ds, eax
       mov   esi, offset drv_conf_file
       mov   ebp, offset DRV_Load_Driver
       call  Load_Filenames_From_File
      pop    ds esi ebp eax
       Print Drv_Load_External_txt
       call  DRV_Init
       Print Drv_External_init_txt
       ret
DRV_Load_Drivers Endp

include .\..\inc\code\code32\drv\drv_load.inc
include .\..\inc\code\code32\drv\drv_tool.inc
include .\..\inc\code\code32\drv\drv_call.inc