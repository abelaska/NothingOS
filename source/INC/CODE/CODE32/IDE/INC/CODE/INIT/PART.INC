;##############################################################################
;# Procedura nacta vsechny partitiony na vsech discich
;##############################################################################
Load_Partitions_Info Proc
        PUSH    ES FS DS
          mov   ax ,data_descriptor
          mov   ds, ax
          mov   gs, ax
          Xor    EAX,EAX
          Mov    AX,word ptr DS:[part_sel]
          Mov    ES,AX  ;ES = partitions info descriptor
          ;vynulujeme obsah descriptoru partitions info
           LSL   ECX,EAX
           Inc   ECX
           Shr   ECX,2  ;ECX = pocet dd, ktere chceme nulovat
           Xor   EAX,EAX
           Xor   EDI,EDI
           Rep   Stosd

          Xor    EBX, EBX ; LBA addressa
          Xor    EDX, EDX ; Pozicev part segmentu, kde se zapisi informace o partitionech
          Mov    dword ptr GS:[Temp_Disk],EDX ;inicializujeme primary master
          Mov    dword ptr GS:[Partition_Count],EDX ;nastavime pocet partitionu na 0
          Call   Load_MBR

          Cmp    Byte ptr GS:[IDE_Drive1. ATA_ATAPI],1
          Jne    @Skip_Disk_1
          Xor    EBX, EBX ; LBA addressa
          Mov    EDX,dword ptr GS:[Part_Per_Disk]
          Mov    Dword ptr GS:[Temp_Disk],1
          Mov    dword ptr GS:[Partition_Count],EBX ;nastavime pocet partitionu na 0
          Call   Load_MBR
@Skip_Disk_1 :
          Cmp    Byte ptr GS:[IDE_Drive2. ATA_ATAPI],1
          Jne    @Skip_Disk_2
          Xor    EBX, EBX ; LBA addressa
          Mov    EDX,dword ptr GS:[Part_Per_Disk]
          Shl    EDX,1
          Mov    Dword ptr GS:[Temp_Disk],2
          Mov    dword ptr GS:[Partition_Count],EBX ;nastavime pocet partitionu na 0
          Call   Load_MBR
@Skip_Disk_2 :
          Cmp    Byte ptr GS:[IDE_Drive3. ATA_ATAPI],1
          Jne    @Skip_Disk_3
          Xor    EBX, EBX ; LBA addressa
          Mov    EDX,dword ptr GS:[Part_Per_Disk]
          Shl    EDX,2
          Sub    EDX,Dword ptr GS:[Part_Per_Disk]
          Mov    Dword ptr GS:[Temp_Disk],3
          Mov    dword ptr GS:[Partition_Count],EBX ;nastavime pocet partitionu na 0
          Call   Load_MBR
@Skip_Disk_3 :
        POP     DS FS ES
        RET
Load_Partitions_Info Endp
;##############################################################################
;# Procedura nacta vsechny partitiony na danem disku
;##############################################################################
;# Vstup : ES = Partition info descriptor
;#         GS = data segment
;#         EBX = lba pozice, ze ktere se bude nacitat MBR
;#         EDX = offset v ES, kam se bude zapisovat (div 4)
;##############################################################################
Load_MBR Proc
        push eax
          mov    ax ,data_descriptor
          mov    gs, ax
          Xor    EAX,EAX
          Mov    AX,word ptr gS:[part_sel]
          Mov    ES,AX  ;ES = partitions info descriptor
        pop eax

       push   eax

       push   ebx
       push   ecx
       push   edx
       push   ds
       push   edi
       push   esi
       PUSH    EAX
        Mov    eax,512
        call   kalloc
        mov    DS,AX
       POP     EAX
        mov   eax, ebx
        ; nacte mbr
       PUSH   EBX EDX DS ES
        PUSH  DS
        POP   ES
        mov   ecx, 0101h
        xor   edi, edi
        mov   edx, dword ptr gs:[temp_disk]
        mov   ebx,7
        call  ATA_Gate
       POP    ES DS EDX EBX
        cmp   word ptr DS:[510],0AA55h
        je    @MBR_signature_OK
        or    EDX,EDX
        jz    @MBR_Load_OK ;signatura nesouhlasi, skipujeme tuto partitionu
        dec   EDX
        Jmp   @MBR_Load_OK ;signatura nesouhlasi, skipujeme tuto partitionu
@MBR_signature_OK :
        mov   ecx, 4
        mov   esi, (512-2)-(10h*4)
Load_MBR_lop:
        cmp   byte ptr ds:[esi+offset partition.Sys_Code],Nothing_Root_Code
        jne   Load_MBR_next3
        PUSH  EAX EBX
         mov   eax,dword ptr  gs:[partition_count]
         shl   eax,2
         mov   ebx, 5
         sub   ebx, ecx
         add   eax,ebx
         mov   ah,byte ptr  gs:[Temp_Disk]
         inc   ah
         cmp   ah, 1 ; primary master
         jnz   short Load_MBR_next10
         shl   eax,16
         or    eax,IDE_driver
         mov   dword ptr gs:[root_partition_num],eax
Load_MBR_next10:
        POP  EBX EAX
        jmp   short Load_MBR_next4
Load_MBR_next3 :
        cmp   byte ptr ds:[esi+offset partition.Sys_Code],Swap_Part_code
        jne   Load_MBR_next4
        PUSH  EAX EBX
         mov   eax,dword ptr  gs:[partition_count]
         shl   eax,2
         mov   ebx, 5
         sub   ebx, ecx
         add   eax,ebx
         mov   ah,byte ptr  gs:[Temp_Disk]
         inc   ah
         cmp   ah, 1 ; primary master
         jnz   short Load_MBR_next11
         shl   eax,16
         or    eax,IDE_driver
         mov   dword ptr gs:[swap_partition_num],eax
Load_MBR_next11:
        POP  EBX EAX
Load_MBR_next4:
        cmp   byte ptr ds:[esi+offset partition.Sys_Code], 15 ;EXT15
        je    short Load_MBR_next
        cmp   byte ptr ds:[esi+offset partition.Sys_Code], 5  ;EXT5
        jne    short Load_MBR_next2
Load_MBR_next:
        mov   ebx, dword ptr ds:[esi+offset partition.Relative_Beginning]
        inc   dword ptr gs:[partition_count]
       push   edx
        mov   eax, dword ptr gs:[partition_count]
        shl   eax, 2
        add   edx,eax
        call  Load_MBR
       pop    edx

Load_MBR_next2:
        add   esi, 10h
        dec   ecx
        jnz   Load_MBR_lop

        mov   esi, 512-2-10h*4
        shl   edx, 4
        mov   edi, edx
        mov   ecx, 10h
        cld
        rep   movsd

        xor  edx, edx
        mov  dx, ds
        mov  ax, gs
        mov  ds, ax
        call memory_manager_deallocate_memory
@MBR_Load_OK :
       pop    esi
       pop    edi
       pop    ds
       pop    edx
       pop    ecx
       pop    ebx
       pop    eax
        ret
Load_MBR Endp


