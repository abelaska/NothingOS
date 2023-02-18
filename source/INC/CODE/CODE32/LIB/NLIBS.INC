;#################################################################################
;# Jednotka pro inicializaci normalnich knihoven :)
;#################################################################################
;# Version : 1.2-1-1
;#################################################################################


;##################################################################################
;# Procedura nastavuje normalni knihovny knihovny
;##################################################################################
;# Vstup  : DS = Data segment kernelu
;#          FS = Libraries name descriptor
;#          GS = Libraries info descriptor
;# Vystup : None
;# Nici   : None
;##################################################################################
LIB_Normal_LIBs_Initializing PROC
        Mov    EBX,LIB_Virtual_Libs
        Mov    Dword ptr DS:[LIB_NumberOfLibraries],EBX
ret
        PUSH   ES
         Mov    Dword ptr DS:[LIB_NumberOfLibraries],LIB_Virtual_Libs
         Mov    ESI, Offset Lib_Config_File
         Call   LIB_Open_File
         Jc     LIB_Normal_LIBs_Initializing_Error
         Mov    ECX,EDX
         Mov    ES,EDX
         PUSH   DS
          Mov    DS, EDX
          Xor    EDI,EDI
          LSL    ECX,ECX
   LIB_Normal_LIBs_Initializing_SearchFile :
          Mov    ESI,EDI
          Mov    Al,010
          Repnz  Scasb
          Jnz    LIB_Normal_LIBs_Initializing_EOF
          Dec    EDI
          Mov    Byte ptr ES:[EDI],0
          Inc    EDI
          Call   LIB_Open_File
          Jc     LIB_Normal_LIBs_Initializing_SearchFile
          PUSH   DS
           Mov   DS,DX
           Cmp   Dword ptr DS:[LIB_F_Signature],LibSignature
           Je    LIB_Normal_LIBs_Initializing_Valid_File
          POP    DS
          ;neni to spravny format, deallokujeme pamet a pokracujeme
           Call   Memory_Manager_Deallocate_Memory
           Jmp    LIB_Normal_LIBs_Initializing_SearchFile
   LIB_Normal_LIBs_Initializing_Valid_File :
           Mov    EAX,LMS_Struc_Size
           Mul    EBX
           Mov    EDX, EAX
           Mov    AX, Word ptr DS:[LIB_F_Version]
           Mov    Word ptr DS:[LIB_F_UnitCnt],BX
           Mov    Word ptr GS:[EDX.LIB_M_Version],AX
           Mov    Word ptr GS:[EDX.LIB_M_UseCnt],1
           Mov    Word ptr GS:[EDX.LIB_M_CallGate],0
           Mov    Word ptr GS:[EDX.LIB_M_Flags],LIB_CallGateNotExist
           Mov    Word ptr GS:[EDX.LIB_M_CodeSel],DS
           Mov    EDX,EBX
           Shl    EDX,4
           Mov    EAX,Dword ptr DS:[LIB_F_Name+0]
           Mov    Dword ptr FS:[EDX],EAX
           Mov    EAX,Dword ptr DS:[LIB_F_Name+4]
           Mov    Dword ptr FS:[EDX+4],EAX
           Mov    EAX,Dword ptr DS:[LIB_F_Name+8]
           Mov    Dword ptr FS:[EDX+8],EAX
           Mov    EAX,Dword ptr DS:[LIB_F_Name+12]
           Mov    Dword ptr FS:[EDX+12],EAX
           Inc    EBX
          Mov    EDX,DS
          POP    DS
          Call   Memory_Manager_Data2Code_Descr
          Jmp    LIB_Normal_LIBs_Initializing_SearchFile
   LIB_Normal_LIBs_Initializing_EOF :
         ;dealokujeme soubor /etc/libs.conf
         POP    DS
         Mov    Dword ptr DS:[LIB_NumberOfLibraries],EBX
         Mov    DX,ES
         Mov    AX,Data_Descriptor
         Mov    ES,AX
         Call   Memory_Manager_Deallocate_Memory
   LIB_Normal_LIBs_Initializing_END :
        POP    ES
         Ret
   LIB_Normal_LIBs_Initializing_Error:
         Print  New_Line_Txt
         Print  LIB_Error_1_MSG
         Jmp    LIB_Normal_LIBs_Initializing_END
LIB_Normal_LIBs_Initializing ENDP
;##########################################################################################
;# Procedura otevre soubor
;##########################################################################################
;# Vstup  : DS:ESI - Pchar jmeno souboru
;# Vystup : If C Then Error
;#          EDX = Selektor
;##########################################################################################
LIB_Open_File PROC
       PUSH  DS ESI EAX ECX EBX
         Mov   EAX,1
         Call  VFS_Open
         Jc    LIB_Open_File_End
         Xor   EBX,EBX
         Xor   ECX,ECX
         Dec   ECX
         Call  VFS_Map_File
   LIB_Open_File_end :
       POP   EBX ECX EAX ESI DS
        Ret
LIB_Open_File ENDP
