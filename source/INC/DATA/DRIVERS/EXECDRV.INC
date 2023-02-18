;############################################################################
;# Datove structury a promenne driveru EXEC
;############################################################################

EXEC_DATA :
EXEC_Data_Start :
;################# standardni fce
   DD      Offset EXEC_INIT     ;00h
   DD      Offset EXEC_DONE     ;01h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;05h
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
   DD      Offset EXEC_Check_Structures      ;010h
   DD      Offset EXEC_EXEC_File             ;011h
   DD      Offset EXEC_Return_Lib_Name       ;012h
   DD      Offset EXEC_Return_Lib_Version    ;013h

EXEC_Fce_Count :
   EXEC_Library_Name    DB      'nEXEC EBF runner 1.3 ',0
EXEC_Data_End :

EXEC_DATA_SIZE equ (((EXEC_DATA_END - EXEC_DATA_Start - 1) shr 12) +1)
