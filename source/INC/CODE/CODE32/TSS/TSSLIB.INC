;##############################################################################
;# Procedury TaskMana pro pouziti v jednotce
;##############################################################################

;##############################################################################
;# Procedura vytvori Task structures
;# Pouze vytvari a inicializuje struktury, navraci TSS a LDT noveho tasku
;##############################################################################
;# EBX a EDX navraceji selektory na LDT a TSS, ale jsou to stale jeste datove
;# descriptory, takze do nich stale muzeme jednoduse zapisovat.
;# Pro zmenu na spravne descriptory zavolat "TSK_Change_Descriptors"
;#
;# Musi se jeste vytvorit struktura pageu...
;#
;# V LDT neni NIC, procedura, ktera vola tuhle musi zajistit nacteni
;# souboru, alokace selektoru do LDT,... atd atd
;#
;# TSS obsahuje standardni inicializacni hodnoty normalnich registeru,
;# musi se v nem nastavit jeste : EIP, CS, DS, FS, GS, ES, SS, ESP, SS0, ESP0
;#
;# Musi se take zavolat fce pro predelani Layout..."TaskMan_ReLayout". Dokud se
;# nezavola, tak se tento nove vytvoreny task nikdy nespusti :)
;##############################################################################
;# Vstup  :    EAX - LO(DW)  Priority - Priorita tasku
;#                   HI(DW)  Status
;#             EBX - UID_GID tasku
;#             ECX - UID_GID efektivni tasku
;#             EDX - UID_GID FS tasku
;#             ESI - DD Parent nebo Brother task
;#             EBP - LO() - Pocet page v LDT
;#                   HI() - RunMode  - Run Mode tasku - 0=child, 1 = brother
;# Vystup : if C then EAX - Error Code
;#             EBX - TSS descriptor
;#             EDX - LDT descriptor
;# Nici   :  EBX,EDX
;##############################################################################
TaskMan_Create_Task_Structure PROC
        PUSH  EBP EAX EDI DS FS GS ES
         ;Incializujeme descriptory
         PUSH    Data_Descriptor
         POP     DS       ;DS = data descriptor
         Mov     FS,Word ptr DS:[TaskMan_TSK_Info_Descriptor] ;FS - info descriptor
         Call    Taskman_Look_For_Free_Task         ;Zjistime, existuje-li ve strukture jeste misto
         Jc      @TaskMan_Create_Task_Error0001
         PUSH    EAX EBX EDX
          Call   Memory_Manager_Allocate_Memory_TSS_LDT
          Or     EAX,EAX
          JNZ    @TaskMan_Create_Task_Error0002
          Mov    ES,BX  ;ES = TSS descriptor
          Mov    GS,DX  ;GS = LDT descriptor
          Xor    EDX,EDX
         Mov     Dword ptr FS:[EDI.TSK_Info_ParentTask],EDX
         Mov     Dword ptr FS:[EDI.TSK_Info_ChildTask],EDX
         Mov     Dword ptr FS:[EDI.TSK_Info_OlderBrother],EDX
         Mov     Dword ptr FS:[EDI.TSK_Info_YoungerBrother],EDX
         POP     EDX EBX EAX
         ;### nastavime hodnoty v info_descriptoru
         Mov     Word ptr  FS:[EDI.TSK_Info_Priority],AX
         Ror     EAX,16
         Mov     Word ptr  FS:[EDI.TSK_Info_Status],AX
         Ror     EAX,16
         Mov     DWord ptr FS:[EDI.TSK_Info_UID_GID],EBX
         Mov     DWord ptr FS:[EDI.TSK_Info_UID_GID_EFF],ECX
         Mov     DWord ptr FS:[EDI.TSK_Info_UID_GID_FS],EDX
         Mov     DWord ptr FS:[EDI.TSK_Info_UID_GID_Save],EDX
         Mov     Word ptr  FS:[EDI.TSK_Info_Selektor],ES
         Mov     EDX,Dword ptr DS:[Last_Task]
         Inc     EDX
         Mov     Dword ptr DS:[Last_Task],EDX
         Mov     Dword ptr FS:[EDI.TSK_Info_ID],EDX
         Inc     Dword ptr DS:[Current_Task_Num]
         Shr     EBP,16
         Or      EBP,EBP
         Jz      Short @TaskMan_Create_Task_Structure_Child
         Xor     EBX,EBX
         Dec     EBX
         Mov     Dword ptr FS:[EDI.TSK_Info_ParentTask],EBX
         PUSH  DS ES
          Push  Data_descriptor
          Pop   DS
          Push  FS
          Pop   ES
          Mov   EDI,EDX
          Call  TaskMan_SetTaskBrother
         POP   ES DS
         Jmp     Short @TaskMan_Create_Task_OK
@TaskMan_Create_Task_Structure_Child:
          Or    ESI,ESI
          Jz    Short @TaskMan_Create_Task_OK
         PUSH  DS ES
          Mov   EAX,Data_Descriptor
          Mov   DS,EAX
          Push  FS
          Pop   ES
          Mov   Dword ptr FS:[EDI.TSK_Info_ParentTask],ESI
          Mov   EAX, ESI
          Mov   EDI,EDX
          Call  Taskman_Find_Task
          Mov   Dword ptr FS:[EDX.TSK_Info_ChildTask],EDI
         POP   ES DS
         Jmp     Short @TaskMan_Create_Task_OK
;####### Error no.2 - no free memory
@TaskMan_Create_Task_Error0002 :
         POP     EDX EBX EAX
         Xor     EAX,EAX
         Add     Al,2
         STC
         Jmp     Short @TaskMan_Create_Task_End
;####### Error no.1 - no free task
@TaskMan_Create_Task_Error0001 :
         Xor     EAX,EAX
         Inc     Al
         STC
         Jmp     Short @TaskMan_Create_Task_End
@TaskMan_Create_Task_OK :
         Xor     EAX,EAX
         Mov     EBX,ES
         Mov     EDX,GS
         CLC
@TaskMan_Create_Task_End :
        POP   ES GS FS DS EDI EAX EBP
        RET
TaskMan_Create_Task_Structure ENDP
;##############################################################################
;# Procedura navraci v EDI cislo tasku, ktery je volny (k pouziti)
;##############################################################################
;# Vstup  : DS  - Data descriptor
;#          FS  - Info descriptor
;# Vystup : EDI - Offset zacatku volneho tasku v Info_Descriptor
;#          C   - 0 - non-error
;#                1 - error
;# Nici   : EDX
;##############################################################################
Taskman_Look_For_Free_Task PROC
        PUSH   ECX
         Mov   ECX,Max_Task
         Cmp   Dword ptr DS:[Current_Task_Num],ECX
         Je    Short @TaskMan_Find_FreeTask_Error
         Xor   EDI,EDI
  @TaskMan_Find_FreeTask_Find :
         Cmp     Dword ptr FS:[EDI.TSK_Info_Struc.TSK_Info_ID],0
         Je      Short @TaskMan_Find_FreeTask_End
         Add     EDI,TSK_Info_Size
         Dec     ECX
         Jnz     Short @TaskMan_Find_FreeTask_Find
 @TaskMan_Find_FreeTask_Error:
         STC
         Jmp     Short @TaskMan_Find_FreeTask_End2
 @TaskMan_Find_FreeTask_End :
         CLC
 @TaskMan_Find_FreeTask_End2 :
        POP    ECX
        RET
Taskman_Look_For_Free_Task ENDP
;##############################################################################
;# Procedura nastavi nektere dulezite promenne do TSS
;##############################################################################
;# Vstup  : ES - Datovy alias na TSS segment
;#          DX - LDT
;# Nici   : Nic
;##############################################################################
TaskMan_InitializeTSS PROC
        PUSH  EDI ECX EAX
         Mov    Word Ptr ES:[TSS_LDT],DX
         Mov    Word ptr ES:[TSS_Struc.TSS_Ports],0FFFFh      ;nastavime konec IO map
         Mov    Word ptr ES:[TSS_Struc.TSS_UnFrm],TSS_UnFormated_Offset  ;nastavime offset IOmap
         Mov    EAX,CR3
         Mov    Dword ptr ES:[TSS_Struc.TSS_CR3],EAX
        POP  EAX ECX EDI
        RET
TaskMan_InitializeTSS ENDP
;##############################################################################
;# Procedura znovu vytvori Layout prepinani tasku
;##############################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;##############################################################################
TaskMan_ReLayout PROC
         PUSHAD
         PUSH   GS ES DS FS
          Call   Task_Switch_Disable
          Mov    AX,Data_Descriptor
          Mov    DS,AX
          Mov    ES,Word ptr DS:[TaskMan_TSK_TSK_Layout_Descriptor]
          Mov    FS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
          Mov    ECX,ES
          LSL    ECX,ECX
          Inc    ECX
          Shr    ECX,2
          Xor    EAX,EAX
          Xor    EDI,EDI
          Rep    Stosd
          ;### ES - Layout descriptor
          ;### FS - Tsk Info Descriptor
          ;### DS - Data segment
          Call   TaskMan_Sort_Tasks
          ;### GS : EDI --> Offset serazenych ID tasku
          ;### GS : ESI --> Offset serazenych priorit tasku
          ;### ECX - Pocet tasku
          ;### EBX - soucet priorit vsech tasku
          Mov    Dword ptr DS:[Current_Layout_Pos],1
          Mov    Dword ptr DS:[Current_Layout_Length],EBX
          Xor    EDX,EDX
          Mov    EAX,EBX
@TaskMan_ReLayout_CreateLayout :
          PUSH   EAX
          Xor    EDX,EDX
          Mov    EBX,dword ptr GS:[ESI]
          Div    EBX
          Shl    EDX,1
          Sub    EDX,EBX     ;je-li > 0 pak je zbytek vetsi nez 0.5
          Js     @TaskMan_Rounding_NoSigned
          Inc    EAX
@TaskMan_Rounding_NoSigned :
          Xor    EDX,EDX
          PUSH   ECX
           Mov    ECX,Dword ptr DS:[Current_Layout_Length]
@TaskMan_Scas_For_Free :
           Cmp    Dword ptr ES:[EDX*4],0
           Jz     Short @TaskMan_Scas_For_Free_Found
           Inc    EDX
           Dec    ECX
           Jnz    @TaskMan_Scas_For_Free
           Mov    ECX,Dword ptr DS:[Current_Layout_Length]
           Xor    EDX,EDX
           Jmp    @TaskMan_Scas_For_Free
@TaskMan_Scas_For_Free_Found :
           Mov    EBP,Dword ptr GS:[EDI]
           Mov    Dword ptr ES:[EDX*4],EBP
           Add    EDX,EAX
           Cmp    EDX,Dword ptr DS:[Current_Layout_Length]
           Jb     Short @TaskMan_Scas_For_Free_Signed
           Sub    EDX,Dword ptr DS:[Current_Layout_Length]
           Mov    ECX,Dword ptr DS:[Current_Layout_Length]
           Sub    ECX,EDX
@TaskMan_Scas_For_Free_Signed :
           Dec    EBX
           Jnz    @TaskMan_Scas_For_Free
          POP    ECX
          POP    EAX
           Add    ESI,4
           Add    EDI,4
           Dec    ECX
           Jnz    @TaskMan_ReLayout_CreateLayout
           Mov    EDX, GS
           Mov    ECX, Data_Descriptor
           Mov    GS, ECX
           Call   Memory_Manager_Deallocate_Memory
           Mov    ECX,Dword ptr DS:[Current_Layout_Length]
           Dec    ECX
           Mov    Dword ptr DS:[Current_Layout_Length],ECX
          Call   Task_Switch_Enable
         POP    FS DS ES GS
         POPAD
         RET
TaskMan_ReLayout ENDP
;##############################################################################
;# Procedura sezaruje cisla tasku tak, aby byli sestupne dle priority
;##############################################################################
;# Vstup  : FS  - TSK info descriptor
;# Vystup : ECX - pocet tasku
;#          EBX - soucet priorit techto tasku
;#          EDI - Offset ID tasku
;#          ESI - Offset priorit tasku
;#          GS  - Pomocny buffer
;# Nici   :
;##############################################################################
TaskMan_Sort_Tasks PROC
       PUSH   EAX EDX EBP
        Mov   ECX, Max_Task
       PUSH  ECX
        Mov   EDX, ECX
        Shr   EDX,9        ;x*(4*2)/4096
        Inc   EDX
        Xor   EAX,EAX
        Call  Memory_Manager_Allocate_Memory
        Jc    @TaskMan_Sort_Tasks_Error
        Mov   GS, EDX
       POP   ESI
        Shl   ESI,2    ;ESI offset v bufferu
       PUSH  ESI
        Xor   EBX,EBX
        Xor   EDI,EDI
        Xor   EDX,EDX
        Xor   EBP,EBP
@TaskMan_Sort_Task_Check_Task:
        Cmp   Word ptr FS:[EDX.TSK_Info_Status],_TSK_Status_RUN
        Je    Short @TaskMan_Sort_Task_Task_OK
        Cmp   Word ptr FS:[EDX.TSK_Info_Status],_TSK_Forever_Run
        Jne   Short @TaskMan_Sort_Task_Skip_This_Task
@TaskMan_Sort_Task_Task_OK :
        Inc   EBX
        Movzx EAX, Word ptr FS:[EDX.TSK_Info_Priority]
        Add   EBP, EAX
        Mov   Dword ptr GS:[ESI],EAX
        Add   ESI,4
        Mov   EAX, Dword ptr FS:[EDX.TSK_Info_ID]
        Mov   Dword ptr GS:[EDI],EAX
        Add   EDI,4
@TaskMan_Sort_Task_Skip_This_Task :
        Add   EDX,TSK_Info_Size
        Dec   ECX
        Jnz   @TaskMan_Sort_Task_Check_Task
       POP   ESI
;# momentalne mame v GS dve struktury
;#   GS:0    -> seznam id tasku
;#   GS:ESI  -> priority tasku
;# budeme tridit pole GS:ESI, pocet zaznamu je v EBX, podle GS:ESI budem presouvat i GS:0
       Or    EBX, EBX
       Jz    @TaskMan_Sort_Tasks_Error
       PUSH  ESI
       PUSH  EBX
       Mov   ECX, EBX
       Xor   EDI,EDI
@Sort_MainLoop :
       Mov   EAX, DWord ptr GS:[ESI]
       Mov   EBX, ECX
       Dec   EBX
       Shl   EBX,2
@Sort_InterLoop :
       Cmp   EAX, DWord ptr GS:[ESI+EBX]
       Jb    Short @Sort_InterLoop_Skip
       Xchg  DWord ptr GS:[EBX+ESI], EAX
       Mov   DWord ptr GS:[ESI], EAX
       Mov   EDX, Dword ptr GS:[EDI]
       Push  Dword ptr GS:[EDI+EBX]
       Mov   Dword ptr GS:[EDI+EBX],EDX
       Pop   Dword ptr GS:[EDI]
@Sort_InterLoop_Skip :
       Sub   EBX,4
       Jns   Short @Sort_InterLoop
       Add   ESI,4
       Add   EDI,4
       Dec   ECX
       Jnz   Short @Sort_MainLoop
       POP   ECX
       POP   ESI
       Xor   EDI,EDI
       Mov   EBX,EBP
       POP    EBP EDX EAX
       STC
        RET
@TaskMan_Sort_Tasks_Error :
       POP    EBP EDX EAX
       CLC
        RET
TaskMan_Sort_Tasks ENDP
;############################################################################
;# Procedura zakaze prepinani tasku
;############################################################################
Task_Switch_Disable PROC
         PUSHFD
         PUSH   DS EAX
          Mov    EAX,Data_Descriptor
          Mov    DS,EAX
          Inc    Byte ptr DS:[Task_Switching_Disabled]
         POP    EAX DS
         POPFD
        RET
Task_Switch_Disable ENDP
;############################################################################
;# Procedura zakaze prepinani tasku
;############################################################################
Task_Switch_Enable PROC
         PUSHFD
         PUSH   DS EAX
          Mov    EAX,Data_Descriptor
          Mov    DS,EAX
          Dec    Byte ptr DS:[Task_Switching_Disabled]
         POP    EAX DS
         POPFD
        RET
Task_Switch_Enable ENDP
;############################################################################
;# Procedura povoli prepinani tasku natvrdo
;############################################################################
Task_Switch_Enable_X PROC
         PUSHFD
         PUSH   DS EAX
          Mov    EAX,Data_Descriptor
          Mov    DS,EAX
          Mov    Byte ptr DS:[Task_Switching_Disabled],00h
         POP    EAX DS
         POPFD
        RET
Task_Switch_Enable_X ENDP
;############################################################################
;# Procedura nastavi task jako brother
;# Vstup :  ES      - Task structures
;#          DS      - Data segment kernelu
;#          EDI     - cislo tasku
;#          ESI     - task starsiho bratra
;############################################################################
TaskMan_SetTaskBrother PROC
       PUSH  ESI EAX EDX
        Call  Task_Switch_Disable
           Mov    EAX,ESI
           Call   Taskman_Find_Task
           Mov    EAX,Dword ptr ES:[EDX.TSK_Info_YoungerBrother]
           Or     EAX,EAX
           Jz     Short @TaskMan_SetTaskBrother_Found
           Mov    ESI,EAX
           Call   TaskMan_SetTaskBrother
@TaskMan_SetTaskBrother_Skip :
        Call  Task_Switch_Enable
       POP  EDX EAX ESI
        RET
@TaskMan_SetTaskBrother_Found :
           Mov    Dword ptr ES:[EDX.TSK_Info_YoungerBrother],EDI
           Mov    EAX, EDI
           Call   Taskman_Find_Task
           Mov    Dword ptr ES:[EDX.TSK_Info_OlderBrother],ESI
        Call  Task_Switch_Enable
       POP  EDX EAX ESI
        RET
TaskMan_SetTaskBrother EndP
;############################################################################
;# Procedura nastavuje UID GID daneho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;#          ECX - UID_GID
;############################################################################
TASK_Set_Task_UID_GID PROC
       PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   DX, Data_Descriptor
        Mov   DS, DX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Set_Task_UID_GID_End
        Mov   DWord ptr DS:[EDX.TSK_Info_UID_GID],ECX
        CLC
@TASK_Set_Task_UID_GID_End :
        Call  Task_Switch_Enable
        POP  EDX DS
        RET
TASK_Set_Task_UID_GID ENDP
;############################################################################
;# Procedura nastavuje UID GID effektivni daneho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;#          ECX - UID_GID
;############################################################################
TASK_Set_Task_UID_GID_Eff PROC
       PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   DX, Data_Descriptor
        Mov   DS, DX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Set_Task_UID_GID_eff_End
        Mov   DWord ptr DS:[EDX.TSK_Info_UID_GID_eff],ECX
        CLC
@TASK_Set_Task_UID_GID_Eff_End :
        Call  Task_Switch_Enable
        POP  EDX DS
        RET
TASK_Set_Task_UID_GID_Eff ENDP
;############################################################################
;# Procedura nastavuje UID GID fylesystemu daneho tasku
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;#          ECX - UID_GID
;############################################################################
TASK_Set_Task_UID_GID_FS PROC
       PUSH   DS EDX
        Call  Task_Switch_Disable
        Mov   DX, Data_Descriptor
        Mov   DS, DX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Set_Task_UID_GID_fs_End
        Mov   DWord ptr DS:[EDX.TSK_Info_UID_GID_fs],ECX
        CLC
@TASK_Set_Task_UID_GID_fs_End :
        Call  Task_Switch_Enable
        POP  EDX DS
        RET
TASK_Set_Task_UID_GID_FS ENDP
;############################################################################
;# Procedura nastavuje task Priority
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;#          CX - Priority
;############################################################################
TASK_Set_Task_Priority PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   DX, Data_Descriptor
        Mov   DS, DX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Set_Task_Priority_End
        Mov   Word ptr DS:[EDX.TSK_Info_Priority],CX
        CLC
@TASK_Set_Task_Priority_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Set_Task_Priority ENDP
;############################################################################
;# Procedura nastavuje task Status
;############################################################################
;# Vstup  : EBX - Cislo Tasku
;#          CX  - Status
;############################################################################
TASK_Set_Task_Status PROC
        PUSH  DS EDX
        Call  Task_Switch_Disable
        Mov   DX, Data_Descriptor
        Mov   DS, DX
        Mov   DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
        Call  Task_Return_TaskInfo_Rec_Offset
        Jc    @TASK_Set_Task_Status_End
        Mov   Word ptr DS:[EDX.TSK_Info_Status],CX
        CLC
@TASK_Set_Task_Status_End :
        Call  Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Set_Task_Status ENDP
;############################################################################
;# Procedura prepne task okamzite - napriklad hodi-li se nekdo do sleepu,
;# ceka-li na klavesnici atd...
;############################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;############################################################################
Task_Switch_Now PROC
         INT  0F0H
        RET
Task_Switch_Now ENDP
;############################################################################
;# Procedura navraci Offsetovou hodnotu zaznamu v TaskMan_TSK_Info_Descriptor
;# pro dany task
;############################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : EDX - offset
;############################################################################
Task_Return_TaskInfo_Rec_Offset PROC
      PUSH     DS ES ECX
        Call  Task_Switch_Disable
       Mov     EDX, Data_Descriptor
       Mov     DS, EDX
       Mov     ES, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
       Mov     ECX, Max_Task
       Xor     EDX,EDX
@Task_Find_Task_Find :
       Cmp     Dword ptr ES:[EDX.TSK_Info_ID],EBX
       Je      @Task_Find_Task_End
       Add     EDX,TSK_Info_Size
       Dec     ECX
       Jnz     Short @Task_Find_Task_Find
       Xor     EDX,EDX
        Call  Task_Switch_Enable
       STC
      POP      ECX ES DS
        RET
@Task_Find_Task_End :
        Call  Task_Switch_Enable
       CLC
      POP      ECX ES DS
        RET
Task_Return_TaskInfo_Rec_Offset Endp
;############################################################################
;# Procedura spocita Zoombie, a je-li jich vice nezli male mnozstvi :), tak
;# nejakou vyhodi
;############################################################################
;# Vstup  : None
;# Vystup : None
;############################################################################
Task_Reduce_Zoombie PROC
      PUSHAD
      PUSH DS ES EDI
        Call  Task_Switch_Disable
       Mov     EDX, Data_Descriptor
       Mov     DS, EDX
       Mov     ES, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
       Mov     ECX, Max_Task
       Xor     EDX,EDX
       Xor     EBX,EBX
       Xor     EAX,EAX
       Dec     EAX
       Mov     EDI,EAX
@Task_Flush_Zoombie_Find :
       Cmp     Dword ptr ES:[EDX.TSK_Info_Status],_TSK_Zoombie
       Je      @Task_Flush_Zoombie_Found
@Task_Flush_Zoombie_Found_ret:
       Add     EDX,TSK_Info_Size
       Dec     ECX
       Jnz     Short @Task_Flush_Zoombie_Find
       Cmp     EBX,Dword ptr DS:[Max_Zoombies]
       Jbe     Short @Task_Flush_Zoombie_No_Zoombies
       Cmp     EAX,0FFFFFFFFh
       Je      Short @Task_Flush_Zoombie_No_Zoombies
       Call    TaskMan_Kill_Zoombie
       Call    TaskMan_ReLayout
@Task_Flush_Zoombie_no_Zoombies :
        Call  Task_Switch_Enable
      POP      EDI ES DS
      POPAD
        RET
@Task_Flush_Zoombie_Found :
    Inc   EBX
    Cmp   EDI, Dword ptr ES:[EDX.TSK_Info_Variable]
    Jb    @Task_Flush_Zoombie_Found_Ret
    Mov   EDI, Dword ptr ES:[EDX.TSK_Info_Variable]
    Mov   EAX, Dword ptr ES:[EDX.TSK_INFO_ID]
    Jmp   @Task_Flush_Zoombie_Found_ret
Task_Reduce_Zoombie  ENDP
;############################################################################
;# Procedura zabije zoombie
;############################################################################
;# Vstup : EAX - Cislo tasku k usmrceni
;############################################################################
TaskMan_Kill_Zoombie PROC
         PUSHAD
         PUSH    DS FS ES
         Call    Task_Switch_Disable
         PUSH    EAX
          Mov     EBX,EAX
          Mov     AX,Data_Descriptor
          Mov     DS,AX
          Mov     DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
          Call    Task_Return_TaskInfo_Rec_Offset
         POP     EBX
         Mov     EDI, EDX
         Movzx   EDX, word ptr DS:[EDI.TSK_Info_Selektor]
         Call    Memory_Manager_Create_Alias
         Jc      @TaskMan_Kill_Task_Cant_Kill_Root
         Mov     FS,EDX
         Mov     EAX, Dword ptr DS:[EDI.TSK_Info_Struc.TSK_Info_ParentTask]
         Or      EAX, EAX
         Jz      @TaskMan_Kill_Task_Cant_Kill_Root
     ;# nejprve musime sloucit rodice s detmi
         Call    TaskMan_Expand_Parents_Child
     ;# potom jeste sloucime bratry s bratrama
         Call    TaskMan_Expand_brothers
     ;# propojime jeste rodinu - bratri k dedeckum a tak dale...
          Mov    EBX, Dword ptr DS:[EDI.TSK_Info_YoungerBrother]
          Mov    ECX, Dword ptr DS:[EDI.TSK_Info_OlderBrother]
          Mov    EAX, Dword ptr DS:[EDI.TSK_Info_ParentTask]
          Cmp    EAX, 0FFFFFFFFh
          Je     Short TaskMan_Kill_Task_Try_Child
          Call   TaskMan_Implant_Task_Into_Brothers
          Jmp    short TaskMan_Kill_Task_Family_Connected
     TaskMan_Kill_Task_Try_Child :
          Mov    EAX, Dword ptr DS:[EDI.TSK_Info_ChildTask]
          OR     EAX, EAX
          Jz     short TaskMan_Kill_Task_Family_Connected
          Call   TaskMan_Implant_Task_Into_Brothers
     TaskMan_Kill_Task_Family_Connected :
     ;# dealokovat pamet
     ;# EAX - cr3, EDX - LDT, FS - alias na TSS deallokovaneho tasku, GS - nas TSS
           Call   TASK_Get_Current_Task_Num
           Call   Task_Return_TaskInfo_Rec_Offset
           Movzx  EDX, word ptr DS:[EDX.TSK_Info_Selektor]
           PUSH   EDI
            Call   Memory_Manager_Create_Alias
            Jc     @TaskMan_Kill_Task_Cant_Kill
            Mov    GS, EDX
            Mov    EAX, Dword ptr FS:[TSS_CR3]
            Movzx  EDX, Word ptr FS:[TSS_LDT]
            SLDT   SI
            LLDT   DX
            Mov    Word ptr  GS:[TSS_LDT],DX
            Mov    Dword ptr GS:[TSS_CR3],EAX
            Mov    ECX,CR3
            Mov    CR3,EAX
            PUSH   ECX
            PUSH   ESI
            Mov    EAX,VMem_Private_Area_Size
            Shr    EAX,12
            ;EAX = pocet 4k bloku, ktere ma task k dispozici
            PUSH   ES
            Mov    DX,4
            Mov    ES,DX
            Xor    EBX,EBX
            Mov    ECX,Dword ptr ES:[EBX.TMA_Block_Size]
            Sub    EAX,ECX
  @TaskMan_Kill_TaskDeallocate :
           Add    EBX,TMA_Size
           Mov    ECX,Dword ptr ES:[EBX.TMA_Block_Size]
           Sub    EAX,ECX
           Movzx  EDX,Word ptr ES:[TMA_Selector]
           Or     EDX,EDX
           Jz     Short @TaskMan_Kill_TaskDeallocate_Skip
           Call   TSKMM_Dealloc
   @TaskMan_Kill_TaskDeallocate_Skip :
           Cmp    EAX,0
           Jne    @TaskMan_Kill_TaskDeallocate
           POP    ES
           POP    ESI
           POP    ECX
           SLDT   BX
           Mov    EAX,CR3
           LLDT   SI
           Mov    Word ptr  GS:[TSS_LDT],SI
           Mov    Dword ptr GS:[TSS_CR3],ECX
           Mov    CR3,ECX
           Dispose_Temp_Descriptor GS
           ;Zrusime stare CR3
           Call   Create_Descriptor_GDT_Baze
           Call   Memory_Manager_Deallocate_Memory
           ;Zrusime LDT
           Movzx  EDX,BX
           Call   Memory_Manager_Deallocate_Memory
          POP     EDI
           ;Zrusime TSS
           Movzx  EDX,Word ptr DS:[EDI.TSK_Info_Selektor]
           Call   Memory_Manager_Deallocate_Memory
           Jmp    @TaskMan_Kill_Task_Deallocate_OK
     @TaskMan_Kill_Task_Cant_Kill :
     ;# vynulujeme zaznam o dannem tasku
          POP    EDI
     @TaskMan_Kill_Task_Deallocate_OK :
          Push   DS
          Push   DS
          Pop    ES
          Mov    ECX,TSK_Info_Size
          Xor    Al,Al
          Rep    Stosb
          POP    DS
          Mov   AX,Data_descriptor
          Mov   DS,AX
          Dec   Dword ptr DS:[Current_Task_Num]
     ;# nemuzeme zabit nekoho, kdo ma za rodice nikoho...
     @TaskMan_Kill_Task_Cant_Kill_Root :
         Dispose_Temp_Descriptor FS
         Call    Task_Switch_Enable
         POP     ES FS DS
         POPAD
         RET
TaskMan_Kill_Zoombie ENDP
;########################################################################################
;# Procedura udela expanzi rodicu na deti rodicu tak, ze
;# proste a jasne, rodicovsky task tasku A se stane rodicovskym taskem
;# podrizeneho tasku tasku A, a podrizenym taskem rodicovskeho tasku tasku A
;# se stane podrizeny task tasku A. :)+<
;########################################################################################
;#   Vstup  : EBX - task na expanzi
;#            DS  - TaskMan_TSK_Info_Descriptor
;########################################################################################
TaskMan_Expand_Parents_Child PROC
      PUSH  EAX ESI EDX EBX
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    ESI, EDX
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_ParentTask]
            Or     EBX, EBX
            Jz     Short TaskMan_Move_Parents_Child_Skip1
            Cmp    EBX,0FFFFFFFFh
            Je     Short TaskMan_Move_Parents_Child_Skip1
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    EAX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_ChildTask]
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_ChildTask],EAX
      TaskMan_Move_Parents_Child_Skip1 :
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_ChildTask]
            Or     EBX, EBX
            Jz     TaskMan_Move_Parents_Child_Skip2
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    EAX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_ParentTask]
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_ParentTask],EAX
       TaskMan_Move_Parents_Child_Skip2 :
      POP   EBX EDX ESI EAX
          RET
TaskMan_Expand_Parents_Child ENDP
;########################################################################################
;# Procedura udela expanzi bratru
;# princip je stejny jako expanze rodicu na deti :)
;########################################################################################
;#   Vstup  : EBX - task na expanzi
;#            DS  - TaskMan_TSK_Info_Descriptor
;########################################################################################
TaskMan_Expand_Brothers PROC
      PUSH  EAX ESI EDX EBX
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    ESI, EDX
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_OlderBrother]
            Or     EBX, EBX
            Jz     TaskMan_Expand_Brothers_Skip1
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    EAX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_YoungerBrother]
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_YoungerBrother],EAX
       TaskMan_Expand_Brothers_Skip1:
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_YoungerBrother]
            Or     EBX, EBX
            Jz     Short TaskMan_Expand_Brothers_Skip2
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    EAX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_OlderBrother]
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_OlderBrother],EAX
       TaskMan_Expand_Brothers_Skip2 :
      POP   EBX EDX ESI EAX
          RET
TaskMan_Expand_Brothers ENDP
;########################################################################################
;# Procedura udela implantuje strukturu tasku mezi struktury tasku
;########################################################################################
;#   Vstup  : EBX - task mladsiho bratra
;#            EAX - task na implantovani
;#            ECX - task starsiho bratra
;#            DS  - TaskMan_TSK_Info_Descriptor
;########################################################################################
TaskMan_Implant_Task_Into_Brothers PROC
      PUSH  ECX EAX ESI EDX EBX
            Or     ECX,ECX
            Jnz    @TaskMan_Implant_Task_Into_Brothers_Cont1
            PUSH   EBX
             Call   Task_Return_TaskInfo_Rec_Offset
             Mov    ESI,EDX
             Mov    EBX,EAX
         @TaskMan_Implant_Task_Into_Brothers_Look_For_Y1 :
             Mov    ECX,EBX
             Call   Task_Return_TaskInfo_Rec_Offset
             Mov    EBX,Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_YoungerBrother]
             Or     EBX,EBX
             Jnz    Short @TaskMan_Implant_Task_Into_Brothers_Look_For_Y1
            POP     Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_YoungerBrother]
             Mov    Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_OlderBrother],ECX
             Jmp    @TaskMan_Implant_Task_Into_Brothers_Skip2

         @TaskMan_Implant_Task_Into_Brothers_Cont1 :
            Or     EBX,EBX
            Jnz    @TaskMan_Implant_Task_Into_Brothers_Cont
             Mov    EBX, ECX
             Call   Task_Return_TaskInfo_Rec_Offset
             Mov    ESI, EDX
             Mov    EBX, EAX
         @TaskMan_Implant_Task_Into_Brothers_Look_For_Oldest1 :
             Mov    EAX,EBX
             Call   Task_Return_TaskInfo_Rec_Offset
             Mov    EBX,Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_OlderBrother]
             Or     EBX,EBX
             Jnz    Short @TaskMan_Implant_Task_Into_Brothers_Look_For_Oldest1
             Call   Task_Return_TaskInfo_Rec_Offset
             Mov    Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_YoungerBrother],EAX
             Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_OlderBrother],ECX
             Jmp    @TaskMan_Implant_Task_Into_Brothers_Skip2
         @TaskMan_Implant_Task_Into_Brothers_Cont :
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    ESI, EDX
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_OlderBrother]
            Or     EBX, EBX
            Jz     @TaskMan_Implant_Task_Into_Brothers_Skip1
            Mov    EBX, EAX
         @TaskMan_Implant_Task_Into_Brothers_Look_For_Oldest :
            Mov    ECX,EBX
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    EBX,Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_OlderBrother]
            Or     EBX,EBX
            Jnz    Short @TaskMan_Implant_Task_Into_Brothers_Look_For_Oldest
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_OlderBrother]
            Or     EBX,EBX
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_OlderBrother],EBX
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_YoungerBrother],ECX
          @TaskMan_Implant_Task_Into_Brothers_Skip1 :
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_YoungerBrother]
            Or     EBX, EBX
            Jz     @TaskMan_Implant_Task_Into_Brothers_Skip2
            Mov    EBX, EAX
         @TaskMan_Implant_Task_Into_Brothers_Look_For_Youngies :
            Mov    ECX,EBX
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    EBX,Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_YoungerBrother]
            Or     EBX,EBX
            Jnz    Short @TaskMan_Implant_Task_Into_Brothers_Look_For_Youngies
            Mov    EBX, Dword ptr DS:[ESI.TSK_Info_Struc.TSK_Info_ID]
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_YoungerBrother],EBX
            Call   Task_Return_TaskInfo_Rec_Offset
            Mov    Dword ptr DS:[EDX.TSK_Info_Struc.TSK_Info_OlderBrother],ECX
          @TaskMan_Implant_Task_Into_Brothers_Skip2 :
      POP   EBX EDX ESI EAX ECX
          RET
TaskMan_Implant_Task_Into_Brothers ENDP
;################################################################################
;# Procedura zabije prave bezici task - respective prepne ho do stavu zoombie
;################################################################################
;# Vstup  : None
;# Vystup : None
;################################################################################
TASK_Kill_Me PROC
     PUSH   EBX
     Call   TASK_Get_Current_Task_Num
     Call   TASK_Kill_Task
     POP    EBX
     Ret
TASK_Kill_Me ENDP
;################################################################################
;# Procedura zabije task
;################################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : None
;################################################################################
TASK_Kill_Task PROC
    PUSHAD
    PUSH   DS
     PUSH   EBX
      Call   TASK_Switch_Disable
      Mov    AX,Data_Descriptor
      Mov    DS,AX
      Mov    DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
     POP   EBX
      Call   Task_Return_TaskInfo_Rec_Offset
      Cmp    Word ptr DS:[EDX.TSK_Info_Status],_TSK_Forever_Run
      Je     @TASK_Kill_Task_SkipIt
      Call   TASK_Get_Youngiest_Zoombie
      Mov    Dword ptr DS:[EDX.TSK_Info_Variable],EAX
      Mov    CX,_TSK_Zoombie
      Call   TASK_Set_Task_Status
  ;# byl-li rodic spinkavy, necht zase ozije
    @TASK_Kill_Task_Find_Parrent :
     Mov    EBX, Dword ptr DS:[EDX.TSK_Info_ParentTask]
     Cmp    EBX,0FFFFFFFFh
     Je     Short @TASK_Kill_Task_Skip_Status_Changing
     Call   Task_Return_TaskInfo_Rec_Offset
     Cmp    Word ptr DS:[EDX.TSK_Info_Status],_TSK_Zoombie
     Je     @TASK_Kill_Task_Find_Parrent
     Mov     Word ptr DS:[EDX.TSK_Info_Status],_TSK_Status_RUN
    @TASK_Kill_Task_Skip_Status_Changing :
     Call   Task_Reduce_Zoombie
     Call   TaskMan_ReLayout
     Call   TASK_Switch_Enable
     Call   TASK_Switch_Now
    @TASK_Kill_Task_SkipIt :
    POP  DS
    POPAD
     Ret
TASK_Kill_Task ENDP
;############################################################################
;# Navraci variable cislo nejmladsi Zoombie + 1
;############################################################################
;# Vstup  : None
;# Vystup : EAX = variable nejmladsi zoombie + 1
;############################################################################
TASK_Get_Youngiest_Zoombie PROC
     PUSH   DS EDX ECX
      Call   TASK_Switch_Disable
      Mov    AX,Data_Descriptor
      Mov    DS,AX
      Mov    DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
      Xor    EDX,EDX
      Mov    ECX,Max_Task
      Xor    EAX,EAX
      Dec    EAX
    @TASK_Get_Youngiest_Zoombie :
      Cmp    Word ptr DS:[EDX.TSK_Info_Status],_TSK_Zoombie
      Jne    @TASK_Get_Youngiest_Zoombie_Skip
      Cmp    EAX, DWord ptr DS:[EDX.TSK_Info_Variable]
      Jb     @TASK_Get_Youngiest_Zoombie_Skip
      Mov    EAX, DWord ptr DS:[EDX.TSK_Info_Variable]
    @TASK_Get_Youngiest_Zoombie_Skip :
      Add    EDX,TSK_Info_Size
      Dec    ECX
      Jnz    @TASK_Get_Youngiest_Zoombie
      Call   TASK_Switch_Enable
      Inc    EAX
     POP     ECX EDX DS
     Ret
TASK_Get_Youngiest_Zoombie ENDP
;############################################################################
;# Procedura nastavuje casovou konstantu prepinani tasku
;############################################################################
;# Vstup  : CX - konstanta
;# Vystup : None
;############################################################################
TASK_Set_Time_Switch_Const PROC
        PUSH  DS EDX
        Call   Task_Switch_Disable
        Mov    DX, Data_Descriptor
        Mov    DS, DX
        Mov    Word ptr DS:[TSK_Time_Switch_Const],CX
        Call   Task_Switch_Enable
        POP   EDX DS
TASK_Set_Time_Switch_Const ENDP
;############################################################################
;# Procedura nastavuje maximalne mozny pocet Zoombie
;############################################################################
;# Vstup  : EDX - Maximalni pocet zoombie
;############################################################################
TASK_Set_Max_Zoombies PROC
        PUSH  DS EAX
        Call  Task_Switch_Disable
        Mov   AX, Data_Descriptor
        Mov   DS, AX
        Mov   Dword ptr DS:[Max_Zoombies],EDX
        Call  Task_Switch_Enable
        POP   EAX DS
        RET
TASK_Set_Max_Zoombies ENDP
;##############################################################################
;# Procedura ulozi do TSS cestu a parametry spusteni
;##############################################################################
;# Vstup  : ES - Datovy alias na TSS segment
;#          DS:ESI -pchar cesta, pchar parametry
;##############################################################################
TaskMan_SaveParams PROC
        PUSHAD
          Mov   ECX,DS
          LSL   ECX,ECX
          Sub   ECX,ESI
          Xor   EDX,EDX
          Mov   EDI,Offset TSS_Parameters
   TaskMan_FindParmas :
          Lodsb
          Or    Al,AL
          Jz    TaskMan_Params
          Dec   ECX
          Jnz   TaskMan_FindParmas
          Jmp   TaskMan_SaveParams_Error
   TaskMan_Params :
          Lodsb
          Or    Al,AL
          Jz    TaskMan_Params_End
          Cmp   EDI,TSK_Params_End
          Ja    TaskMan_Params_End
          Stosb
          Inc   EDX
          Dec   ECX
          Jnz   TaskMan_Params
   TaskMan_Params_End :
          Mov   Dword ptr ES:[TSS_Parameters_Length],EDX
          Jmp   Short TaskMan_SaveParams_End
   TaskMan_SaveParams_Error :
          Mov   Dword ptr ES:[TSS_Parameters_Length],EDX
   TaskMan_SaveParams_End :
        POPAD
        RET
TaskMan_SaveParams ENDP
;################################################################################
;# Procedura uspi prave bezici task
;################################################################################
;# Vstup  : None
;# Vystup : None
;################################################################################
TASK_Sleep_Me PROC
     PUSH   EBX ECX
      Call   TASK_Get_Current_Task_Num
      Call   TASK_Sleep_Task
      Call   Task_Switch_Now
     POP    ECX EBX
     Ret
TASK_Sleep_Me ENDP
;################################################################################
;# Procedura uspi task
;################################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : None
;################################################################################
TASK_Sleep_Task PROC
     PUSH   EDX EAX EBX ECX DS
       Call   Task_Switch_Disable
       Mov    AX,Data_Descriptor
       Mov    DS,AX
       Mov    DS,Word ptr DS:[TaskMan_TSK_Info_Descriptor]
       Xor    EDX,EDX
       Mov    ECX,Max_Task
       Xor    EAX,EAX
    @TASK_Get_Num_Of_RunTask   :
       Cmp    Word ptr DS:[EDX.TSK_Info_Status],_TSK_Status_Run
       Jne    @TASK_Get_Num_Of_RunTask_NotInc
       Inc    EAX
    @TASK_Get_Num_Of_RunTask_NotInc :
       Add    EDX,TSK_Info_Size
       Dec    ECX
       Jnz    @TASK_Get_Num_Of_RunTask
      Cmp    EAX,2
      Jb     TASK_Sleep_Task_Error
      Mov    ECX,_TSK_Status_SLEEP
      Call   TASK_Set_Task_Status
    TASK_Sleep_Task_Error :
       Call   Task_Switch_Enable
     POP    DS ECX EBX EAX EDX
     Ret
TASK_Sleep_Task ENDP
;################################################################################
;# Procedura ozivi task
;################################################################################
;# Vstup  : EBX - cislo tasku
;# Vystup : None
;################################################################################
Task_Wakeup_Task PROC
     PUSH  ECX
      Mov    ECX,_TSK_Status_Run
      Call   TASK_Set_Task_Status
      Call   TaskMan_Relayout
     POP   ECX
      Ret
Task_Wakeup_Task ENDP
;############################################################################
;# Procedura nastavuje VFS inodu domovskeho adresare
;############################################################################
;# Vstup  : EBX - cislo tasku
;#          EAX - VFS inode
;############################################################################
TASK_Set_VFS_HOME PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    DX, Data_Descriptor
         Mov    DS, DX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Set_VFS_HOME_Error
         Mov    DS, EDX
         Mov    Dword ptr DS:[TSS_Home_VFS_Inode],EAX
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Set_VFS_HOME_End
@Set_VFS_HOME_Error :
         XOR    EAX,EAX
         STC
@Set_VFS_HOME_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Set_VFS_HOME ENDP
;############################################################################
;# Procedura nastavuje VFS inodu tempovaciho adresare
;############################################################################
;# Vstup  : EBX - cislo tasku
;#          EAX - VFS inode
;############################################################################
TASK_Set_VFS_Temp PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    DX, Data_Descriptor
         Mov    DS, DX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Set_VFS_Temp_Error
         Mov    DS, EDX
         Mov    Dword ptr DS:[TSS_Temp_VFS_Inode],EAX
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Set_VFS_Temp_End
@Set_VFS_Temp_Error :
         XOR    EAX,EAX
         STC
@Set_VFS_Temp_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Set_VFS_Temp ENDP
;############################################################################
;# Procedura nastavuje VFS inodu tempovaciho adresare
;############################################################################
;# Vstup  : EBX - cislo tasku
;#          EAX - VFS inode
;############################################################################
TASK_Set_Current_Dir PROC
        PUSH  DS EDX
        Call    Task_Switch_Disable
         Mov    DX, Data_Descriptor
         Mov    DS, DX
         Mov    DS, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Call   Task_Return_TaskInfo_Rec_Offset
         Movzx  EDX,Word ptr DS:[EDX.TSK_Info_Selektor]
         Call   Memory_Manager_Create_Alias
         Jc     @Set_Current_Dir_Error
         Mov    DS, EDX
         Mov    Dword ptr DS:[TSS_Work_VFS_Inode],EAX
         Dispose_Temp_Descriptor DS
         CLC
         Jmp    Short @Set_Current_Dir_End
@Set_Current_Dir_Error :
         XOR    EAX,EAX
         STC
@Set_Current_Dir_End :
        Call   Task_Switch_Enable
        POP   EDX DS
        RET
TASK_Set_Current_Dir ENDP
;############################################################################
;# Procedura nastavuje VFS inodu tempovaciho adresare
;############################################################################
;# Vstup  :  EAX - VFS inode
;############################################################################
TASK_Set_My_Current_Dir PROC
        PUSH EBX
           Call   TASK_Get_Current_Task_Num
           Call   TASK_Set_Current_Dir
        POP  EBX
        RET
TASK_Set_My_Current_Dir ENDP
;############################################################################
;# Procedura prepne na danny task
;############################################################################
;# Vstup  : EBX - cislo tasku, na ktery mame prepnout
;############################################################################
TASK_Switch_2_Task PROC
        PUSHAD
        PUSH  DS ES
        Call Task_Switch_Disable
        Mov  eax, Data_Descriptor
        Mov  DS,EAX
        Mov  ECX,Dword ptr DS:[Current_Layout_Length]
        Mov  ES,Word ptr DS:[TaskMan_TSK_TSK_Layout_Descriptor]
        Mov  EDX,0
@TASK_Switch_2_Task_Find :
        Cmp  Dword ptr ES:[EDX],EBX
        Je   @TASK_Switch_2_Task_Found
        Add  EDX,4
        Dec  ECX
        Jnz  @TASK_Switch_2_Task_Find
        Jmp  @TASK_Switch_2_Task_Error
@TASK_Switch_2_Task_Found :
        Or   EDX,EDX
        Jnz  @TASK_Switch_2_Task_Next
        Mov  EDX,Dword ptr DS:[Current_Layout_Length]
@TASK_Switch_2_Task_Next :
        Dec  EDX
        Mov  Dword ptr DS:[Current_Layout_Pos],EDX
        Call Task_Switch_Enable
        Call Task_Switch_Now
@TASK_Switch_2_Task_Error :
        POP  ES DS
        POPAD
        RET
TASK_Switch_2_Task ENDP