;#############################################################################
;# Virtualni jednotka TaskMan
;#############################################################################

;#############################################################################
;# Fce      - name
;#############################################################################
;# 00000000 - EXEC_Restricted
;# 00000001 - TASK_Kill_Me
;# 00000002 - TASK_Get_Current_Task_Num
;# 00000003 - TASK_Get_Task_UID_GID
;# 00000004 - TASK_Get_Task_UID_GID_Eff
;# 00000005 - TASK_Get_Task_UID_GID_FS
;# 00000006 - TASK_Get_Task_Priority
;# 00000007 - TASK_Get_Task_Status
;# 00000008 - TASK_Get_Task_Parent
;# 00000009 - TASK_Get_Task_Child
;# 0000000A - TASK_Get_Task_OlderBrother
;# 0000000B - TASK_Get_Task_YoungerBrother
;# 0000000C - TASK_Get_Tasks_Count
;# 0000000D - TASK_Get_Max_Tasks
;# 0000000E - TASK_Get_Max_Zoombies
;# 0000000F - TASK_Get_Time_Switch_Const
;# 00000010 - TASK_Switch_Now
;# 00000011 - TASK_Get_Time_Used
;# 00000012 - TASK_Get_VFS_STDOut_Inode
;# 00000013 - TASK_Get_VFS_STDIn_Inode
;# 00000014 - TASK_Get_VFS_STDError_Inode
;# 00000015 - TASK_Get_VFS_Home_Inode
;# 00000016 - TASK_Get_VFS_Temp_Inode
;# 00000017 - TASK_Get_My_Parameters_Length
;# 00000018 - TASK_Get_My_Parameters
;# 00000019 - TASK_Get_Task_Name
;# 0000001A - TASK_Get_VFS_Inode
;# 0000001B - TASK_Sleep_Me
;# 0000001C - EXEC_Ret_Libs_Name
;# 0000001D - EXEC_Ret_Libs_Version
;# 0000001E - TASK_Set_My_Current_Dir
;# 0000001F - TASK_Get_VFS_HOME
;# 00000020 - TASK_Get_VFS_Temp
;# 00000021 - TASK_Get_Current_Dir
;# 00000022 - TASK_Get_My_Current_Dir
;# 00000023 - TASK_Get_Processes_Info
;# 00000024 - TASK_Get_Parameters_Length (Booh Only)
;# 00000025 - TASK_Get_Parameters        (Booh Only)
;# 00000026 - TASK_Switch_Disable        (Booh Only)
;# 00000027 - TASK_Switch_Enable         (Booh Only)
;# 00000028 - TASK_Switch_Enable_X       (Booh Only)
;# 00000029 - TASK_Set_Task_UID_GID      (Booh Only)
;# 0000002A - TASK_Set_Task_UID_GID_eff  (Booh Only)
;# 0000002B - TASK_Set_Task_UID_GID_FS   (Booh Only)
;# 0000002C - TASK_Set_Task_Priority     (Booh Only)
;# 0000002D - TASK_Set_Task_Status       (Booh Only)
;# 0000002E - TASK_Kill_Task             (Booh Only)
;# 0000002F - EXEC_CPL0                  (Booh Only)
;# 00000030 - TASK_Set_Time_Switch_Const (Booh Only)
;# 00000031 - TASK_Set_Max_Zoombies      (Booh Only)
;# 00000032 - TASK_Sleep_Task            (Booh Only)
;# 00000033 - Task_Wakeup_Task           (Booh Only)
;# 00000034 - TASK_Set_VFS_HOME          (Booh Only)
;# 00000035 - TASK_Set_VFS_Temp          (Booh Only)
;# 00000036 - TASK_Set_Current_Dir       (Booh Only)
;#############################################################################

VL_Task :
;################ Emulovana structura souboru knihovny
DB   'LIB',0                 ; Signatura
DW   0001h                   ; Version
DB   'taskman         '      ; Library Name
DW   0000h                   ; Unit Number  - not used in Virtual +18
DD   00000000h               ; CodeSeg size - not used in Virtual +20
Jmp VL_Task_EnterPoint      ;+???
;##################################################################################
;# Vstupni bod knihovny
;##################################################################################
VL_Task_EnterPoint :
          Cmp    EAX,VL_Task_Num_Of_Fce
          Ja     @VL_Task_End
        PUSH     DS
          PUSH   DATA_Descriptor
          Pop    DS
          Mov    EAX,Dword ptr DS:[VL_Task_Fce+EAX*4]
        POP      DS
          PUSH   EAX
          Call   Dword ptr SS:[ESP]
          Jc     @VL_TASK_End_C
          Add    ESP,4
          CLC
          Jmp     @VL_TASK_End
   @VL_TASK_End_C :
          ADD     ESP,4
          STC
   @VL_Task_End :
        RetF
;##################################################################################
;# Tyto procedury maji za ukol zjistit, ma-li danny task pravo k provedeni fce
;##################################################################################
TASK_Switch_Disable_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Switch_Disable_vl_NoRights
         Call   TASK_Switch_Disable
    TASK_Switch_Disable_vl_NoRights :
        Ret
TASK_Switch_Disable_vl ENDP
;##################################################################################
TASK_Switch_Enable_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Switch_Enable_vl_NoRights
         Call   TASK_Switch_Enable
    TASK_Switch_Enable_vl_NoRights :
        Ret
TASK_Switch_Enable_vl ENDP
;##################################################################################
TASK_Switch_Enable_X_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Switch_Enable_X_vl_NoRights
         Call   TASK_Switch_Enable_X
    TASK_Switch_Enable_X_vl_NoRights :
        Ret
TASK_Switch_Enable_X_vl ENDP
;##################################################################################
TASK_Set_Task_UID_GID_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Set_Task_UID_GID_vl_NoRights
         Call   TASK_Set_Task_UID_GID
    TASK_Set_Task_UID_GID_vl_NoRights :
        Ret
TASK_Set_Task_UID_GID_vl ENDP
;##################################################################################
TASK_Set_Task_UID_GID_eff_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Set_Task_UID_GID_Eff_vl_NoRights
         Call   TASK_Set_Task_UID_GID_Eff
    TASK_Set_Task_UID_GID_Eff_vl_NoRights :
        Ret
TASK_Set_Task_UID_GID_eff_vl ENDP
;##################################################################################
TASK_Set_Task_UID_GID_FS_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Set_Task_UID_GID_FS_vl_NoRights
         Call   TASK_Set_Task_UID_GID_FS
    TASK_Set_Task_UID_GID_FS_vl_NoRights :
        Ret
TASK_Set_Task_UID_GID_FS_vl ENDP
;##################################################################################
TASK_Set_Task_Priority_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Set_Task_Priority_vl_NoRights
         Call   Task_Set_Task_Priority
    Task_Set_Task_Priority_vl_NoRights :
        Ret
TASK_Set_Task_Priority_vl ENDP
;##################################################################################
TASK_Set_Task_Status_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Set_Task_Status_vl_NoRights
         Call   Task_Set_Task_Status
    Task_Set_Task_Status_vl_NoRights :
        Ret
TASK_Set_Task_Status_vl ENDP
;##################################################################################
TASK_Kill_Task_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Kill_Task_vl_NoRights
         Call   TASK_Kill_Task
    TASK_Kill_Task_vl_NoRights :
        Ret
TASK_Kill_Task_vl ENDP
;##################################################################################
EXEC_CPL0_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    EXEC_CPL0_vl_NoRights
         Call   EXEC_CPL0
    EXEC_CPL0_vl_NoRights :
        Ret
EXEC_CPL0_vl ENDP
;##################################################################################
TASK_Set_Time_Switch_Const_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Set_Time_Switch_Const_vl_NoRights
         Call   Task_Set_Time_Switch_Const
    Task_Set_Time_Switch_Const_vl_NoRights :
        Ret
TASK_Set_Time_Switch_Const_vl ENDP
;##################################################################################
TASK_Set_Max_Zoombies_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Set_Zoombie_NoRights
         Call   TASK_Set_Max_Zoombies
    Task_Set_Zoombie_NoRights :
        Ret
TASK_Set_Max_Zoombies_vl ENDP
;##################################################################################
TASK_Get_Parameters_Length_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Get_Parameters_Length_Nr
         Call   TASK_Get_Parameters_Length
    Task_Get_Parameters_Length_Nr :
        Ret
TASK_Get_Parameters_Length_vl ENDP
;##################################################################################
TASK_Get_Parameters_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Get_Parameters_Nr
         Call   TASK_Get_Parameters
    Task_Get_Parameters_Nr :
        Ret
TASK_Get_Parameters_vl ENDP
;##################################################################################
TASK_Sleep_Task_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Sleep_Task_Nr
         Call   TASK_Sleep_Task
    Task_Sleep_Task_Nr :
        Ret
TASK_Sleep_Task_vl ENDP
;##################################################################################
Task_Wakeup_Task_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Task_Wakeup_Task_vl_Nr
         Call   Task_Wakeup_Task
    Task_Wakeup_Task_vl_Nr :
        Ret
Task_Wakeup_Task_vl ENDP
;##################################################################################
TASK_Set_VFS_HOME_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Set_VFS_HOME_vl_Nr
         Call   TASK_Set_VFS_HOME
    TASK_Set_VFS_HOME_vl_Nr :
        Ret
TASK_Set_VFS_HOME_vl ENDP
;##################################################################################
TASK_Set_VFS_Temp_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Set_VFS_Temp_vl_Nr
         Call   TASK_Set_VFS_Temp
    TASK_Set_VFS_Temp_vl_Nr :
        Ret
TASK_Set_VFS_Temp_vl ENDP
;##################################################################################
TASK_Set_Current_Dir_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    TASK_Set_VFS_Current_Dir_vl_Nr
         Call   TASK_Set_Current_Dir
    TASK_Set_VFS_Current_Dir_vl_Nr :
        Ret
TASK_Set_Current_Dir_vl ENDP
