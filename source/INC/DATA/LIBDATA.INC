;##############################################################################
;# Datove struktury zavadece knihoven
;##############################################################################

LIB_File_Struc struc
     LIB_F_Signature   DD 0
     LIB_F_Version     DW 0
     LIB_F_Name        DB 16 Dup(0)
     LIB_F_UnitCnt     DW 0
     LIB_F_CodeLen     DD 0
     LIB_F_CallGate    DB 0
LIB_File_Struc ends


LIB_Mem_Struc struc
  LMS_Struc_Start equ $
     LIB_M_CodeSel     DW 0
     LIB_M_UseCnt      DW 0
     LIB_M_CallGate    DW 0
     LIB_M_Flags       DW 0
     LIB_M_Version     DW 0
  LMS_Struc_Size equ ($-LMS_Struc_Start)
LIB_Mem_Struc ends


;Flagy
LIB_CallGateNotExist   equ 000b
LIB_CallGateExist      equ 001b
Virtual_Lib_Flags      equ 011b


LIB_NumberOfLibraries           dd 0   ;pocet knihoven celkem
LibNames_Descriptor             dw 0   ;descriptor na jmena knihoven
LibInfo_Descriptor              dw 0   ;descriptor na informace o knihovnach

Lib_Config_File                 db '/etc/libs.conf',0

Libs_Name_Len                   Equ 16    ;nazev knihovny 16b
Max_Libs                        Equ 512   ;pocet knihoven - 512
LibSignature                    Equ 0042494Ch ;Signatura souboru knihovny... ='LIB',0
Inode_dir_LIB                   DD 0