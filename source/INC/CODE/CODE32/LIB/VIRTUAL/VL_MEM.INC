;#############################################################################
;# Virtualni jednotka Memory
;#############################################################################

;#############################################################################
;# Fce      - name
;#############################################################################
;# 00000000 - TSKMM_Alloc_B
;# 00000001 - TSKMM_Dealloc
;# 00000002 - Alloc_shared_memory               (Booh Only)
;# 00000003 - Alloc_fyslin_memory               (Booh Only)
;# 00000004 - Alloc_taskman_memory              (Booh Only)
;# 00000005 - DeAlloc_vl                        (Booh Only)
;# 00000006 - Memory_Manager_Create_CallGate_vl (Booh Only)
;#############################################################################

VL_Mem :
;################ Emulovana structura souboru knihovny
DB   'LIB',0                 ; Signatura
DW   0001h                   ; Version
DB   'memory          '      ; Library Name
DW   0000h                   ; Unit Number  - not used in Virtual +18
DD   00000000h               ; CodeSeg size - not used in Virtual +20
VL_Mem_EnterPoint_CPL3:Jmp VL_Mem_EnterPoint_3  ;+???
;##################################################################################
;# Vstupni bod knihovny
;##################################################################################
VL_Mem_EnterPoint_3 :
          Cmp    EAX,VL_Mem_Num_Of_Fce
          Ja     Short @VL_Mem_End
        PUSH     DS
          PUSH   DATA_Descriptor
          Pop    DS
          Mov    EAX,Dword ptr DS:[VL_Mem_Fce+EAX*4]
        POP      DS
          PUSH   EAX
          Call   Dword ptr SS:[ESP]
          Jc     @VL_MEM_End_C
          Add    ESP,4
          CLC
          Jmp     @VL_MEM_End
   @VL_MEM_End_C :
          ADD     ESP,4
          STC
   @VL_Mem_End :
        RetF
;##################################################################################
Alloc_shared_memory PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Alloc_Shared_mem_vl_NoRights
         Xor    EAX, EAX
         Call   Memory_Manager_Allocate_Memory
    Alloc_Shared_mem_vl_NoRights :
        Ret
Alloc_shared_memory ENDP
;##################################################################################
Alloc_fyslin_memory PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Alloc_FysLin_mem_vl_NoRights
         Xor    EAX, EAX
         Inc    EAX
         Call   Memory_Manager_Allocate_Memory
    Alloc_FysLin_mem_vl_NoRights :
        Ret
Alloc_fyslin_memory ENDP
;##################################################################################
Alloc_taskman_memory PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    Alloc_TaskMan_mem_vl_NoRights
         Mov    EAX,2
         Call   Memory_Manager_Allocate_Memory
    Alloc_TaskMan_mem_vl_NoRights :
        Ret
Alloc_taskman_memory ENDP
;##################################################################################
DeAlloc_vl PROC
        PUSHAD
         Call   TASK_Get_Current_Task_Num
         Call   TASK_Get_Task_UID_GID_Eff
         Or     AX,AX
        POPAD
         Jnz    DeAlloc_vl_NoRights
         Call   Memory_Manager_Deallocate_Memory
    DeAlloc_vl_NoRights :
        Ret
DeAlloc_vl ENDP
;############################################################################
;# Procedura vytvari CallGate descriptor v dane tabulce descriptoru
;############################################################################
;# Vstup  : (BX:EDI) = vstupni bod
;#                       - BX  = Selector na kodovy descriptor
;#                       - EDI = Offset vstupniho bodu
;#           DL = CallGate DPL
;#           DH = CallGate CNT
;#           CL = Tabulka descriptoru, ve ktere chceme vytvorit call gate
;#                   0 = GDT
;#                   1 = LDT
;#           FS = LDT, jestli Al = 1
;############################################################################
;# Vystup : EAX - Error Code
;#                  00000000 = v poradku
;#                  00000001 = nedostatek volnych descriptoru
;#          EDX - selector na alokovany deskriptor, je-li EAX = 0
;############################################################################
;# Nici registery : None
;############################################################################
Memory_Manager_Create_CallGate_vl PROC
         Mov   Al,Cl
         CAll  Memory_Manager_Create_CallGate
        Ret
Memory_Manager_Create_CallGate_vl ENDP