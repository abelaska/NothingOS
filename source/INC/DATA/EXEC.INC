;###########################################################################
;# Struktury Execu
;###########################################################################

;####################################################################################
;# Struktura hlavicky
;####################################################################################
EBF_Header_Struc   Struc
      EBF_Signature           DB  4  dup (0) ;signatura
      EBF_Header_Size         DD  0          ;velikost hlavicky
      EBF_Name                DB  32 dup (0) ;vnitrni jmeno souboru
      EBF_Data_Size           DD  0          ;velikost data segmentu ve 4k (zarovnani nahoru)
      EBF_Code_Header_Size    DD  0          ;velikost hlavicky a code segmentu ve 4k (zarovnani nahoru)
      EBF_Code_Entry_Point    DD  0          ;vstupni bod programu
      EBF_Flags               DD  0          ;flagy
      EBF_Stack_Size          DD  0          ;velikost zasobniku
      EBF_LibsNum             DD  0          ;pocet knihoven
      EBF_Libs equ $                         ;zacatek knihoven
EBF_Header_Struc   Ends
;####################################################################################
;# Struktura podhlavicky
;####################################################################################
EBF_Lib_SubHeader  Struc
EBF_LSH_Start Equ $
      EBF_LS_LibName       DB  16 dup (0)
      EBF_LS_Lib_MinVer    DW  0
      EBF_LS_Lib_CallGate  DD  0
EBF_LSH_Size  Equ ($-EBF_LSH_Start)
EBF_Lib_SubHeader  Ends
;####################################################################################
EXEC_Driver_Lib_Error equ 00010000h
EBF_Signature_Equ     equ 00464245h ;= 'EBF',0
;####################################################################################
EXEC_SS               dd 0
Exec_Saved_SS         dd 0
Exec_Saved_ESP        dd 0
;####################################################################################
DRV_Exec_Drivers_Res    equ 255   ;muze byt 255 driveru
DRV_Exec_Drivers        equ 8000h ;prvni je na adrese 8000
DRV_Exec_Check_Fce      equ 10h   ;fce check v EXEC driveru je na 10h
DRV_Exec_Exec_Fce       equ 11h   ;fce exec je na 11h
DRV_Exec_AddOfs         equ 1
;####################################################################################
EXEC_Structure Struc
EXEC_S_Start Equ $
       EXEC_VFS_Std_In  DD 0    ;VFS inoda standardniho vstupu
       EXEC_VFS_Std_OUT DD 0    ;VFS inoda standardniho vystupu
       EXEC_VFS_Std_Err DD 0    ;VFS inoda standardniho erroroveho vystupu
       EXEC_VFS_Home    DD 0    ;VFS inoda adresare, ze ktereho byl task spusten
       EXEC_VFS_Temp    DD 0    ;VFS inoda adresare, ktery task pouziva jako pracovni
       EXEC_VFS_Work    DD 0    ;VFS inoda adresare, ve kterem se prave nachazime
       EXEC_Run_Mode    DW 0    ;Mod spusteni - 0 - child, 1 - brother
       EXEC_Run_Status  DW 0    ;Status spusteni - Sleep, run,...
       EXEC_Priority    DW 0    ;Priorita tasku
       EXEC_CPLx        DW 0    ;CPL tasku
       EXEC_UID_GID     DD 0    ;UID a GID tasku
       EXEC_UID_GID_Eff DD 0
       EXEC_UID_GID_FS  DD 0
       EXEC_VFS         DD 0    ;VFS spousteneho souboru - nenastavovat
EXEC_S_Size Equ ($ - EXEC_S_Start)
EXEC_Structure ENDS

EXEC_Temp_Struc EXEC_Structure <>

CPL0_Stack_Size         equ 2   ;CPL0 velikost zasobniku

EXEC_Run_Mode_Brother   equ 1
EXEC_Run_Mode_Child     equ 0

