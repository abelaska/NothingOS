;############################################################################################
;# Task Manager interrupt
;############################################################################################

;############################################################################################
;# Hlavni prerusovaci interrupt
;############################################################################################
Task_Manager_interrupt :
     @TaskMan_Interrupt_Entrance_Code :
         Mov    Ax, Data_Descriptor
         Mov    DS, AX
         Mov    AX, Word ptr DS:[TaskMan_TSK_Info_Descriptor]
         Mov    ES, AX
         Mov    AX, Word ptr DS:[TaskMan_TSK_TSK_Layout_Descriptor]          ;Layout tasku je pole obsahujici cisla tasku v poradi prepinani
         Mov    FS, AX
         ;GS = alias pro taskmanovsky TSS
         Mov    Ax, Word ptr DS:[TaskMan_TSS_Alias_Sel]
         Mov    GS, AX
@TaskMan_Interrupt_Entrance_Code_Inter :
           Inc    Dword ptr DS:[System_Timer]                 ;Zvysime pocitadlo pruchodu
           Dec    Dword ptr DS:[Timer_Hook_ATA]                ;snizime pocitadlo pro ATA
           CMP    Byte ptr DS:[Task_Switching_Disabled],00h    ;Musime zjistit, jestli je mozno prepnout task
           Jne    @TaskMan_Interrupt_NotAble2Switch_Task
           Call    Task_Switch_Disable
           Call   TaskMan_ClearBusyFlag                       ;vynulujeme flag busy z posledniho tasku
           Call   TaskMan_Save_Procesor_State                 ;ulozi informace o stavu procesu do TSS stareho tasku
     @TaskMan_Interrupt_Returned_Zombie :
           Mov    EDX,Dword ptr DS:[Current_Layout_Pos]       ;Pozice v layout
           Inc    EDX
           Cmp    EDX,Dword ptr DS:[Current_Layout_Length]    ;Pocet rozmisteni tasku v layout
           Jna    Short @TaskMan_Interrupt_Continue0001
           Xor    EDX,EDX
      @TaskMan_Interrupt_Continue0001 :
           ;v EDX je pozice v layout
           Mov    Dword ptr DS:[Current_Layout_Pos],EDX
           Shl    EDX,2
           Mov    EAX,Dword ptr FS:[EDX] ;EAX je cislo tasku, ktery ma byt spusten
           Mov    Dword ptr DS:[Current_Task],EAX
           Call   Taskman_Find_Task
           Cmp    Word ptr ES:[EDX.TSK_Info_Status],_TSK_Status_RUN
           Je     @TaskMan_Interrupt_Task_Avaible_To_Run
           Cmp    Word ptr ES:[EDX.TSK_Info_Status],_TSK_Forever_RUN
           Jne    @TaskMan_Interrupt_Returned_Zombie
      @TaskMan_Interrupt_Task_Avaible_To_Run :
           Call   TaskMan_SetUp_NewTask
      @TaskMan_Interrupt_End :
           Call    Task_Switch_Enable
      @TaskMan_Interrupt_NotAble2Switch_Task :
           Mov    AX, Word ptr DS:[TSK_Time_Switch_Const]     ;Nastavime konstantu casovace
           Out    40h, Al
           Xchg   Al,Ah
           Out    40h,Al
           ;irq acknowledge
           Mov    Al, 20h
           Out    0a0h,Al
           Out    20h, Al
         IRET ;samotne prepnuti tasku
           Jmp   @TaskMan_Interrupt_Entrance_Code_Inter

;##############################################################################
;# Procedura vyhleda v seznamu tasku ten task, jehoz ID se shoduje s EAX
;#############################################################################
;# Vstup  : EAX - ID cislo tasku, ktery hledame
;#          ES  - selektor s informacemi o tascich
;#          DS  - data segment kernelu
;# Vystup : EDX - zacatek informaci o tasku
;#############################################################################
Taskman_Find_Task PROC
         Xor     EDX,EDX
         Mov     ECX,Max_Task
  @TaskMan_Find_Task_Find :
         Cmp     Dword ptr ES:[EDX.TSK_Info_ID],EAX
         Je      @TaskMan_Find_Task_End
         Add     EDX,TSK_Info_Size
         Dec     ECX
         Jnz     Short @TaskMan_Find_Task_Find
         ;### Olala, dany task nebyl nalezen... nastala chyba
         Mov    EAX,er_TSK+0001h
         Call   Fatal_Error
  @TaskMan_Find_Task_End :
         RET
Taskman_Find_Task ENDP
;##############################################################################
;# Procedura nuluje priznak busy u predesleho tasku
;#############################################################################
;# Vstup  : none
;# Vystup : noe
;#############################################################################
TaskMan_ClearBusyFlag PROC
         Xor    EDX,EDX
         Mov    DX,word ptr GS:[TSS_Struc.TSS_Link] ;EDX = selektor na TSS posledniho
         PUSH   DS
          Mov    Ax,GDT_Descriptor
          Mov    DS,AX
          Mov    EAX,Dword ptr DS:[EDX+4]
          And    Ah,0F0h
          Or     Ah,TSS_Descriptor_Type_Free
          Mov    Dword ptr DS:[EDX+4],EAX
         POP    DS
         RET
TaskMan_ClearBusyFlag ENDP
;##############################################################################
;# Procedura, ktera ulozi vsechny dulezite informace o predeslem tasku
;##############################################################################
TaskMan_Save_Procesor_State PROC
         PUSH    DS
          Movzx   EDX, Word ptr DS:[Current_Task_TSS_Sel]
          Call    Memory_Manager_Create_Alias
          Jc      @TaskMan_Save_Procesor_State_Skip
          Mov     DS, EDX
          FSAVE   DS:[TSS_Coprocesor_Register]
          Dispose_Temp_Descriptor DS
@TaskMan_Save_Procesor_State_Skip :
         POP     DS
         RET
TaskMan_Save_Procesor_State ENDP
;##############################################################################
;# Procedura, ktera obnovi stav procesoru pro novy task
;##############################################################################
;# Vstup  : EDX - zacatek informaci o tasku
;##############################################################################
TaskMan_SetUp_NewTask PROC
          PUSH   DS FS
           Xor   EBX,EBX
           Mov   BX,Word ptr ES:[EDX.TSK_Info_Selektor]
           Mov   Word ptr GS:[TSS_Struc.TSS_Link],BX ;ulozime na Link novou adresu
           Mov   Word ptr DS:[Current_Task_TSS_Sel],BX
           ;## musime nastavit novy TSS jako busy
           Mov    Ax,GDT_Descriptor
           Mov    FS,AX
           Mov    EAX,Dword ptr FS:[EBX+4]
           Mov    ECX,DWord ptr FS:[EBX+0]
           And    Ah,0F0h
           Or     Ah,TSS_Descriptor_Type_Busy
           Mov    Dword ptr FS:[EBX+4],EAX
           Mov    EDX,EBX
           Call   Memory_Manager_Create_Alias
           Jc     @TaskMan_Restore_Procesor_State_Skip
           Mov    DS,DX
           Inc    Dword ptr DS:[TSS_Time_Used]
           FRSTOR DS:[TSS_Coprocesor_Register]
           Dispose_Temp_Descriptor DS
     @TaskMan_Restore_Procesor_State_Skip:
          Pop    FS DS
         RET
TaskMan_SetUp_NewTask ENDP
