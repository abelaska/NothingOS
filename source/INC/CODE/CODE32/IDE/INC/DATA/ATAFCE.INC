;################################################################################
;# Soubor datovych ukazatelu na funkce ATA supportu
;################################################################################
ATA_Table :  DD     Offset ATA_DriveDiagnostics  ;0
             DD     Offset ATA_FormatTrack       ;1
             DD     Offset ATA_InitDriveParam    ;2
             DD     Offset ATA_ReadBuffer        ;3
             DD     Offset ATA_ReadLong          ;4
             DD     Offset ATA_SetMultipleMode   ;5
             DD     Offset ATA_ReadMultiple      ;6
             DD     Offset ATA_ReadSectors       ;7
             DD     Offset ATA_VerifySectors     ;8
             DD     Offset ATA_GetCurrentPos     ;9
             DD     Offset ATA_Recalibrate       ;10
             DD     Offset ATA_Seek              ;11
             DD     Offset ATA_SetFeauture       ;12
             DD     Offset ATA_WriteBuffer       ;13
             DD     Offset ATA_WriteLong         ;14
             DD     Offset ATA_WriteMultiple     ;15
             DD     Offset ATA_WriteSectors      ;16
             DD     Offset ATA_CheckPowerMode    ;17
             DD     Offset ATA_SetIDleMode       ;18
             DD     Offset ATA_IdleImmediatly    ;19
             DD     Offset ATA_Sleep             ;20
             DD     Offset ATA_Standby           ;21
             DD     Offset ATA_StandByImm        ;22
ATA_Table_End :