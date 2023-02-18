;##########################################################################################
;# Procedury pro spousteni programu
;##########################################################################################

;##########################################################################################
;# Procedura spusti program tak, ze natvrdo nastavi urcite parametry EXECu -
;# - z duvodu bezpecnosti. Tuto proceduru budou volat normalni programy pri
;# spousteni jinych. Prave spousteny program dostane stejnou prioritu, UID GID
;# status jako jeho predek.
;##########################################################################################
;# Vstup : (DS:ESI) - Pchar cesta k souboru
;#         EBX - VFS Inode STD_IN
;#         ECX - VFS Inode STD_OUT
;#         EDI - VFS Inode STD_Error
;#          DX - Run Mode
;##########################################################################################
EXEC_Restricted  PROC
     Call   Task_Switch_Disable
    PUSH   ES EDI
     PUSH  EDI
      MOV   EDI,Data_Descriptor
      Mov   ES,EDI
      Mov   EDI,Offset EXEC_Temp_Struc
      Mov   Dword ptr ES:[EDI.EXEC_VFS_Std_In], EBX
      Mov   Dword ptr ES:[EDI.EXEC_VFS_Std_OUT],ECX
     POP   Dword ptr ES:[EDI.EXEC_VFS_Std_Err]
     Call  TASK_Get_Current_Task_Num
     Call  TASK_Get_VFS_Home
     Mov   Dword ptr ES:[EDI.EXEC_VFS_Home],EAX
     Call  TASK_Get_VFS_Temp
     Mov   Dword ptr ES:[EDI.EXEC_VFS_TEMP],EAX
     Call  Task_Get_Current_Dir
     Mov   Dword ptr ES:[EDI.EXEC_VFS_Work],EAX
     Mov   word ptr  ES:[EDI.EXEC_Run_Mode],DX
     Call  TASK_Get_Current_Task_Num
     Call  TASK_Get_Task_UID_GID
     Mov   Dword ptr ES:[EDI.EXEC_UID_GID],EAX
     Call  TASK_Get_Task_UID_GID_Eff
     Mov   Dword ptr ES:[EDI.EXEC_UID_GID_Eff],EAX
     Call  TASK_Get_Task_UID_GID_FS
     Mov   Dword ptr ES:[EDI.EXEC_UID_GID_FS],EAX
     Call  TASK_Get_Task_Priority
     Mov   Word ptr ES:[EDI.EXEC_Priority],AX
     Mov   Word ptr ES:[EDI.EXEC_Run_Status],_TSK_Status_RUN
     Mov   Word ptr ES:[EDI.EXEC_CPLx],3
     Call  EXEC_CPL0
    POP EDI ES
     Call  Task_Switch_Enable_X
     Ret
EXEC_Restricted  ENDP
;##########################################################################################
;# Procedura spusti program s parametrama urcenyma v ES:EDI
;# Tato procedura je pristupna pouze pro Roota
;##########################################################################################
;# Vstup  : (DS:ESI) - Pchar cesta k souboru
;#          (ES:EDI) - EXEC_Structure
;# Vystup : EAX - Error Code
;#          EBX - Extedned information
;# EAX         EBX          Vysledek
;# 00000000    cokoliv       OK
;# 00000001    cokoliv       Chyba pri alokaci pameti
;# 00000002    cokoliv       Format neni podporavan zadnou jednotkou
;# 00000003    drv_num       Format je podporavan driverem drv_num, pri vytvareni struktur vsak driver nahlasil chybu
;# 00000004    cokoliv       Neni dostatek descriptoru pro spusteni
;# 00000005    cokoliv       soubor na spusteni nenalezen
;# 0001xxxx    drv_num       Format je podporovan driverem drv_num, neexistuje vsak jednotka s cislem xxxx
;# 0003xxxx    drv_num       Format je podporavan driverem drv_num, jednotka xxxx vsak nema dostatecnou verzi
;# 0007xxxx    drv_num       Format je podporavan driverem drv_num, jednotka xxxx je mapovana za hranici data descriptoru
;##########################################################################################
EXEC_CPL0 PROC
     PUSH EBP EDI ESI EDX ECX DS ES FS GS
          Call   Open_File_For_Exec
          Jnc    @EXEC_File_Mapping_OK
          Mov    EAX,5
          Jmp    @EXEC_CPL0_END_Total
      @EXEC_File_Mapping_OK :
          Call   Task_Switch_Disable               ;vypneme prepnuti tasku
          CLI
          ;zmenime stack
          Mov    Ax,SS
          Mov    GS,AX
          Mov    EAX,Stack_Descriptor_Helper
          Mov    SS,  EAX
          Mov    EAX, ESP
          Mov    ESP,0FFFFFFFFh
          Push   GS
          Push   EAX
          Mov    AX,Data_Descriptor
          Mov    GS,EAX
          ;EDX = memory maped file
          ;EBX = VFS indode
          Mov    Dword ptr ES:[EDI.EXEC_VFS],EBX
        PUSH  EDX ;EDX = virtual memory maped file
          Mov    ECX,DRV_Exec_Drivers_Res
          Mov    EBX,DRV_Exec_Check_Fce
          Mov    EAX,DRV_Exec_Drivers
@EXEC_CPL0_Try2Find_Exec_Driver : ;pokousime se najit driver, ktery by byl ochotny nejak zpracovat nas program
          PUSH   EAX ECX
          Call   Drv_GATE
          Or     EAX,EAX
          Jz     @EXEC_CPL0_Try2Find_Exec_Driver_End
          Test   EAX,EXEC_Driver_Lib_Error
          Jnz    Short @EXEC_CPL0_Try2Find_Exec_Driver_NotLib
          POP    ECX EAX
          Add    EAX,DRV_Exec_AddOfs
          Dec    ECX
          Jnz    Short @EXEC_CPL0_Try2Find_Exec_Driver
          Xor    EAX,EAX
          Mov    Al,2
          Jmp    Short @EXEC_CPL0_Try2Find_Exec_Driver_NotSucces
@EXEC_CPL0_Try2Find_Exec_Driver_NotLib :
          POP    EBX EBX
@EXEC_CPL0_Try2Find_Exec_Driver_NotSucces:
        POP      EDX
          PUSH   EAX
           Mov    EAX,Dword ptr ES:[EDI.EXEC_VFS]
           Call   VFS_Close
          POP    EAX
          Jmp    @EXEC_CPL0_END
@EXEC_CPL0_Try2Find_Exec_Driver_End :
        POP      EBP
          Mov    EBP,ECX                           ;EBP = pocek 4k page ktere musi byt v LDT
          Call   TASK_Get_Current_Task_Num         ;zjistime svoje ID cislo
          PUSH   ESI
           Mov    ESI,EBX
           Ror    EBP,16
           Mov    BP,  Word ptr  ES:[EDI.EXEC_Run_Mode]
           Ror    EBP,16
           Mov    AX,  Word ptr  ES:[EDI.EXEC_Run_Status]
           Shl    EAX,16
           Mov    AX,  Word ptr  ES:[EDI.EXEC_Priority]
           Mov    EBX, Dword ptr ES:[EDI.EXEC_UID_GID]
           Mov    ECX, Dword ptr ES:[EDI.EXEC_UID_GID_Eff]
           Mov    EDX, Dword ptr ES:[EDI.EXEC_UID_GID_FS]
           Call   TaskMan_Create_Task_Structure  ;# vytvorime struktury TSS a LDT
          POP    ESI
          JC     @EXEC_CPL0_Error
          PUSH   ES
           Mov    ES,EBX
           Call   TaskMan_SaveParams   ;# do TSS ulozime parametry spusteni
          POP    ES
          Call   MemMan_Create_CR3     ;# vytvorime struktury CR3
          JC     @EXEC_CPL0_Error
        POP      EBP      ;# EBP - Driver
        POP      ECX      ;# ECX - virtual maped file
        PUSH   DS FS EBX EDX
          Mov    DS ,EBX  ;# DS  - TSS Alias descriptor
          Mov    EBX,ECX  ;# EBX - Selektor na descriptor souboru
          PUSH   EAX
           Mov   AX,LDT_Descriptor_Type
           Call  Memory_Manager_Change2System_Descr
          POP    EAX
          PUSH   ESI
           PUSH  EDX
            STR  DX
            Call Memory_Manager_Create_Alias
            Mov  FS,EDX
           POP   EDX
           SLDT   SI
           LLDT   DX
           Mov    Word ptr FS:[TSS_LDT],DX
           Mov    ECX,CR3
           Mov    DWord ptr FS:[TSS_CR3],EAX
           Mov    CR3,EAX
           PUSH   ES
            Push  DS
            Pop   ES
            Call   TaskMan_InitializeTSS
           POP    ES
           Mov     EDX, EBX
           Call    TSKMM_Inicialize_LDT
           Jnc     short @EXEC_CPL0_Exec_LDT_Initialized
           POP     ESI
           Jmp     @EXEC_CPL0_Exec_Error_NoDescriptors
        @EXEC_CPL0_Exec_LDT_Initialized :
           Mov    EBX,DRV_Exec_Exec_Fce
           Mov    EAX,EBP
           PUSH   ESI ECX
             Call   Drv_GATE
           POP    ECX ESI
           Mov    Word ptr FS:[TSS_LDT],SI
           Mov    DWord ptr FS:[TSS_CR3],ECX
           LLDT   SI
           Mov    EBX, CR3
           Mov    CR3,ECX
           Dispose_Temp_Descriptor FS
          POP     ESI
          Or     EAX,EAX
          Jnz    @EXEC_CPL0_Exec_Error
        POP    EDX EBX FS DS
        Call   TSK_Change_Descriptors              ;zmenime descriptory
        POP    EAX
        POP    EBX
        Mov    SS,EBX
        Mov    ESP,EAX
        Mov    AX,word ptr ES:[EDI.EXEC_Run_Mode]
        Cmp    AX,EXEC_Run_Mode_Brother
        Je     @EXEC_CPL0_RunMode_Brother
        ;Task byl spusten jako dite
        Call   TASK_Get_Current_Task_Num
        Mov    CX,_TSK_Status_STOP
        Call   TASK_Set_Task_Status     ;zastavime tento task
        Call   TaskMan_ReLayout         ;predelame tabulku rozlozeni tasku
        Call   Task_Switch_Enable_X     ;povolime swichnuti tasku
        STI
        Call   Task_Switch_Now          ;a prepneme task
        ;# momentalne je tento task zastaven. Jakmile se task, ktery jsme spustily
        ;# ukonci, vratime se zpatky na toto misto :)
        XOR    EAX,EAX
        Jmp    Short @EXEC_CPL0_END_Total
@EXEC_CPL0_RunMode_Brother :
        ;Task byl spusten jako bratricek
        Call   TaskMan_ReLayout         ;predelame tabulku rozlozeni tasku
        Call   Task_Switch_Enable_X
        STI
        Jmp    Short @EXEC_CPL0_END_Total
@EXEC_CPL0_END   :
        POP    ECX
        POP    EDX
        Mov    SS,EDX
        Mov    ESP,ECX
        Call   Task_Switch_Enable_X
        STI
@EXEC_CPL0_END_Total :
     POP  GS FS ES DS ECX EDX ESI EDI EBP
      Ret
;##############################################################################
;# Errors
;##############################################################################
@EXEC_CPL0_Exec_Error_NoDescriptors :
;sem jeste pridat zruseni zaznamu o soucasnem tasku, vytvorene procedurou
;TaskMan_Create_Task_Structure - proste staci zrusit posledni zaznam, protoze
;je zakazano prepnuti tasku, tudiz posledni zaznam jsme jasne my :)
        POP    EAX EAX
        Call   PAGE_Set_Page_As_Free
        Mov    EAX,Dword ptr ES:[EDI.EXEC_VFS]
        Call   VFS_Close
        Mov    EDX,DS
        Pop    DS
        Call   Memory_Manager_Deallocate_Memory
        Mov    EDX,FS
        Pop    FS
        Call   Memory_Manager_Deallocate_Memory
        Xor    EAX,EAX
        Mov    Al,4
        Mov    EBX,EBP
        Jmp    Short  @EXEC_CPL0_END
@EXEC_CPL0_Exec_Error :
        POP    EAX EAX
        ;EBX je stara CR3 struktura
        Call   PAGE_Set_Page_As_Free
        Mov    EAX,Dword ptr ES:[EDI.EXEC_VFS]
        Call   VFS_Close
        Mov    EDX,DS
        Pop    DS
        Call   Memory_Manager_Deallocate_Memory
        Mov    EDX,FS
        Pop    FS
        Call   Memory_Manager_Deallocate_Memory
        Xor    EAX,EAX
        Mov    Al,3
        Mov    EBX,EBP
        Jmp    @EXEC_CPL0_END
@EXEC_CPL0_Error :
        POP    EDX                        ;EDX je memory maped soubor
        Mov    EAX,Dword ptr ES:[EDI.EXEC_VFS]
        Call   VFS_Close
        Xor    EAX,EAX
        Inc    Al
        Jmp    @EXEC_CPL0_END
EXEC_CPL0 ENDP
;##########################################################################################
;# Exec - Return Lib name
;##########################################################################################
;# Vstup :  CX     -  cislo jednotky
;#          DS:ESI -  Ukazatel na soubor
;#          ES:EDI -  16b buffer kam se jmeno ulozi
;#          EBP    -  cislo driveru
;# Vystup : EAX    =  0 -> OK
;##########################################################################################
EXEC_Ret_Libs_Name PROC
          PUSH  EBX EDX
           Call  Open_File_For_Exec
           Jc    EXEC_Ret_Libs_Name_err
           Mov   EAX, EBP
           PUSH  EBX EDX
            Mov   EBX, 012h
            Call  Drv_Gate
           POP   EDX EBX
           PUSH  EAX
            Mov   EAX, EBX
            Call  VFS_Close
           POP   EAX
  EXEC_Ret_Libs_Name_end :
          POP   EDX EBX
        Ret
  EXEC_Ret_Libs_Name_err :
        Mov    EAX,1
        Jmp    Short EXEC_Ret_Libs_Name_end
EXEC_Ret_Libs_Name ENDP
;##########################################################################################
;# Exec - Return Lib version
;##########################################################################################
;# Vstup :  CX     -  cislo jednotky
;#          DS:ESI -  Ukazatel na soubor
;#          ES:EDI -  16b buffer kam se jmeno ulozi
;#          EBP    -  cislo driveru
;# Vystup : EAX    -  verze knihovny
;##########################################################################################
EXEC_Ret_Libs_Version PROC
          PUSH  EBX EDX
           Call  Open_File_For_Exec
           Jc    EXEC_Ret_Libs_Version_err
           Mov   EAX, EBP
           PUSH  EBX EDX
            Mov   EBX, 013h
            Call  Drv_Gate
           POP   EDX EBX
           PUSH  EAX
            Mov   EAX, EBX
            Call  VFS_Close
           POP   EAX
  EXEC_Ret_Libs_Version_end :
          POP   EDX EBX
        Ret
  EXEC_Ret_Libs_Version_err :
        Mov    EAX,1
        Jmp    Short EXEC_Ret_Libs_Version_end
EXEC_Ret_Libs_Version ENDP
;##########################################################################################
;# Procedura otevre soubor pro spusteni
;##########################################################################################
;# Vstup  : DS:ESI - Pchar jmeno souboru
;# Vystup : If C Then Error
;#          EDX = Selektor
;#          EBX = VFS inoda
;##########################################################################################
Open_File_For_Exec PROC
       PUSH  DS ESI EAX ECX
         Mov   EAX,1
         Call  VFS_Open
         Jc    Open_File_For_Exec_End
         Call  VFS_Is_Inode_File
         Jnc   Open_File_For_Exec_Cont
         Call  VFS_Close
         Stc
         Jmp   Open_File_For_Exec_End
   Open_File_For_Exec_Cont :
         PUSH  EAX
         Xor   EBX,EBX
         Xor   ECX,ECX
         Dec   ECX
         Call  VFS_Map_File
         POP   EBX
   Open_File_For_Exec_End :
       POP   ECX EAX ESI DS
        Ret
Open_File_For_Exec ENDP

