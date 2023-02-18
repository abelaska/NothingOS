;########################################################################################
;# Procedury pro ziskavani informaci o tascich
;########################################################################################

;############################################################################
;# Procedura navraci cislo aktualniho Tasku
;############################################################################
;# Vstup  : None
;# Vystup : EBX - Cislo tasku
;############################################################################
TASK_Get_Current_Task_Num PROC
      PUSH   DS
        Call  Task_Switch_Disable
        Mov   BX, Data_Descriptor
        Mov   DS,BX
        Mov   EBX,Dword ptr DS:[Current_Task]
        Call  Task_Switch_Enable
      POP    DS
        RET
TASK_Get_Current_Task_Num ENDP
;############################################################################
;# Procedura navraci UID GID daneho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : EAX - UID_GID
;############################################################################
TASK_Get_Task_UID_GID PROC
       PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_UID_GID_End
        Mov   EAX,DWord ptr DS:[EDX.TSK_Info_UID_GID]
        CLC
@TASK_Get_Task_UID_GID_End :
        Call  Task_Switch_Enable
        POP  EDX DS
        RET
TASK_Get_Task_UID_GID ENDP
;############################################################################
;# Procedura navraci UID GID effektivni daneho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : EAX - UID_GID
;############################################################################
TASK_Get_Task_UID_GID_Eff PROC
       PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_UID_GID_EF_End
        Mov   EAX,DWord ptr DS:[EDX.TSK_Info_UID_GID_eff]
        CLC
@TASK_Get_Task_UID_GID_EF_End :
        Call  Task_Switch_Enable
        POP  EDX DS
        RET
TASK_Get_Task_UID_GID_Eff ENDP
;############################################################################
;# Procedura navraci UID GID fylesystemu daneho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : EAX - UID_GID
;############################################################################
TASK_Get_Task_UID_GID_FS PROC
       PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_UID_GID_FS_End
        Mov   EAX,DWord ptr DS:[EDX.TSK_Info_UID_GID_fs]
        CLC
@TASK_Get_Task_UID_GID_FS_End :
        Call  Task_Switch_Enable
        POP  EDX DS
        RET
TASK_Get_Task_UID_GID_FS ENDP
;############################################################################
;# Procedura navraci task Priority
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : AX - Priority
;############################################################################
TASK_Get_Task_Priority PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_Priority_End
        Mov   AX, Word ptr DS:[EDX.TSK_Info_Priority]
        Or    AX, AX
        Jnz   Short @TASK_Get_Task_Priority_Skip
        Mov   AX,1
@TASK_Get_Task_Priority_Skip :
        CLC
@TASK_Get_Task_Priority_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Task_Priority ENDP
;############################################################################
;# Procedura navraci status tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : AX - Priority
;############################################################################
TASK_Get_Task_Status PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_Status_End
        Mov   AX, Word ptr DS:[EDX.TSK_Info_Status]
        Or    AX, AX
        Jnz   Short @TASK_Get_Task_Status_Skip
        Mov   AX,1
@TASK_Get_Task_Status_Skip :
        CLC
@TASK_Get_Task_Status_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Task_Status ENDP
;############################################################################
;# Procedura navraci cislo rodicovskeho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : EBX - Cislo Rodice, if not C
;############################################################################
TASK_Get_Task_Parent PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_Parent_End
        Mov   EAX, dWord ptr DS:[EDX.TSK_Info_ParentTask]
        Or    AX, AX
        Jnz   Short @TASK_Get_Task_Parent_Skip
        Mov   AX,1
@TASK_Get_Task_Parent_Skip :
        CLC
@TASK_Get_Task_Parent_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Task_Parent ENDP
;############################################################################
;# Procedura navraci cislo podrizeneho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : EBX - Cislo Rodice, if not C
;############################################################################
TASK_Get_Task_Child PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_Child_End
        Mov   EAX, dWord ptr DS:[EDX.TSK_Info_ChildTask]
        Or    AX, AX
        Jnz   Short @TASK_Get_Task_Child_Skip
        Mov   AX,1
@TASK_Get_Task_Child_Skip :
        CLC
@TASK_Get_Task_Child_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Task_Child ENDP
;############################################################################
;# Procedura navraci cislo tasku starsiho bratra
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : EBX - Cislo Rodice, if not C
;############################################################################
TASK_Get_Task_OlderBrother PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_OlderBrother_End
        Mov   EAX, dWord ptr DS:[EDX.TSK_Info_OlderBrother]
        Or    AX, AX
        Jnz   Short @TASK_Get_Task_OlderBrother_Skip
        Mov   AX,1
@TASK_Get_Task_OlderBrother_Skip :
        CLC
@TASK_Get_Task_OlderBrother_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Task_OlderBrother ENDP
;############################################################################
;# Procedura navraci cislo tasku mladsiho bratra
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;# Vystup : EBX - Cislo Rodice, if not C
;############################################################################
TASK_Get_Task_YoungerBrother PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Get_Task_YoungerBrother_End
        Mov   EAX, dWord ptr DS:[EDX.TSK_Info_YoungerBrother]
        Or    AX, AX
        Jnz   Short @TASK_Get_Task_YoungerBrother_Skip
        Mov   AX,1
@TASK_Get_Task_YoungerBrother_Skip :
        CLC
@TASK_Get_Task_YoungerBrother_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Task_YoungerBrother ENDP
;############################################################################
;# Procedura navraci pocet existujicich tasku
;############################################################################
;# Vstup  : None
;# Vystup : EAX - pocet tasku
;############################################################################
TASK_Get_Tasks_Count PROC
        PUSH  DS
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   EAX, Dword ptr DS:[Current_Task_Num]
        Call  Task_Switch_Enable
        POP   DS
        RET
TASK_Get_Tasks_Count ENDP
;############################################################################
;# Procedura navraci maximalne mozny pocet tasku
;############################################################################
;# Vstup  : None
;# Vystup : EAX - pocet tasku
;############################################################################
TASK_Get_Max_Tasks PROC
         Mov   EAX,Max_Task
        RET
TASK_Get_Max_Tasks ENDP
;############################################################################
;# Procedura navraci maximalne mozny pocet Zoombie
;############################################################################
;# Vstup  : None
;# Vystup : EAX - pocet tasku
;############################################################################
TASK_Get_Max_Zoombies PROC
        PUSH  DS
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   EAX,Dword ptr DS:[Max_Zoombies]
        Call  Task_Switch_Enable
        POP   DS
        RET
TASK_Get_Max_Zoombies ENDP
;############################################################################
;# Procedura navraci casovou konstantu prepinani tasku
;############################################################################
;# Vstup  : None
;# Vystup : EAX - konstanta
;############################################################################
TASK_Get_Time_Switch_Const PROC
        PUSH  DS
        Call   Task_Switch_Disable
        Mov    AX, Data_Descriptor
        Mov    DS, AX
        Movzx  EAX, word ptr DS:[TSK_Time_Switch_Const]
        Call   Task_Switch_Enable
        POP   DS
TASK_Get_Time_Switch_Const ENDP
;############################################################################
;# Procedura navraci kolik casovych kvant task obdrzel
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - kvantum
;############################################################################
TASK_Get_Time_Used PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     Short @TASK_Get_Time_Used_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Time_Used]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @TASK_Get_Time_Used_End
@TASK_Get_Time_Used_Error :
         STC
@TASK_Get_Time_Used_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Time_Used ENDP
;############################################################################
;# Procedura navraci VFS indodu Stadnardniho vystupu
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS STDOUT indode
;############################################################################
TASK_Get_VFS_STDOUT_Inode PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     Short @TASK_Get_VFS_STDOUT_Inode_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Handle_STDOUT]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @TASK_Get_VFS_STDOUT_Inode_End
@TASK_Get_VFS_STDOUT_Inode_Error :
         STC
@TASK_Get_VFS_STDOUT_Inode_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_STDOUT_Inode ENDP
;############################################################################
;# Procedura navraci VFS indodu Stadnardniho vstupu
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS STDIN indode
;############################################################################
TASK_Get_VFS_STDIN_Inode PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     Short @TASK_Get_VFS_STDIN_Inode_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Handle_STDIN]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @TASK_Get_VFS_STDIN_Inode_End
@TASK_Get_VFS_STDIN_Inode_Error :
         STC
@TASK_Get_VFS_STDIN_Inode_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_STDIN_Inode ENDP
;############################################################################
;# Procedura navraci VFS indodu Stadnardniho erroroveho vystupu
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS STDError indode
;############################################################################
TASK_Get_VFS_STDError_Inode PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     Short @TASK_Get_VFS_STDError_Inode_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Handle_STDERR]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @TASK_Get_VFS_STDError_Inode_End
@TASK_Get_VFS_STDError_Inode_Error :
         STC
@TASK_Get_VFS_STDError_Inode_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_STDError_Inode ENDP
;############################################################################
;# Procedura navraci VFS indodu HOME
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS Home
;############################################################################
TASK_Get_VFS_Home_Inode PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     Short @TASK_Get_VFS_Home_Inode_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Home_VFS_Inode]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @TASK_Get_VFS_Home_Inode_End
@TASK_Get_VFS_Home_Inode_Error :
         STC
@TASK_Get_VFS_Home_Inode_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_Home_Inode ENDP
;############################################################################
;# Procedura navraci VFS indodu TEMP
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS TEMP
;############################################################################
TASK_Get_VFS_Temp_Inode PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     Short @TASK_Get_VFS_Temp_Inode_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Temp_VFS_Inode]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @TASK_Get_VFS_Temp_Inode_End
@TASK_Get_VFS_Temp_Inode_Error :
         STC
@TASK_Get_VFS_Temp_Inode_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_Temp_Inode ENDP
;############################################################################
;# Procedura navraci TSS_Parameters_Length
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - TSS_Parameters_Length
;############################################################################
TASK_Get_Parameters_Length PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Get_Parameters_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Parameters_Length]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Get_Parameters_End
@Get_Parameters_Error :
         XOR    EAX,EAX
         STC
@Get_Parameters_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Parameters_Length ENDP
;############################################################################
;# Procedura navraci Parametry tasku
;############################################################################
;# Vstup  : ES:EDI -> Pozice, kam se prekopiruji parametry
;#          EBX - cislo tasku
;############################################################################
TASK_Get_Parameters PROC
        PUSH  DS EDX ESI EDI ECX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Get_Parameters2_Error
         Mov    DS, EDX
         Mov    ECX, ES
         LSL    ECX,ECX
         Sub    ECX, EDI
         Sub    ECX, Dword ptr DS:[TSS_Parameters_Length]
         Js     @Get_Parameters2_Error2
         Mov    ECX, Dword ptr DS:[TSS_Parameters_Length]
         Mov    ESI, TSS_Parameters
         Rep    Movsb
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Get_Parameters2_End
@Get_Parameters2_Error2 :
         Dispose_Temp_Descriptor DS
@Get_Parameters2_Error :
         STC
@Get_Parameters2_End :
        Call   Task_Switch_Enable
        POP   ECX EDI ESI EDX DS
        RET
TASK_Get_Parameters ENDP
;############################################################################
;# Procedura navraci delku Parameteru naseho tasku
;############################################################################
;# Vstup  : None
;# Vystup : EAX - TSS_Parameters2_Length
;############################################################################
TASK_Get_My_Parameters_Length PROC
        PUSH   EBX
         Call  TASK_Get_Current_Task_Num
         Call  TASK_Get_Parameters_Length
        POP    EBX
        RET
TASK_Get_My_Parameters_Length ENDP
;############################################################################
;# Procedura navraci Parametry
;############################################################################
;# Vstup  : ES:EDI -> Pozice, kam se prekopiruji parametry
;############################################################################
TASK_Get_My_Parameters PROC
        PUSH   EBX
         Call  TASK_Get_Current_Task_Num
         Call  TASK_Get_Parameters
        POP    EBX
        RET
TASK_Get_My_Parameters ENDP
;############################################################################
;# Procedura navraci jmeno tasku
;############################################################################
;# Vstup  : ES:EDI -> Pozice, kam se prekopiruji parametry
;############################################################################
TASK_Get_Task_Name PROC
        PUSH  DS EDX ESI EDI ECX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Get_Task_Name2_Error
         Mov    DS, EDX
         Mov    ECX, ES
         LSL    ECX,ECX
         Sub    ECX, EDI
         Sub    ECX, 32
         Js     @Get_Task_Name2_Error2
         Mov    ECX, 8
         Mov    ESI, TSS_Program_Name
         Rep    Movsd
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Get_Task_Name2_End
@Get_Task_Name2_Error2 :
         Dispose_Temp_Descriptor DS
@Get_Task_Name2_Error :
         STC
@Get_Task_Name2_End :
        Call   Task_Switch_Enable
        POP   ECX EDI ESI EDX DS
        RET
TASK_Get_Task_Name ENDP
;############################################################################
;# Procedura navraci VFS inodu spustitelneho programu tasku
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS inode
;############################################################################
TASK_Get_VFS_Inode PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Get_VFS_Inode_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_VFS_Inode]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Get_VFS_Inode_End
@Get_VFS_Inode_Error :
         XOR    EAX,EAX
         STC
@Get_VFS_Inode_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_Inode ENDP
;############################################################################
;# Procedura navraci VFS inodu domovskeho adresare
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS inode
;############################################################################
TASK_Get_VFS_HOME PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Get_VFS_HOME_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Home_VFS_Inode]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Get_VFS_HOME_End
@Get_VFS_HOME_Error :
         XOR    EAX,EAX
         STC
@Get_VFS_HOME_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_HOME ENDP
;############################################################################
;# Procedura navraci VFS inodu tempovaciho adresare
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS inode
;############################################################################
TASK_Get_VFS_Temp PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Get_VFS_Temp_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Temp_VFS_Inode]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Get_VFS_Temp_End
@Get_VFS_Temp_Error :
         XOR    EAX,EAX
         STC
@Get_VFS_Temp_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_VFS_Temp ENDP
;############################################################################
;# Procedura navraci VFS inodu work adresare
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EAX - VFS inode
;############################################################################
TASK_Get_Current_Dir PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    AX, Data_Descriptor
         Mov    DS, AX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Get_Current_Dir_Error
         Mov    DS, EDX
         Mov    EAX, Dword ptr DS:[TSS_Work_VFS_Inode]
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Get_Current_Dir_End
@Get_Current_Dir_Error :
         XOR    EAX,EAX
         STC
@Get_Current_Dir_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Get_Current_Dir ENDP
;############################################################################
;# Procedura navraci VFS inodu work adresare
;############################################################################
;# Vstup  : -
;# Vystup : EAX - VFS inode
;############################################################################
TASK_Get_My_Current_Dir PROC
        PUSH  EBX
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Current_Dir
        POP   EBX
        RET
TASK_Get_My_Current_Dir ENDP
;############################################################################
;# Procedura navraci jmeno tasku
;############################################################################
;# Vstup  : None
;# Vystup : ES - novy descriptor s datama
;############################################################################
TASK_Get_Processes_Info PROC
        PUSHAD
        PUSH  DS
        Call Task_Switch_Disable
        Mov  EAX,Max_Task
        Mov  EBX,TSK_Info_Size
        Mul  EBX
        Mov  EDX,EAX
        Call TSKMM_Alloc_B
        Jc   @TASK_Get_Processes_Info_err
        Mov  ES,EDX
        Mov  EAX,Data_Descriptor
        Mov  DS,EAX
        Mov  DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Xor  EDI,EDI
        Xor  ESI,ESI
        Mov  ECX,DS
        LSL  ECX,ECX
        Inc  ECX
        Shr  ECX,2
        Rep  Movsd
        CLC
        Jmp  @TASK_Get_Processes_Info_end
  @TASK_Get_Processes_Info_err :
        STC
  @TASK_Get_Processes_Info_end :
        Call Task_Switch_Enable
        POP   DS
        POPAD
        RET
TASK_Get_Processes_Info ENDP