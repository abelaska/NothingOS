;#############################################################################
;# Inicializace prepinani tasku
;#############################################################################

;#############################################################################
;# Procedura provede inicializaci taskmana
;#############################################################################
;# Vstup  : none
;# Vystup : none
;#############################################################################
TSK_Initializing PROC
        Call    TSK_Allocation_Memory
        Call    TSK_TSS_LDT_ClearUP
        Call    TSK_InfoDsc_Init
        Call    TSK_Kernel_TSS_SetUP
        Call    TSK_Change_Descriptors
        Call    TSK_Load_LDT
        CLI
        PUSH    ES
         Mov     Ax,IDT_Descriptor
         Mov     Es,Ax
         Mov     AX,BX
         Shl     EAX,16
         Mov     Dword ptr ES:[(0F0h Shl 3)],EAX
         Xor     EAX,EAX
         Mov     Ah,TSS_Atr1
         Mov     Dword ptr ES:[(0F0h Shl 3)+4],EAX
        POP    ES
         Call    TSK_Load_First_Program
Kernel_Task_Starting_Point :
        STI
         Call    TSKMM_Inicialize_LDT
         Print   Task_Init_txt
         Ret
TSK_Initializing ENDP
;#############################################################################
;# Procedura alokuje pameti pro vsechny struktury taskmana
;#############################################################################
;# Vstup  : None
;# Vystup : EBX = selektor na TSS kernelu
;#          EDX = selektor na LDT kernelu
;#############################################################################
;# Pozn   : EBX i EDX ukazuji na datovy segment, musime v nich upravit hodnoty
;#          a prevest na system type.
;#############################################################################
TSK_Allocation_Memory PROC
         PUSH  DS
           Mov    AX,Data_Descriptor
           Mov    DS,AX

           Mov    EAX,Max_Task*4
           Call   Kalloc
           Mov    Word ptr DS:[TaskMan_CR3s_Descriptor],AX
           Mov    Byte ptr DS:[TaskMan_CR3_Semafor],1
           PUSH   ES
            Mov    ES,AX
            Mov    EAX,CR3
            Mov    Dword ptr ES:[0],EAX
           POP    ES

           Mov    AX,4096
           Call   KAlloc
           PUSH   EAX
            Mov   EDX,EAX
            Call  MemMan_ReturnDescriptorBaseAddress
            PUSH  FS
             Mov   EDX,GDT_Descriptor
             Mov   FS,EDX
             Mov   EDX,Stack_Descriptor_Helper
             Mov   Word ptr  FS:[EDX+2],AX
             ROR   EAX,16
             Mov   Byte ptr  FS:[EDX+4],AL
             Mov   Byte ptr  FS:[EDX+7],AH
             ROR   EAX,16
             Mov   Word  ptr FS:[EDX+0],0
             Shr   ECX,16
             Mov   Byte ptr FS:[EDX+5], Data_Atr1
             Mov   Byte ptr FS:[EDX+6], Data_Atr2
            POP   FS
           POP    EAX
           Dispose_Temp_Descriptor EAX
           Mov    EDX,Stack_Descriptor_Helper
           Call   Memory_Manager_Data2Stack_Descr

           Mov    Dword ptr DS:[Max_Zoombies],Start_Max_Zoombies
           Mov    EAX,(TSK_Info_Size*Max_Task)
           ;Alokujeme pamet pro TSK_Info struktury
           Call   Kalloc
           Mov    Word ptr DS:[TaskMan_TSK_Info_Descriptor],AX
           Mov    EAX,(Max_Task*Max_Priority)
           ;Alokujeme pamet pro TSK_Layout struktury
           Call   Kalloc
           Mov    Word ptr DS:[TaskMan_TSK_TSK_Layout_Descriptor],AX
         POP   DS
         ;Alokujeme si prvni TSS,pro kernel
         Mov    EAX,2
         Mov    EDX,TSS_Size
         Call   Memory_Manager_Allocate_Memory
         Or     EAX,EAX
         Jnz    Short @TSK_Allocation_Error
         ;Alokujeme LDT kernelu
         Mov    EBX,EDX
         Mov    EAX,2
         Mov    EDX,1
         Call   Memory_Manager_Allocate_Memory
         Or     EAX,EAX
         Jnz    Short @TSK_Allocation_Error
         Jmp    Short @TSK_Allocation_OK
@TSK_Allocation_Error :
         Mov    EAX,er_TSK+0000h
         Call   Fatal_Error
@TSK_Allocation_OK :
        RET
TSK_Allocation_Memory ENDP
;#############################################################################
;# Procedura nuluje hodnoty v LDT
;#############################################################################
;# Vstup  : EDX = selektor na LDT
;#          EBX = selektor na TSS
;# Vystup : None
;# Nici   : None
;#############################################################################
TSK_TSS_LDT_ClearUp PROC
        PUSH   ES ECX EDI EAX
         LSL   ECX,EDX
         Mov   ES,DX
         Inc   ECX
         Shr   ECX,2  ;ECX = pocet dd, ktere chceme nulovat
         Xor   EAX,EAX
         Xor   EDI,EDI
         Rep   Stosd
         LSL   ECX,EBX
         Mov   ES,BX
         Inc   ECX
         Shr   ECX,2  ;ECX = pocet dd, ktere chceme nulovat
         Xor   EAX,EAX
         Xor   EDI,EDI
         Rep   Stosd
        POP   EAX EDI ECX ES
        RET
TSK_TSS_LDT_ClearUp ENDP
;#############################################################################
;# Procedura nuluje hodnoty v TSK_Info_Descriptor
;#############################################################################
;# Vstup  : EDX = selektor na LDT
;# Vystup : None
;# Nici   : None
;#############################################################################
TSK_InfoDsc_Init PROC
        PUSH   ES ECX EDI EAX
         Mov   AX,Data_Descriptor
         Mov   ES,AX
         Xor   ECX,ECX
         Mov   Cx,Word ptr ES:[TaskMan_TSK_Info_Descriptor]
         Mov   ES,CX
         LSL   ECX,ECX
         Inc   ECX
         Shr   ECX,2
         Xor   EAX,EAX
         Xor   EDI,EDI
         Rep   Stosd
        POP   EAX EDI ECX ES
        RET
TSK_InfoDsc_Init ENDP
;#############################################################################
;# Procedura nastavuje pocatecni hodnoty v TSS kernelu
;#############################################################################
;# Vstup  : EBX = alias na TSS
;#          EDX = alias na LDT
;# Vystup : EAX = Error Code
;# Nici   : EAX
;#############################################################################
TSK_Kernel_TSS_SetUP PROC
        PUSH  DS
          ;vytvorime alias pro TSS selektor TaskMana
          Mov   AX,GDT_Descriptor
          Mov   DS,AX
          Mov   ECX,Task_Descriptors_Start_Pos
          Mov   EDI,(First_Avaible_Descriptor)-8
@TSK_LookForDescriptor :
          Add   EDI,8
          Mov   EAX,Dword ptr DS:[EDI]
          Add   EAX,Dword ptr DS:[EDI+4]
          Jz    @TSK_FoundDescriptor
          Dec   ECX
          Jnz   @TSK_LookForDescriptor
           Mov   EAX,er_TSK +0002h
           Call  Fatal_Error
@TSK_FoundDescriptor :
         Mov   EAX,Dword ptr DS:[EBX]
         Mov   Dword ptr DS:[EDI],EAX
         Mov   EAX,Dword ptr DS:[EBX+4]
         Mov   Dword ptr DS:[EDI+4],EAX
         Mov   AX,Data_Descriptor
         Mov   DS,AX
         Mov   Word ptr DS:[TaskMan_TSS_Alias_Sel],DI
         ;Alias hotovy, nyni musime provest inicializaci hodnot v TSS
         Mov   DS,EBX
         Mov   Word ptr DS:[TSS_Struc.TSS_Ports],0FFFFh      ;nastavime konec IO map
         Mov   Word ptr DS:[TSS_Struc.TSS_UnFrm],TSS_UnFormated_Offset  ;nastavime offset IOmap
         Mov   EAX,Offset Task_Manager_interrupt
         Mov   Dword ptr DS:[TSS_Struc.TSS_EIP],EAX
         Mov   EAX,SS
         PUSH  EDX
          Mov   EDX,1
          Xor   EAX,EAX
          Call  Memory_Manager_Allocate_Memory
          Call  Memory_Manager_Data2Stack_Descr
          Mov   EAX,EDX
         POP  EDX
         Xor  ECX,ECX
         DEC  ECX
         Mov   Word ptr DS:[TSS_Struc.TSS_SS],AX
         Mov   Word ptr DS:[TSS_Struc.TSS_CPL0_SS],AX
         Mov   DWord ptr DS:[TSS_Struc.TSS_ESP],ECX
         Mov   Dword ptr DS:[TSS_CPL0_ESP],ECX
         Mov   Ax,Data_Descriptor
         Mov   Word ptr DS:[TSS_Struc.TSS_DS],AX
         Mov   Word ptr DS:[TSS_Struc.TSS_ES],AX
         Mov   Word ptr DS:[TSS_Struc.TSS_FS],AX
         Mov   Word ptr DS:[TSS_Struc.TSS_GS],AX
         Mov   EAX,CS
         Mov   Word ptr DS:[TSS_Struc.TSS_CS],AX
         Mov   EAX,CR3
         Mov   Dword ptr DS:[TSS_Struc.TSS_CR3],EAX
         PUSHFD
          CLI
          PUSHFD
          POP   EAX
         POPFD
         Mov   Dword ptr DS:[TSS_EEFLAGS],EAX
         Xor   EAX,EAX
         Mov   word ptr DS:[TSS_TRAP],AX
        POP   DS
        RET
TSK_Kernel_TSS_SetUP ENDP
;#############################################################################
;# Procedura zmeni typ descriptoru TSS a LDT na spravne hodnoty
;#############################################################################
;# Vstup  : EBX = selektor na TSS
;#          EDX = selector na LDT
;#############################################################################
TSK_Change_Descriptors PROC
        PUSH   EDX EAX
         Mov   AX,LDT_Descriptor_Type
         Call  Memory_Manager_Change2System_Descr
         Mov   AX,TSS_Descriptor_Type_Free
         Mov   EDX,EBX
         Call  Memory_Manager_Change2System_Descr
        POP    EAX EDX
        RET
TSK_Change_Descriptors ENDP
;#############################################################################
;# Procedura nastavuje LDT kernelu a TSS kernelu (LLDT a LTR)
;#############################################################################
;# Vstup  : EDX = selektor na LDT
;# Vystup :
;# Nici   :
;#############################################################################
TSK_Load_LDT PROC
       LLDT  DX
      RET
TSK_Load_LDT ENDP
;#############################################################################
;# Procedura provede nacteni prvniho programu, tzn init :)
;#############################################################################
;# Vstup  : DX - selektor na LDT prvniho tasku - kernelu
;# Vystup :
;#############################################################################
TSK_Load_First_Program PROC
       PUSH   EBP EBX DS ES
        PUSH   EDX
         Mov   AX, _TSK_Status_RUN   ; Status tasku INIT
         Shl   EAX,16
         Mov   AX, Normal_Priority   ; priorita - normalni, vsak tam je sam :)
         Xor   EBX,EBX               ; Root : Root
         XOR   ECX,ECX               ; Root : Root
         Xor   EDX,EDX               ; Roor : Root
         Xor   ESI,ESI               ; Parrent = Kernel :)
         Mov   EBP,1h
         Call  TaskMan_Create_Task_Structure
       POP    EDX
       PUSH   EBX EDX
         Mov   EDX,Data_Descriptor
         Mov   DS,EDX
         Mov   Word ptr  DS:[Current_Task_TSS_Sel],BX
         Mov   Dword ptr DS:[Current_Layout_Pos],0
         Mov   Dword ptr DS:[Current_Layout_Length],0
         Mov   Dword ptr DS:[Current_Task],1
         Mov   DS,Word ptr DS:[TaskMan_TSK_TSK_Layout_Descriptor]
         Xor   EAX,EAX
         Inc   EAX
         Mov   Dword Ptr DS:[0],EAX
       POP  EDX EBX
         Mov   ES,BX
         PUSHAD
          Mov   ECX,ES
          LSL   ECX,ECX
          Xor   EDI,EDI
          Xor   EAX,EAX
          Inc   ECX
          Shr   ECX,2
          Rep   Stosd
         POPAD
         Call  TaskMan_InitializeTSS
         Mov   Word Ptr  ES:[TSS_CS],CS
         Mov   Dword ptr ES:[TSS_EIP],Offset Kernel_Task_Starting_Point
         Mov   Word ptr  ES:[TSS_SS],SS
         Mov   Word ptr  ES:[TSS_Struc.TSS_CPL0_SS],SS
         Mov   EAX,ESP
         Mov   Dword ptr ES:[TSS_ESP],EAX
         Mov   Dword ptr ES:[TSS_CPL0_ESP],EAX
         Mov   Word ptr  ES:[TSS_TRAP],0
         PUSHFD
         POP EAX
         Mov   Dword ptr ES:[TSS_EEFlags],EAX
         Mov   EAX,CR3
         Mov   Dword ptr ES:[TSS_CR3],EAX
        Mov    EAX,Data_Descriptor
        Mov    ES,EAX
        Mov    DS,EAX
        PUSH  EBX
         Call  TSK_Change_Descriptors
        POP   EDX
        LTR   DX
    POP   ES DS EBX EBP
       RET
TSK_Load_First_Program ENDP
;#############################################################################
;# Include part :)
;#############################################################################
Include .\..\inc\code\code32\TSS\tsslib.inc
Include .\..\inc\code\code32\TSS\taskinfo.inc

