;############################################################################
;# Program INIT - zakladni program natazeny jako prvni do pameti :)
;############################################################################
Execute_INIT PROC
        Print starting_init
        Call   TASK_Get_Current_Task_Num
        Xor    EAX,EAX
        Call   TASK_Set_VFS_Home
        Call   TASK_Set_My_Current_Dir
       ;Otevreme tempovaci adresar
        Call   TASK_Set_VFS_Temp
       ;Spustime program nosh na vsech konzolach
         Mov    Eax, Data_Descriptor
         Mov    DS, EAX
         Mov    ESI, Offset TaskMan_Name
         Call   LIB_Get_CallGate
         Mov    Dword ptr DS:[TaskMan_Call_Gate],EDX
         Mov    Dword ptr DS:[TaskMan_Call_Gate+4],EDX
         Mov    ECX,10
@Init_Open_Consoles :
         Print  Point_txt
         Mov    ESI, Offset Offset Cons_Path_1
         Mov    EAX, 3
         Call   VFS_Open
         PUSH   EAX
         Dec    Byte ptr DS:[cons_path_1_change]
         Mov    ESI, Offset Offset Keyb_Path_1
         Mov    EAX, 3
         Call   VFS_Open
         PUSH   EAX
         Dec    Byte ptr DS:[Keyb_path_1_change]
         Dec    ECX
         Jnz    @Init_Open_Consoles
         Print  New_Line_Txt
         Print  New_Line_Txt
         Mov    EBP,10
@Init_Exec_Loop :
         POP   EBX
         POP   ECX
         Or    EBX,EBX
         Jnz   @Init_Exec_Loop_Cont
         Dec   EBP
         Jnz   @Init_Exec_Loop
         Jmp   @Init_Exec_Loop_End
@Init_Exec_Loop_Cont :
         Mov   EDI, ECX
         Mov   ESI, Offset exec_path
         Mov   EDX, 1
         Mov   EAX, 0
         Cmp   EBP,1
         Jne   @Init_Exec_Skip_Last
         Dec   EDX
@Init_Exec_Skip_Last :
         Call  Fword ptr DS:[TaskMan_Call_Gate]
         Dec   EBP
         Jnz   @Init_Exec_Loop
@Init_Exec_Loop_End :
         Call  Task_Sleep_Me
         jmp $
      RET
Execute_INIT ENDP

