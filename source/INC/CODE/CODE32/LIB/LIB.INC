;##################################################################################
;# Zavadec knihoven
;##################################################################################
;# Version : 1.2-1-1
;##################################################################################

;##################################################################################
;# Inicializacni procedura zavadece knihoven
;##################################################################################
;# Vstup  : None
;# Nici   :
;##################################################################################
LIB_Initializing PROC
          Push   FS
          Push   GS
          Mov    EAX,Data_Descriptor
          Mov    DS,EAX
          Add    ECX,(LIB_Virtual_Libs-1)
          Mov    EAX,(Libs_Name_Len*Max_Libs)
          Call   Kalloc
          Mov    Word ptr DS:[LibNames_Descriptor],AX
          Mov    FS,AX
          Mov    EAX,(LMS_Struc_Size*Max_Libs)
          Call   Kalloc
          Mov    Word ptr DS:[LibInfo_Descriptor],AX
          Mov    GS,AX
          Call   LIB_Virtual_LIBs_Initializing
          Call   LIB_Normal_LIBs_Initializing
@LIB_Initialization_OK :
          Print  Libraries_Init_TXT
          Pop    GS
          Pop    FS
          Ret
@LIB_Initialization_Error_01 :
          Mov    EAX,Er_Lib
          Call   Fatal_Error
LIB_Initializing ENDP
;##################################################################################
;# Procedura zjisti, jestli dana knihovna existuje...
;##################################################################################
;# Vstup  : DS:ESI - jmeno jednotky
;# Vystup : C = 1 Then non-exist
;#          EDX    - Cislo jednotky
;##################################################################################
LIB_TestLIBExist  PROC
        Push    ES FS ESI EDI ECX EBX EAX
        CLD
        Push    Data_Descriptor
        Pop     FS
        Mov     ES,Word ptr FS:[LIBNames_Descriptor]
        Mov     EBX,Dword ptr FS:[LIB_NumberOfLibraries]
        Xor     EDI,EDI
@Lib_TestLIBExist_MainLoop :
        Mov     ECX,4
        Mov     EDX,EDI
        Mov     EAX,ESI
        Repz    Cmpsd
        Jz      Short @LIB_Test_LIBExist_Found
        Mov     ESI,EAX
        Mov     EDI,EDX
        Add     EDI,16
        Dec     EBX
        Jnz     Short @Lib_TestLIBExist_MainLoop
        STC
        Jmp     Short @LIB_Test_LIBExist_End
@LIB_Test_LIBExist_Found :
        SHR     EDX,4
        CLC
@LIB_Test_LIBExist_End:
        Pop     EAX EBX ECX EDI ESI FS ES
        Ret
LIB_TestLIBExist  ENDP
;##################################################################################
;# Procedura navraci verzi jednotky
;##################################################################################
;# Vstup  : EDX    - Pozice dat, navraci fce LIB_TestLIBExist
;# Vystup :  AX    - Verze jednotky
;##################################################################################
LIB_Get_Lib_Version PROC
      PUSH   DS EDX
        Mov  AX,Data_Descriptor
        Mov  DS,AX
        Mov  DS,Word ptr DS:[LibInfo_Descriptor]
        Mov  EAX,LMS_Struc_Size
        Mul  EDX
        Mov  AX,Word ptr DS:[EAX.LIB_M_Version]
      POP    EDX DS
        Ret
LIB_Get_Lib_Version ENDP
;##################################################################################
;# Procedura Load_Get_CallGate navrati call gate na knihovnu
;##################################################################################
;# Vstup  : DS:ESI - Jmeno knihovny...
;# Vystup : EDX - Selektor na call gate, if not C
;##################################################################################
LIB_Get_CallGate PROC
        Push    FS ES EDI EBX EAX
        Mov     DX,Data_Descriptor
        Mov     FS,DX
        Call    LIB_TestLIBExist
        Jc      @LIB_Get_CallGate_Error0001
        Mov     EAX,LMS_Struc_Size
        Mul     EDX
        Mov     EDX,EAX
        Mov     EDI,EDX
        Mov     ES,Word ptr FS:[LibInfo_Descriptor]
        Test    Word ptr ES:[EDX.LIB_M_Flags],LIB_CallGateExist
        Jnz     Short @LIB_Get_CallGate_Return
        Mov     BX,Word ptr ES:[EDX.LIB_M_CodeSel]
        PUSH    EDX
         Mov     EDI,LIB_F_CallGate
         Xor     EDX,EDX       ; CNT = 0
         Mov     Dl,0          ; DPL = 3
         Xor     AL,AL         ; alokovat v GDT
         Call    Memory_Manager_Create_CallGate
        POP     EDI
        Or      EAX,EAX
        Jnz     @LIB_Get_CallGate_Error0001
        Or      Word ptr ES:[EDI.LIB_M_Flags],LIB_CallGateExist
        Mov     Word ptr ES:[EDI.LIB_M_CallGate],DX
        Jmp     Short @LIB_Get_CallGate_OK
@LIB_Get_CallGate_Return :
        Mov     DX,Word ptr ES:[EDI.LIB_M_CallGate]
        Jmp     Short @LIB_Get_CallGate_OK
@LIB_Get_CallGate_Error0001 :
        STC
        Jmp     Short @LIB_Get_CallGate_End
@LIB_Get_CallGate_OK :
        CLC
@LIB_Get_CallGate_End :
        Pop     EAX EBX EDI ES FS
        Ret
LIB_Get_CallGate ENDP
;##################################################################################
;# Include Part
;##################################################################################
Include   .\..\inc\code\code32\LIB\vlibs.inc
Include   .\..\inc\code\code32\LIB\nlibs.inc