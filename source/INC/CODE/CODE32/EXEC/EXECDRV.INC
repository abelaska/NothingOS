;###########################################################################
;# Code segment
;###########################################################################
EXEC_Code:
EXEC_Sel_Data  dw      0           ; selector na datasegment daneho driveru
;###########################################################################
;# Inicializacni procedura
;###########################################################################
EXEC_INIT PROC
    RetF
EXEC_INIT ENDP
;###########################################################################
;# Deinicializacni procedure
;###########################################################################
EXEC_DONE PROC
    RetF
EXEC_DONE ENDP
;###########################################################################
;# Procedura navraci informace o tom, je-ji schopna zpracovat dany soubor
;# Vstup  :  EDX - Selektor na descriptor souboru
;# Vystup :  EAX - 00000000 - OK
;#               - 0000FFFF - Neni muj typ
;#               - 0001xxxx - Jednotka xxxx neexistuje
;#               - 0003xxxx - Jednotka xxxx nema dostatecnou verzi
;#               - 0007xxxx - Jednotka xxxx je mapovana za hranici DS segmentu
;#           ECX - LDT size
;###########################################################################
EXEC_Check_Structures PROC
    PUSH   DS GS EBX EDI EDX ESI
      ;# Provedeme test signatury
      Mov    GS,EDX
      Mov    EAX,Dword ptr GS:[EBF_Signature]
      Cmp    EAX,EBF_Signature_Equ
      Jne    @EXEC_Check_Structures_Error
      Mov    EAX,Dword ptr GS:[EBF_Code_Header_Size]
      SHL    EAX,12
      Cmp    EAX,Dword ptr GS:[EBF_Code_Entry_Point]
      Jb     @EXEC_Check_Structures_Error        ;vstupni  bod za hranici Code segmentu
      Mov    ECX,Dword ptr GS:[EBF_Header_Size]
      Xor    EDI,EDI
      Xor    EAX,EAX
      Xor    EBX,EBX
      ;# Provedeme prepocteni CRCcka
    @EXEC_Check_Structures_Count_CRC :
      Mov    BL,Byte ptr GS:[EDI]
      Inc    EDI
      Add    EAX,EBX
      Dec    ECX
      Jnz    Short @EXEC_Check_Structures_Count_CRC
      Cmp    EAX,Dword ptr GS:[EDI]
      Jne    Short @EXEC_Check_Structures_Error
       ;# Pokusime se nacist knihovny
      Mov    ECX,Dword ptr GS:[EBF_LibsNum]
      Or     ECX,ECX
      Jz     @EXEC_Check_Structures_OK
      PUSH   DS
        PUSH   GS
        POP    DS
        Mov    ESI,EBF_Libs
        Mov    EDI, Dword ptr GS:[EBF_Data_Size]
        Shl    EDI,12
        Sub    EDI,2
        Xor    EBX,EBX
    @EXEC_Check_Structures_Check_Libs:
        Call   LIB_TestLIBExist
        Jc     Short @EXEC_Check_Structures_Error_Lib
        Call   LIB_Get_Lib_Version
        Cmp    AX,Word ptr GS:[ESI.EBF_LS_Lib_MinVer]
        Jb     @EXEC_Check_Structures_Error_Lib_Ver
        Mov    EAX, Dword ptr GS:[ESI.EBF_LS_Lib_CallGate]
        Cmp    EDI, EAX
        Jb     @EXEC_Check_Structures_Error_Lib_CallGate
        Add    ESI,EBF_LSH_Size
        Inc    EBX
        Dec    ECX
        Jnz    Short @EXEC_Check_Structures_Check_Libs
      POP    DS
      Jmp    Short @EXEC_Check_Structures_OK
@EXEC_Check_Structures_Error :
      Xor    EAX,EAX
      Dec    AX ;Error code 0000FFFFh - neni muj format
      Jmp    Short @EXEC_Check_Structures_End
@EXEC_Check_Structures_Error_Lib_CallGate :
      POP    DS
      Mov    EAX,00070000h
      Mov    AX,BX
      Jmp    Short @EXEC_Check_Structures_End
@EXEC_Check_Structures_Error_Lib_Ver :
      POP    DS
      Mov    EAX,00030000h
      Mov    AX,BX
      Jmp    Short @EXEC_Check_Structures_End
@EXEC_Check_Structures_Error_Lib :
      POP    DS
      Mov    EAX,00010000h
      Mov    AX,BX ;Error code 01xxxxh - knihovna cislo xxxxx neexistuje
      Jmp    Short @EXEC_Check_Structures_End
@EXEC_Check_Structures_OK :
      Xor    EAX,EAX
      Mov    ECX,Dword ptr GS:[EBF_LibsNum]
      Shl    ECX,1       ;jeden call gate a jeden datovy descriptor na jednotku
      Add    ECX,511     ;jeden task bude mit moznost alokovat si az 511 descriptoru
      Shr    Ecx,9       ;nasobime velikosti descriptoru a delime 4k
      Inc    ECX         ;ECX je pocet 4k page, ktere bude muset LDTcko obsahovat
@EXEC_Check_Structures_End :
    POP      ESI EDX EDI EBX GS DS
    RetF
EXEC_Check_Structures ENDP
EXEC_CodeEnd:
;###########################################################################
;# Procedura se snazi vytvorit struktury tasku
;###########################################################################
;# Vstup  : DS - TSS Alias descriptor
;#          (ES:EDI) - EXEC block
;#          EDX - Selektor na descriptor
;#          CR3 a LDT nastavene :)
;# Vystup : EAX = 0 Then OK else Error
;###########################################################################
EXEC_EXEC_File PROC
        PUSH   EBX ECX EDX ESI EDI GS FS
        Mov      FS, EDX
        Cmp      Word ptr ES:[EDI.EXEC_CPLx],0
        Je       @EXEC_EXEC_File_VivaLaBooh
    ;Task je spousten jako radovy vojacek, cpl3
    ;zkopirujeme ukazatele na stdin, out a error, home a temp adr
        Mov      EAX, Dword ptr ES:[EDI.EXEC_VFS_Std_OUT]
        Mov      Dword ptr DS:[TSS_Handle_STDOUT],EAX
        Mov      EAX, Dword ptr ES:[EDI.EXEC_VFS_Std_In]
        Mov      Dword ptr DS:[TSS_Handle_STDIN],EAX
        Mov      EAX, Dword ptr ES:[EDI.EXEC_VFS_Std_Err]
        Mov      Dword ptr DS:[TSS_Handle_STDERR],EAX
        Mov      EAX, Dword ptr ES:[EDI.EXEC_VFS_Home]
        Mov      Dword ptr DS:[TSS_Home_VFS_Inode],EAX
        Mov      EAX, Dword ptr ES:[EDI.EXEC_VFS_Temp]
        Mov      Dword ptr DS:[TSS_Temp_VFS_Inode],EAX
        Mov      EAX, Dword ptr ES:[EDI.EXEC_VFS_Work]
        Mov      Dword ptr DS:[TSS_Work_VFS_Inode],EAX
        Mov      EAX, Dword ptr ES:[EDI.EXEC_VFS]
        Mov      Dword ptr DS:[TSS_VFS_Inode],EAX
        Mov      ECX,8
        Xor      EDX,EDX
@EXEC_EXEC_File_Move_Name :
        Mov      EAX, Dword ptr FS:[EBF_Name+EDX]
        Mov      Dword ptr DS:[TSS_Program_Name+EDX],EAX
        Add      EDX,4
        Dec      ECX
        Jnz      @EXEC_EXEC_File_Move_Name
   ;Vytvorime Code descriptor
        Mov      EDX,FS
        Call     MemMan_ReturnDescriptorBaseAddress
        Mov      ECX, Dword ptr FS:[EBF_Code_Header_Size]
        Call     TSKMM_CreateDescriptor_LDT
        Call     TSKMM_Memory_Manager_Data2Code_Descr_LDT
        PUSH     EDX
    ;Vytvorime zasobnik
        Mov      EDX, Dword ptr FS:[EBF_Stack_Size]
        Mov      Dword ptr DS:[TSS_Stack_Memory],EDX
        Call     TSKMM_Alloc
        Jnc      Short @EXEC_EXEC_File_Stack_Allocation_OK
@EXEC_EXEC_File_Stack_Allocation_Error :
        POP      EDX
        Call     TSKMM_DeAlloc
        Jmp      @EXEC_EXEC_File_Error
@EXEC_EXEC_File_Stack_Allocation_OK :
        Call     TSKMM_Memory_Manager_Data2Stack_Descr_LDT
        Jc       Short @EXEC_EXEC_File_Stack_Allocation_Error
        PUSH     EDX
   ; Vytvorime zasobnik pro CPL0
        Mov      EDX,CPL0_Stack_Size ;CPL0 zasobnik bude mit konstantne velikou velikost
        Call     TSKMM_Alloc
        Jnc      Short @EXEC_EXEC_File_Stack_CPL0_Allocation_OK
@EXEC_EXEC_File_Stack_CPL0_Allocation_Error :
        POP      EDX
        Call     TSKMM_Dealloc
        POP      EDX
        Call     TSKMM_Dealloc
        Jmp      @EXEC_EXEC_File_Error
@EXEC_EXEC_File_Stack_CPL0_Allocation_OK :
        Xor      EAX,EAX
        Call     TSKMM_Memory_Manager_ChangeDPL_Descr_LDT
        Jc       Short @EXEC_EXEC_File_Stack_CPL0_Allocation_Error
        Call     TSKMM_Memory_Manager_Data2Stack_Descr_LDT
        Jc       Short @EXEC_EXEC_File_Stack_CPL0_Allocation_Error
        PUSH     EDX
   ; alokujeme pamet pro datovy descriptor
        Mov      EDX, Dword ptr FS:[EBF_Data_Size]
        Call     TSKMM_Alloc
        Jnc      Short EXEC_EXEC_Data_Alloc_OK
        POP      EDX
        Call     TSKMM_Dealloc
        POP      EDX
        Call     TSKMM_Dealloc
        POP      EDX
        Call     TSKMM_Dealloc
        Jmp      @EXEC_EXEC_File_Error
EXEC_EXEC_Data_Alloc_OK :
        PUSH      EDX
   ; zkopirujeme datovy segment
        PUSH      ES DS
         PUSH      FS
         POP       DS
         Mov       ES,EDX
         LSL       ECX,EDX
         Inc       ECX
         Shr       ECX,2
         Xor       EDI,EDI
         Mov       ESI,Dword ptr FS:[EBF_Code_Header_Size]
         Shl       ESI,12
         Rep       MovsD
        POP       DS ES
    ; na zasobniku jsou v poradi DS(cpl3),SS(cpl0), SS(cpl) a Code(cpl3)
        POP       EAX
        Mov       DWord ptr DS:[TSS_DS],EAX
        Mov       DWord ptr DS:[TSS_FS],EAX
        Mov       DWord ptr DS:[TSS_GS],EAX
        Mov       DWord ptr DS:[TSS_ES],EAX
        POP       DWord ptr DS:[TSS_CPL0_SS]
        POP       DWord ptr DS:[TSS_SS]
        POP       DWord ptr DS:[TSS_CS]
        Mov       EAX, Dword ptr FS:[EBF_Code_Entry_Point]
        Mov       Dword ptr DS:[TSS_EIP],EAX
        PUSHFD
        POP       EAX
        Or        Ah,2
        Mov       Dword ptr DS:[TSS_EEFLAGS],EAX
        XOR       EAX,EAX
        Mov       Dword ptr DS:[TSS_EAX],EAX
        Mov       Dword ptr DS:[TSS_EBX],EAX
        Mov       Dword ptr DS:[TSS_ECX],EAX
        Mov       Dword ptr DS:[TSS_EDX],EAX
        Mov       Dword ptr DS:[TSS_EBP],EAX
        Mov       Dword ptr DS:[TSS_EDI],EAX
        Mov       Dword ptr DS:[TSS_ESI],EAX
        Dec       EAX
        Mov       Dword ptr DS:[TSS_ESP],EAX
        Mov       Dword ptr DS:[TSS_CPL0_ESP],EAX
        Mov       EAX,CR3
        Mov       Dword ptr DS:[TSS_CR3],EAX
        SLDT      AX
        Mov       Word ptr DS:[TSS_LDT],AX
    ;Ulozime pocet knihoven, inicializujeme je
        Mov       ECX, Dword ptr FS:[EBF_LibsNum]
        Mov       Dword ptr DS:[TSS_Libs_Count],ECX
        Or        ECX,ECX
        Jz        Short @EXEC_EXEC_Skip_Init_Libs
        Mov       ESI,EBF_Libs
@EXEC_EXEC_Init_Libs :
         PUSH   DS
         PUSH   FS
         POP    DS
          Call      LIB_Get_CallGate
         POP    DS
         Jc        @EXEC_EXEC_File_Error_2
         Mov       EDI, Dword ptr FS:[ESI.EBF_LS_Lib_CallGate]
         PUSH      ES
          Mov       ES,DWord ptr DS:[TSS_DS]
          Mov       Dword ptr ES:[EDI],EDX
         POP       ES
         PUSHAD
          PUSH      EDX
          PUSH      EDX
          Mov       EAX,0FFFFFFFFh
          Call      fword ptr SS:[ESP]
          Add       ESP,8
         POPAD
         Add       ESI,EBF_LSH_Size
         Dec       ECX
         Jnz       Short @EXEC_EXEC_Init_Libs
@EXEC_EXEC_Skip_Init_Libs :
    ;Ulozime stav koprocesoru
        FSAVE     DS:[TSS_Coprocesor_Register]
    ;Nastavime datum a cas vytvoreni procesu
        Call      CMOS_Packed_Date
        Mov       Dword ptr DS:[TSS_Create_Time.DT_Date],EAX
        Call      CMOS_Packed_Time
        Mov       Dword ptr DS:[TSS_Create_Time.DT_Time],EAX
    ;Inicializace probehla uplne v pohode, koncime... :)
        Jmp       @EXEC_EXEC_File_OK
@EXEC_EXEC_File_VivaLaBooh :
    ;Task je spousten jako CPL0
@EXEC_EXEC_File_OK :
    Xor    EAX,EAX
    POP    FS GS EDI ESI EDX ECX EBX
    RetF
@EXEC_EXEC_File_Error_2 :
    POP    DS
@EXEC_EXEC_File_Error :
    Mov    EAX,1
    POP    FS GS EDI ESI EDX ECX EBX
    RetF
EXEC_EXEC_File ENDP
;###########################################################################
;# Procedura navraci jmeno jednotky v souboru
;# Vstup :  CX  - cislo jednotky
;#          EDX - Selektor na descriptor souboru
;#          ES:EDI - 16b buffer kam se jmeno ulozi
;# Vystup : EAX = 0 OK
;###########################################################################
EXEC_Return_Lib_Name PROC
     PUSH    DS ECX ESI EDI EDX
      Mov    DS,EDX
      Movzx  ECX,CX
      Cmp    ECX,Dword ptr DS:[EBF_LibsNum]
      Ja     Short @EXEC_Return_Lib_Name_Err
      Mov    ESI,EBF_Libs
      Mov    EAX,EBF_LSH_Size
      Mul    ECX
      Add    ESI,EAX
      Mov    ECX,4
      Rep    Movsd
      Xor    EAX,EAX
      Jmp    Short @EXEC_Return_Lib_Name_End
@EXEC_Return_Lib_Name_Err :
      Mov    EAX,1
@EXEC_Return_Lib_Name_End :
     POP     EDX EDI ESI ECX DS
     RetF
EXEC_Return_Lib_Name ENDP
;###########################################################################
;# Procedura navraci verzi jednotky v souboru
;# Vstup : CX  - cislo jednotky
;#         EDX - Selektor na descriptor
;#         EAX - Verze jednotky
;###########################################################################
EXEC_Return_Lib_Version PROC
     PUSH    DS ECX ESI EDI EDX
      Mov    DS,EDX
      Movzx  ECX,CX
      Cmp    ECX,Dword ptr DS:[EBF_LibsNum]
      Ja     Short @EXEC_Return_Lib_Version_Err
      Mov    ESI,EBF_Libs
      Mov    EAX,EBF_LSH_Size
      Mul    ECX
      Add    ESI,EAX
      Movzx  EAX, Word ptr DS:[ESI.EBF_LS_Lib_MinVer]
      Jmp    Short @EXEC_Return_Lib_Version_End
@EXEC_Return_Lib_Version_Err :
      Xor    EAX,EAX
@EXEC_Return_Lib_Version_End :
     POP     EDX EDI ESI ECX DS
     RetF
     RetF
EXEC_Return_Lib_Version ENDP


EXEC_CODE_Size equ (((EXEC_CodeEnd - EXEC_Code-1) Shr 12)+1)

