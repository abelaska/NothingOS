;##################################################################################################
;# Jednotka obsahuje procedury k praci s descriptory
;##################################################################################################

;############################################################################
;# Procedura vytvari descriptor v dane tabulce, Datovy Descriptor,
;# G=1,D/B=1
;############################################################################
;# Vstup  : EAX  - typ deskriptoru (tzn do ktere Tabulky Deskriptoru se ma zapisovat)
;#                 0 = Globalni deskriptor ve shared mem
;#                 1 = globalni descriptor ve fyslin mem
;#                 2 = Alokovat v Descriptorovem prostoru TaskMana
;#          EBX  - Prvni bit
;#          ESI  - Prvni block
;#          ECX  - Pocet stranek
;#          DS - DATA_Descriptor
;############################################################################
;# Vystup : EAX - Error code
;#          EDI - Selektor na alokovany descriptor
;############################################################################
;# Nici :   EAX
;############################################################################
MemMan_CreateDescriptor PROC
         PUSH  ECX ESI EBX EDX ES
         PUSH  ECX EAX
          ;normalni descriptorovy prostor
           Mov   ECX,Task_Descriptors_Start_Pos  ;maximalni pocet descriptoru
           Mov   EDI,(First_Avaible_Descriptor)-8
           Or    EAX,EAX
           Jz    Short @MemMan_GlobalDescriptorAllocation
           Dec   EAX
           JZ    Short @MemMan_GlobalDescriptorAllocation
          ;Descriptorovy prostor taskmana
           Mov   ECX,(8192 - Task_Descriptors_Start_Pos)
           Mov   EDI,(Task_Descriptors_Start_Pos*8)-8
@MemMan_GlobalDescriptorAllocation :
           Mov   AX,GDT_Descriptor
           Mov   ES,AX
@MemMan_LookForDescriptor :
           Add   EDI,8
           Mov   EAX,Dword ptr ES:[EDI]
           Add   EAX,Dword ptr ES:[EDI+4]
           Jz    Short @MemMan_FoundDescriptor
           Dec   ECX
           Jnz   Short @MemMan_LookForDescriptor
         POP   EAX ECX ES
           Xor   EAX,EAX
           Add   Al,3 ;chyba cislo 3 - nenalezen volny descriptor
           Jmp   Short @MemMan_CreateDesc_End
@MemMan_FoundDescriptor :
         POP   EAX ECX
          Or    EAX,EAX
          Jnz   Short @MemMan_CreateDescriptor_Skip0001
          Add   ESI,VMem_Shared_Area/(4096*8) ;EAX = 0, sdilena pamet
@MemMan_CreateDescriptor_Skip0001 :
          Mov   EAX,ESI
          Shl   EAX,3
          Add   EAX,EBX
          Shl   EAX,12
          Mov   Word ptr ES:[EDI+2], AX
          Shr   EAX,16
          Mov   Byte ptr ES:[EDI+4], AL
          Mov   Byte ptr ES:[EDI+7], AH
          Dec   ECX
          Mov   Word  ptr ES:[EDI+0], CX
          Shr   ECX,16
          And   Cl,0Fh
          OR    Cl,Data_Atr2
          Mov   Byte ptr ES:[EDI+5], Data_Atr1
          Mov   Byte ptr ES:[EDI+6], CL
         POP   ES
          Xor   EAX,EAX
@MemMan_CreateDesc_End :
         POP   EDX EBX ESI ECX
         RET
MemMan_CreateDescriptor ENDP
;############################################################################
;# Procedura navraci bazovou adresu ulozenou v descriptoru
;############################################################################
;# Vstup  : EDX = Descriptor
;# Vystup : EAX = Bazova Addresa
;# Nici   : EAX
;############################################################################
MemMan_ReturnDescriptorBaseAddress PROC
        PUSH   DS EDX EBX ECX
        Mov    AX,GDT_Descriptor
        Mov    DS,AX
        Mov    EAX,Dword ptr DS:[EDX]
        Shr    EAX,16
        Mov    EBX,Dword ptr DS:[EDX+4]
        Mov    ECX,EBX
        And    EBX,0FFh
        Shl    EBX,16
        Or     EAX,EBX
        And    ECX,0FF000000h
        Or     EAX,ECX
        POP    ECX EBX EDX DS
        RET
MemMan_ReturnDescriptorBaseAddress ENDP
;############################################################################
;# Procedura nastavuje bazovou adresu descriptoru
;############################################################################
;# Vstup  : EDX = Descriptor
;#          EDI = bazova adresa
;# Nici   : none
;############################################################################
MemMan_SetDescriptorBaseAddress PROC
        PUSH   EDI DS EDX EBX EAX
        Mov    AX,GDT_Descriptor
        Mov    DS,AX
        Mov    EAX,Dword ptr DS:[EDX]
        ROR    EAX,16
        Mov    AX,DI
        Ror    EAX,16
        Mov    Dword ptr DS:[EDX],EAX
        Mov    EAX,Dword ptr DS:[EDX+4]
        Shr    EDI,16
        Mov    EBX,EDI
        Mov    AL,Bl
        Shr    EDI,8
        Shl    EDI,24
        And    EAX,00FFFFFFh
        Or     EAX,EDI
        Mov    Dword ptr DS:[EDX+4],EAX
        POP    EAX EBX EDX DS EDI
        RET
MemMan_SetDescriptorBaseAddress ENDP
;############################################################################
;# Procedura meni typ descriptoru Datovy <---> Codovy (pouze CPL=0)
;############################################################################
;# Vstup  : EDX  - Selector na descriptor pro prevod
;#          FS   - selector na LDT (je-li ES bit TI = 1)
;############################################################################
;# Nici registery : zadne
;############################################################################
Memory_Manager_Data2Code_Descr PROC
          Push    ES
          Push    EDX
          Push    ECX
          Test    DX,4
          Jz      @MemMan_GlobalDescriptor_Data2Seg
          Mov     CX,FS
          Jmp     @MemMan_LocalDescriptor_Data2Seg
@MemMan_GlobalDescriptor_Data2Seg :
          Mov     CX,GDT_Descriptor
@MemMan_LocalDescriptor_Data2Seg :
          Mov     ES,CX
          And     DX,0FFF8h
          Mov     ECX,800h
          XOR     Dword ptr ES:[EDX+4],ECX
          Pop     ECX
          Pop     EDX
          Pop     ES
          RET
Memory_Manager_Data2Code_Descr ENDP
;############################################################################
;# Procedura meni typ descriptoru na systemovy (pouze CPL=0)
;# Vyuziti napr. u TSS...
;############################################################################
;# Vstup  : EDX  - Selector na descriptor pro prevod - musi byt v GDT
;#          AX - typ systemoveho descriptoru
;############################################################################
;# Nici registery : EAX
;############################################################################
Memory_Manager_Change2System_Descr PROC
          Push    ECX
          Push    EDX
          Push    ES
          And     DX,0FFF8h
          Mov     CX,GDT_Descriptor
          Mov     ES,CX
          Mov     ECX,Dword ptr ES:[EDX+4]
          And     ECX,0FFFFE0FFh
          And     EAX,00000000Fh
          Shl     EAX,8
          Or      ECX,EAX
          Mov     Dword ptr ES:[EDX+4],ECX
          Pop     ES
          Pop     EDX
          Pop     ECX
          RET
Memory_Manager_Change2System_Descr ENDP
;############################################################################
;# Procedura meni DPL descriptoru (pouze CPL=0)
;############################################################################
;# Vstup  : EDX  - Selector descriptoru ke zmene
;#          EAX - novy DPL
;#          FS  - selector na LDT (je-li DX bit TI = 1, tzn localni descr)
;# Vysup    EDX - novy selector (zmenene RPL)
;############################################################################
;# Nici registery : zadne
;############################################################################
Memory_Manager_ChangeDPL_Descr PROC
          Push    ES
          Push    EBX
          Push    EAX
          Push    ECX
          Test    DX,4
          Jz      @MemMan_GlobalDescriptor_DPL
          Mov     CX,FS
          Jmp     @MemMan_LocalDescriptor_DPL
@MemMan_GlobalDescriptor_DPL :
          Mov     CX,GDT_Descriptor
@MemMan_LocalDescriptor_DPL :
          Mov     ES,CX
          Mov     EBX,EDX
          And     EBX,0FFF8h
          Mov     ECX,Dword ptr ES:[EBX+4]
          And     ECX,0FFFF9FFFH
          And     EAX,3
          And     Dl,0FCh
          Or      Dl,Al
          Shl     EAX,13
          Or      ECX,EAX
          Mov     Dword ptr ES:[EBX+4],ECX
          Invlpg  Dword ptr ES:[EBX+4]
          Pop     ECX
          Pop     EAX
          Pop     EBX
          Pop     ES
          RET
Memory_Manager_ChangeDPL_Descr ENDP
;############################################################################
;# Procedura invertuje Readable / Writeble bit
;############################################################################
;# Vstup  : EDX - Selector descriptoru ke zmene
;#          FS  - selector na LDT (je-li ES bit TI = 1, tzn localni descr)
;############################################################################
;# Nici registery : zadne
;############################################################################
Memory_Manager_InvertRWBit_Descr PROC
          Push    ES
          Push    EDX
          Push    ECX
          Test    DX,4
          Jz      @MemMan_GlobalDescriptor_RW
          Mov     CX,FS
          Jmp     @MemMan_LocalDescriptor_RW
@MemMan_GlobalDescriptor_RW :
          Mov     CX,GDT_Descriptor
@MemMan_LocalDescriptor_RW :
          Mov     ES,CX
          And     DX,0FFF8h
          Mov     ECX,200h
          XOR     Dword ptr ES:[EDX+4],ECX
          Pop     ECX
          Pop     EDX
          Pop     ES
          RET
Memory_Manager_InvertRWBit_Descr ENDP
;############################################################################
;# Procedura vytvari z datoveho segmentu stackovy segment
;# nebo, je-li to codovy segment, nastavuje C bit do jednicky...
;############################################################################
;# Vstup  : EDX - Selector descriptoru ke zmene
;#          FS  - selector na LDT (je-li ES bit TI = 1, tzn localni descr)
;############################################################################
;# Nici registery : zadne
;############################################################################
Memory_Manager_Data2Stack_Descr PROC
          Push    ES
          Push    EDX
          Push    ECX
          Test    DX,4
          Jz      Short @MemMan_GlobalDescriptor_Stack
          Mov     CX,FS
          Jmp     Short @MemMan_LocalDescriptor_Stack
@MemMan_GlobalDescriptor_Stack :
          Mov     CX,GDT_Descriptor
@MemMan_LocalDescriptor_Stack :
          Mov     ES,CX
          PUSH    EDX
          And     EDX,0FFF8h
          Mov     ECX,Dword ptr ES:[EDX+4]
          Or      ECX,700h
          Mov     Dword ptr ES:[EDX+4],ECX
          ;nastveni descriptoru
          POP     EDX
          Mov     ES,EDX
          Mov     Byte Ptr ES:[0FFFFFFFFh],Al
          Pop     ECX
          Pop     EDX
          Pop     ES
          RET
Memory_Manager_Data2Stack_Descr ENDP
;###########################################################################
;# Procedura nuluje dany descriptor
;###########################################################################
;# Vstup  : DX - Selector Descriptor
;#          FS - LDT, jesli se nici lokalni descriptor
;# Vystup : None
;# Nici   : EDX
;###########################################################################
Memory_Manager_Dispose_Descriptor PROC
          Push   FS
          Push   EAX
          Test   DX,4
          Jnz    @Memory_Manager_Dispose_Descriptor_LDT
          Mov    AX,GDT_Descriptor
          Mov    FS,AX
@Memory_Manager_Dispose_Descriptor_LDT :
          Xor    EAX,EAX
          And    EDX,0FFF8h
          Mov    Dword ptr FS:[EDX],EAX
          Mov    DWord ptr FS:[EDX+4],EAX
          Pop    EAX
          Pop    FS
          Ret
Memory_Manager_Dispose_Descriptor ENDP
;############################################################################
;# Procedura vytvari CallGate descriptor v dane tabulce descriptoru
;############################################################################
;# Vstup  : (BX:EDI) = vstupni bod
;#                       - BX  = Selector na kodovy descriptor
;#                       - EDI = Offset vstupniho bodu
;#           DL = CallGate DPL
;#           DH = CallGate CNT
;#           AL = Tabulka descriptoru, ve ktere chceme vytvorit call gate
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
Memory_Manager_Create_CallGate PROC
          Push FS ESI ECX EDX EAX
           Xor   ESI,ESI
           Or   AL,AL
           Jnz  @MemMan_Create_CallGate_Next
           Mov   Ax,GDT_Descriptor
           Mov   FS,AX
           Mov   ESI,(First_Avaible_Descriptor)-8
@MemMan_Create_CallGate_Next :
           Mov   ECX,8191  ;maximalni pocet descriptoru
@MemMan_LookForDescriptor_Create_Gate :
           Add   ESI,8
           Mov   EAX,Dword ptr FS:[ESI]
           Add   EAX,Dword ptr FS:[ESI+4]
           Jz    @MemMan_FoundDescriptor_Create_Gate
           Dec   ECX
           Jnz   @MemMan_LookForDescriptor_Create_Gate
          Pop   EAX EDX ECX
           Xor   EAX,EAX
           Inc   Al                 ;error code 1 - no free descriptor in table
           Jmp   @MemMan_Create_CallGate_End
@MemMan_FoundDescriptor_Create_Gate :
          Pop   EAX EDX ECX
           Mov   Word ptr FS:[ESI+0],DI
           Mov   Word ptr FS:[ESI+2],BX
           Ror   EDI,16
           Mov   Word ptr FS:[ESI+6],DI
           Rol   EDI,16
           And   DH,00011111b
           Mov   Byte ptr FS:[ESI+4],DH
           Shl   Dl,5
           Or    Dl,8Ch
           Mov   Byte ptr FS:[ESI+5],DL
           Mov   EDX,ESI
           And   Al,1
           Shl   Al,2
           Or    Dl,Al  ;nastaveni TI
           And   Al,3
           Or    Dl,Al  ;nastaveni RPL
           Xor   EAX,EAX
@MemMan_Create_CallGate_End :
          Pop  ESI FS
          Ret
Memory_Manager_Create_CallGate ENDP
;####################################################################################
;# Procedura vytvari alias
;####################################################################################
;# Vstup  : EDX - Selector na ktery ma byt alias
;# Vystup : EDX - selector na alias, if not C
;####################################################################################
Memory_Manager_Create_Alias PROC
         PUSH  FS ECX EAX EDI
          Mov   EAX,GDT_Descriptor
          Mov   FS,EAX
          Mov   EDI, EDX
          Mov   EDX,First_Avaible_Descriptor
          Mov   ECX,8192-(First_Avaible_Descriptor/8)
@MMCA_LookForDescriptor :
          Mov   EAX,Dword ptr FS:[EDX]
          Add   EAX,Dword ptr FS:[EDX+4]
          Jz    @MMCA_FoundDescriptor
          Add   EDX,8
          Dec   ECX
          Jnz   @MMCA_LookForDescriptor
          STC
       POP  EDI EAX ECX FS
         RET
@MMCA_FoundDescriptor :
          Mov   EAX,Dword ptr FS:[EDI]
          Mov   Dword ptr FS:[EDX],EAX
          Mov   EAX,Dword ptr FS:[EDI+4]
          Mov   Dword ptr FS:[EDX+4],EAX
          Mov   Byte ptr  FS:[EDX+5], Data_Atr1
          Mov   Byte ptr  FS:[EDX+6], Data_Atr2
          CLC
       POP  EDI EAX ECX FS
         RET
Memory_Manager_Create_Alias ENDP
;####################################################################################
;# Procedura se pokusi vytvorit novy descriptor v GDT, velikost 4k
;####################################################################################
;# Vstup  : EAX - baze
;# Vystup : If not C then EDX = Selektor
;# Nici   : EDX
;####################################################################################
Create_Descriptor_GDT_Baze PROC
         PUSH   ECX EBX ES
           Mov   ECX,(8192-First_Avaible_Descriptor)  ;maximalni pocet descriptoru
           Mov   EBX,GDT_Descriptor
           Mov   ES,EBX
           Mov   EDX,(First_Avaible_Descriptor-8)
@Create_Descriptor_GDT_Baze_LookForDescriptor :
           Add   EDX,8
           Mov   EBX,Dword ptr ES:[EDX]
           Add   EBX,Dword ptr ES:[EDX+4]
           Jz    @Create_Descriptor_GDT_Baze_FoundDescriptor
           Dec   ECX
           Jnz   @Create_Descriptor_GDT_Baze_LookForDescriptor
           Xor   EDX,EDX
           STC
           Jmp   Short @Create_Descriptor_GDT_Baze_CreateDesc_End
@Create_Descriptor_GDT_Baze_FoundDescriptor :
           Mov   Word ptr ES:[EDX+0], 0
           Mov   Word ptr ES:[EDX+2], AX
           Ror   EAX, 16
           Mov   Byte ptr ES:[EDX+4], AL
           Mov   Byte ptr ES:[EDX+5], Data_Atr1
           Mov   Byte ptr ES:[EDX+6], Data_Atr2
           Mov   Byte ptr ES:[EDX+7], AH
           Ror   EAX, 16
           CLC
@Create_Descriptor_GDT_Baze_CreateDesc_End :
         POP    ES EBX ECX
         RET
Create_Descriptor_GDT_Baze ENDP
