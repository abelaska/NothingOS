;############################################################################
;# Texty, ktere se zobrazuji pri inicializaci NothingOS
;############################################################################

Nothing_Init_txt  db 'Nothing Operation System v1.5 ...\n'
                  db 'Compiled '
                  db ??DATE
                  db ' at '
                  db ??TIME
                  db 0
CompInfoText1     db '\nRunning on 80x'
CompInfoText2     db '086 processor at %d4'
                  dd  Offset Processor_Speed
                  db 'MHz',0
CompInfoText3     db ', with %d4'
                  dd  Offset RAM_Size_MB
                  db 'MB RAM\n',0

Root_Mounted_txt      db '\nRoot partition mounted ...',0
Consoles_Init_txt     db '\nConsoles initialized ...',0
Debug_Init_txt        db '\nDebugger initialized ...',0
Task_Init_txt         db '\nTaskMan initialized ...',0
VFS_Init_txt          db '\nVFS initialized ...',0
FS_Init_txt           db '\nFS initialized ...',0
Drv_Internal_init_txt db '\nInternal drivers initialized ...',0
Drv_Load_External_txt db '\nExternal drivers loaded...',0
Drv_External_init_txt db '\nExternal drivers initialized...',0
Wait_init_txt         db '\nWait loops calibrated ...',0
Swap_Init_txt         db '\nSwap Deamon initialized, using %d4'
                      dd offset Swap_Partition_Size_MB
                      db 'MB swap partition ...',0
Swap_Init_Error_txt   db '\nSWAP partition not found, Swap Deamon running in pasive mode',0
Libraries_Init_TXT    db '\nLibraries loaded ...',0
FPU_Init_TXT          db '\nFPU initialized ...',0
Starting_init         dB '\nStarting INIT ',0
Point_txt             DB '.',0