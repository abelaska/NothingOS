;##############################################################################
;# Procedura pro alokovani dvou descriptoru, jeden pro TSS a druhy pro LDT
;##############################################################################
;# Vstup  : EBP(- pocet 4k page ktere maji byt v LDT
;# Vystup : BX  - TSS descriptor
;#          DX  - LDT descriptor
;#          EAX - Error
;# Nici : EDX, EBX, EAX
;##############################################################################
Memory_Manager_Allocate_Memory_TSS_LDT PROC
           Mov   EAX,2
           Mov   EDX,TSS_Size
           Call  Memory_Manager_Allocate_Memory
           OR    EAX,EAX
           Jnz   @Memory_Manager_Allocate_Memory_TSS_LDT_Error
           Mov   EBX,EDX
           Mov   EDX,EBP
           And   EDX,0FFFFh
           Call  Memory_Manager_Allocate_Memory
           Or    EAX,EAX
           Jnz   @Memory_Manager_Allocate_Memory_TSS_LDT_Error
           Call  TSK_TSS_LDT_ClearUp
@Memory_Manager_Allocate_Memory_TSS_LDT_Error :
           RET
Memory_Manager_Allocate_Memory_TSS_LDT ENDP
;###########################################################################
;# Procedura se pokusi vytvorit CR3 descriptor
;###########################################################################
;# Vstup  : None
;# Vystup : If C <> 0 Then EAX - Bazova adresa CR3
;###########################################################################
MemMan_Create_CR3 PROC
      PUSH   EDX ECX EDI ESI
        Mov    EDX, 1
        Mov    EAX, EDX
        Call   Memory_Manager_Allocate_Memory
        JC     @MemMan_Create_CR3_End
        Mov    EAX, EDX
        Mov    EDX,Pages_Descriptor
       PUSH    DS ES
        Mov    DS,EDX
        Mov    ES,EAX
        Xor    ESI,ESI
        Xor    EDI,EDI
        Mov    ECX,(4096/4)
        Xor    EAX,EAX
        Rep    Stosd
        Xor    EDI,EDI
        Mov    ECX,(1024*3)
        Rep    Movsb
        Mov    EDX,ES
       POP     ES DS
        Call   MemMan_ReturnDescriptorBaseAddress
        Call   Memory_Manager_Dispose_Descriptor
       PUSH   EAX DS
         Mov  EDX,Data_Descriptor
         Mov  DS,EDX
         Mov  DS,Word ptr DS:[TaskMan_CR3s_Descriptor]
         Xor  EDX,EDX
         Mov  ECX,Max_Task
   @MemMan_Create_CR3Loop :
         Cmp  Dword ptr DS:[EDX],0
         Je   @MemMan_Create_CR3Loop_found
         Add  EDX,4
         Dec  ECX
         Jnz  @MemMan_Create_CR3Loop
   @MemMan_Create_CR3Loop_found :
         Mov    Dword ptr DS:[EDX],EAX
       POP    DS EAX
@MemMan_Create_CR3_End :
      POP    ESI EDI ECX EDX
        RET
MemMan_Create_CR3 ENDP


