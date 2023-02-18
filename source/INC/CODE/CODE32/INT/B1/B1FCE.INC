;#############################################################################
;# Implementace funkci int B1
;#############################################################################
;# B1 je verejny interrupt pro zjisteni informaci pro CPL3 od kernelu
;#############################################################################

Return_Lib_CallGate :
     PUSH  DS EAX ECX ESI
      Mov   AX,Data_Descriptor
      Mov   DS,AX
      Mov   ESI,Offset VL_Libraries
      Add   ESI,LIB_F_Name
      Call  LIB_Get_CallGate
      Jnc   Return_Lib_CallGate_OK
      Xor   EDX,EDX
   Return_Lib_CallGate_OK :
     POP   ESI ECX EAX DS
   RET

Return_System_CallGate :
     PUSH  DS EAX ECX ESI
      Mov   AX,Data_Descriptor
      Mov   DS,AX
      Mov   ESI,Offset VL_System
      Add   ESI,LIB_F_Name
      Call  LIB_Get_CallGate
      Jnc   Return_Sys_CallGate_OK
      Xor   EDX,EDX
   Return_Sys_CallGate_OK :
     POP   ESI ECX EAX DS
   RET


Wr_1 :
     pushad
     push ds
      call TASK_Get_Current_Task_Num
      mov eax, data_descriptor
      mov ds, eax
      mov edx,ebx
      push edx
      shl edx,2
      inc dword ptr ds:[task_1_priority+edx]
      push edx
       xor edx,edx
       mov eax,160
       mul ebx
       mov ebx, eax
      pop edx
      mov eax, dword ptr ds:[task_1_priority+edx]
      pop edx
      wr_reg_nowait edx, ebx
      add ebx, 20
      wr_reg_nowait eax, ebx
      push ebx
       mov   ebx, edx
       call  TASK_Get_Time_Used
      pop  ebx
       add ebx, 20
      wr_reg_nowait eax,ebx
     pop ds
     popad
   RET

Wr_2 :
  print new_line_txt
  wr_cons_nowait edx
RET

wr_3 :
 call write_tasks
 ret
