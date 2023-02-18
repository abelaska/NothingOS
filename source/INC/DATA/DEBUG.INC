;##############################################################################
;# Datove struktury debugu...
;##############################################################################
Debug_Data_Descriptor dw 0
Debug_Prefixsize      dd 0

Debug_Start_EIP       dd 0
Debug_Active_Show     db 0

debug_loaded          db 0
Debug_Text_Different_Code db 'Executing non-kernel code segment...',0