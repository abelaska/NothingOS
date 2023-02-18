IDE_DataStart :
;################# standardni fce
   DD      Offset INIT          ;00h
   DD      Offset DONE          ;01h
   DD      Offset READ          ;02h
   DD      Offset WRITE         ;03h
   DD      Offset SEEK          ;04h
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
   DD      Offset G_ATA_DriveDiagnostics  ;010h
   DD      Offset G_ATA_FormatTrack       ;011h
   DD      Offset G_ATA_InitDriveParam    ;012h
   DD      Offset G_ATA_ReadBuffer        ;013h
   DD      Offset G_ATA_ReadLong          ;014h
   DD      Offset G_ATA_SetMultipleMode   ;015h
   DD      Offset G_ATA_ReadMultiple      ;016h
   DD      Offset G_ATA_ReadSectors       ;017h
   DD      Offset G_ATA_VerifySectors     ;018h
   DD      Offset G_ATA_GetCurrentPos     ;019h
   DD      Offset G_ATA_Recalibrate       ;01Ah
   DD      Offset G_ATA_Seek              ;01Bh
   DD      Offset G_ATA_SetFeauture       ;01Ch
   DD      Offset G_ATA_WriteBuffer       ;01Dh
   DD      Offset G_ATA_WriteLong         ;01Eh
   DD      Offset G_ATA_WriteMultiple     ;01Fh
   DD      Offset G_ATA_WriteSectors      ;020h
   DD      Offset G_ATA_CheckPowerMode    ;021h
   DD      Offset G_ATA_SetIDleMode       ;022h
   DD      Offset G_ATA_IdleImmediatly    ;023h
   DD      Offset G_ATA_Sleep             ;024h
   DD      Offset G_ATA_Standby           ;025h
   DD      Offset G_ATA_StandByImm        ;026h
IDE_Fce_Count :
   Library_Name    DB      'IDE',0
   IDE_Flags       DB      00h          ; flagy z IRQ
   Initialized     DB      00h          ; =0 pak jeste nejni inicializovany
   Part_Sel        Dw      63h          ; selektor na seznam partitionu
   Part_Per_Disk   DD      64h          ; kolik muze byt na jednom disku partitionu

   IDE_Buffer      DB 512 dup ('A')       ; 1sektorovy buffer pro cteni
IDE_TABLE_PTRs :

   IDE_Drive0_OFS  DD Offset IDE_Drive0
   IDE_Drive1_OFS  DD Offset IDE_Drive1
   IDE_Drive2_OFS  DD Offset IDE_Drive2
   IDE_Drive3_OFS  DD Offset IDE_Drive3

   IDE_Drive0      IDE_TABLE <1F0h,0>   ; info struktura pro Primary Master
   IDE_Drive1      IDE_TABLE <1F0h,1>   ; info struktura pro Primary Slave
   IDE_Drive2      IDE_TABLE <170h,0>   ; info struktura pro Secondary Master
   IDE_Drive3      IDE_TABLE <170h,1>   ; info struktura pro Secondary Slave
   Temp_Disk        DD      00h          ; pomocna promenna pro inicializaci Partitionu
   Partition_Count  DD      00h
   Temp_Sel         DW      00h
   Temp_Ofs         DD      00h
   Initializing_Delay equ 50
   Include         .\..\INC\code\code32\ide\inc\data\ATAfce.inc
DataEnd :