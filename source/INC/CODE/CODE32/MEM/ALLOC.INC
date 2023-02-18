;############################################################################
;# Jednotka obsahuje procedury Allocate a Deallocate Memory
;############################################################################

;############################################################################
;# Procedura alokuje pamet
;############################################################################
;# Vstup  : EDX - velikost ve 4kb strankach
;#          EAX - 0 - alokovat do sdilene pameti
;#              - 1 - alokovat do fyslin
;#              - 2 - alokovat do adresoveho prostoru taskmana
;############################################################################
;# Vystup : if C then EAX - Error Code
;#          EDX - selector na alokovany deskriptor, je-li EAX = 0
;############################################################################
;# Nici registery :
;############################################################################
;# Poznamka : Implicitne se vytvari datovy descriptor...
;#            Potrebuje-li OS codovy segment, tak at nejprve allokuje pamet,
;#            pak do ni nahraje z HardDisku program a nasledne zavola
;#            proceduru Memory_Manager_Data2Code_Descr. Stejnetak musi OS
;#            volat proceduru Memory_Manager_Data2Stack_Descr, jestlize
;#            potrebuje zasobnikovy segnemt :)) Isn't it easy ???
;############################################################################
Memory_Manager_Allocate_Memory PROC
         OR     EDX,EDX ;# Test spravnych vstupnich parametru
         Jz     @MemMan_Allocate_Error_InvalidInputValues
       PUSH    ES EBX EAX
         Mov    EBX,Shared_Mem_Descriptor
         OR     EAX,EAX
         Jz    @MemMan_Not_Fysical_Memory
         Mov     EBX,FysLin_Mem_Descriptor
      @MemMan_Not_Fysical_Memory :
         Mov    ES,EBX
         Cmp    EDX,8
         JNA    Short @MemMan_Allocate_32k_Below
      @MemMan_Allocate_32k_Above :
         Call   Memory_Manager_Internal_Allocate_Memory_Above
         Jmp    Short @MemMan_Allocate_Exit
      @MemMan_Allocate_32k_Below :
         Call   Memory_Manager_Internal_Allocate_Memory_Below
      @MemMan_Allocate_Exit :
        POP   EBX
        Test  EBX,0FFFFFFFFh
        Jz    @MemMan_Not_Fysical_Memory2
        Mov   EBX,_PAGE_NonSwapable
        Call  PAGE_SetPages_Fys_And_AVL
        Call  FPIT_PAGE_Set_Kernels_Descriptor
@MemMan_Not_Fysical_Memory2 :
        POP   EBX ES
        OR    EAX,EAX
        Jz    Short @MemMan_Not_Fysical_Memory_Non_Error
        STC
        RET
@MemMan_Not_Fysical_Memory_Non_Error :
        CLC
         RET
@MemMan_Allocate_Error_InvalidInputValues :
;#Error Code 1 - Invalid Input Parameters
         Xor    EAX,EAX
         Inc    Al
         STC
         Ret
Memory_Manager_Allocate_Memory ENDP
;############################################################################
;# Procedura dealokuje pamet - nici descriptor
;############################################################################
;# Vstup  : EDX  - Selector descriptoru pro dealokovani
;############################################################################
Memory_Manager_DeAllocate_Memory PROC
         PUSH  EDX EBX
          Call  PAGE_SetPageAS_NotUsed
          Call  Memory_Manager_Dispose_Descriptor
         POP   EBX EDX
         RET
Memory_Manager_DeAllocate_Memory ENDP
;############################################################################
;# Procedura alokuje pamet, EDX > 8
;############################################################################
;# Vstup  : EDX - velikost ve 4kb strankach
;# Vstup  : EAX - typ deskriptoru (tzn do ktere Tabulky Deskriptoru se ma zapisovat)
;#                 0 = Globalni deskriptor ve shared mem
;#                 1 = globalni descriptor ve fyslin mem
;#                 2 = Alokovat v Descriptorovem prostoru TaskMana
;#          ES  - bitova tabulka
;############################################################################
;# Vystup : EAX - Error Code
;#          EDX - selector na alokovany deskriptor, je-li EAX = 0
;############################################################################
;# Nici registery :
;############################################################################
Memory_Manager_Internal_Allocate_Memory_Above PROC
         PUSH    DS
         PUSH    EDI
         PUSH    ESI
         PUSH    EBX
         PUSH    ECX
         PUSH    EDX
         PUSH    EAX
          Mov    Bx,Data_Descriptor
          Mov    DS,BX                              ;DS = Data Descriptor
          Xor    EBX,EBX                            ;EBX = cislo masky
          Mov    CL,8                               ;CL = pocitadlo masek
          Mov    EDI,Offset Memory_Allocate_Masks00 ;EDI = zacatek masek
          Mov    ESI,EDX
          Sub    ESI,9                              ;ESI = pocet 4k stranek - (8 + 1)
          Xor    EAX,EAX
@MemMan_IAMA_CreateMasks :
          Mov    EDX,ESI
          Add    EDX,EBX
          Mov    Al,Dl
          Shr    EDX,3                               ;EDX = pocet byte mezi
          Mov    Dword ptr DS:[EDI+EBX*8+Alocate_Mask_Array_Len],EDX
          Mov    EDX,EAX
          And    DL,7
          Mov    Al,Byte ptr DS:[JohnBit_Table2.EDX] ;AL = koncova maska
          Mov    Byte ptr DS:[EDI+EBX*8+Alocate_Mask_End_Mask],AL
          Inc    EBX
          Dec    Cl
          Jnz    Short @MemMan_IAMA_CreateMasks
          Mov    EDX,EDI                                 ;EDX = Zacatek Masek
          Xor    EDI,EDI                                 ;Zacneme od nuly
          Mov    ECX,ES
          LSL    ECX,ECX
@MemMan_IAMA_LookFor_NonZero_Byte :
          Mov    Al,0FFh                                 ;Hledame AL <> $FF
          Repz   ScasB
          Jz     @MemMan_IAMA_Error_NoFreeMemory
          Mov    ESI,EDI
          Dec    ESI                                    ;prvni byte, kde je nula
          Mov    Al,Byte ptr ES:[ESI]
          Xor    EBX,EBX
          Mov    AH,8
@MemMan_IAMA_TestForMask :
          Test   Al,Byte ptr DS:[EDX+EBX*8+Alocate_Mask_Start_Mask]
          Jz     Short @MemMan_IAMA_FoundMask
          Inc    EBX
          Dec    AH
          Jnz    Short @MemMan_IAMA_TestForMask
          Jmp    Short @MemMan_IAMA_LookFor_NonZero_Byte
@MemMan_IAMA_FoundMask :
          PUSH   ECX
           Mov   ECX,ES
           LSL   ECX,ECX
           SUB   ECX,ESI
           Sub   ECX,Dword ptr DS:[EDX+EBX*8+Alocate_Mask_Array_Len]
           SHL   ECX,1
           JC    @MemMan_IAMA_Error_NoFreeMemory3
           Mov    ECX,Dword ptr DS:[EDX+EBX*8+Alocate_Mask_Array_Len]
           Or     ECX,ECX
           Jz     Short @MemMan_IAMA_SkipInterArray
           Xor    Al,Al
           Repz   ScasB
           Jz     Short @MemMan_IAMA_SkipInterArray
          POP    ESI
           Sub    ECX,Dword ptr DS:[EDX+EBX*8+Alocate_Mask_Array_Len]
           Neg    ECX
           Sub    ECX,ESI
           Jmp    @MemMan_IAMA_LookFor_NonZero_Byte
@MemMan_IAMA_SkipInterArray :
          POP    ECX
           Sub    ECX,Dword ptr DS:[EDX+EBX*8+Alocate_Mask_Array_Len]
           Mov    Al,Byte ptr ES:[EDI]
           Inc    EDI
           Test   Al,Byte ptr DS:[EDX+EBX*8+Alocate_Mask_End_Mask]
           Jnz    @MemMan_IAMA_LookFor_NonZero_Byte
@MemMan_IAMA_Found_Block :
;# nasli jsme blok pameti, zacatek na ESI, typ masek EBX
         POP     EAX
         POP     ECX
          Call    MemMan_CreateDescriptor
          OR      EAX,EAX
          Jnz     Short @MemMan_IAMA_END

          Mov     ECX,Dword ptr DS:[EDX+EBX*8+Alocate_Mask_Array_Len]
          Mov     Al,Byte ptr DS:[EDX+EBX*8+Alocate_Mask_Start_Mask]
          Or      Byte ptr ES:[ESI],Al
          Inc     ESI
          Or      ECX,ECX
          Jz      Short @MemMan_IAMA_Skip_InterArray2
          Mov     Al,0FFh
@MemMan_IAMA_Fill_InterArray :
          Or      Byte ptr ES:[ESI],Al
          Inc     ESI
          Dec     ECX
          Jnz     Short @MemMan_IAMA_Fill_InterArray
@MemMan_IAMA_Skip_InterArray2 :
          Mov     Al,Byte ptr DS:[EDX+EBX*8+Alocate_Mask_End_Mask]
          Or      Byte ptr ES:[ESI],Al
          Mov     EDX,EDI
          Jmp     Short @MemMan_IAMA_OK
@MemMan_IAMA_Error_NoFreeMemory3:
         POP     ECX
@MemMan_IAMA_Error_NoFreeMemory :
         POP     EAX
         POP     EDX
@MemMan_IAMA_Error_NoFreeMemory2 :
          Xor     EAX,EAX
          Add     Al,2 ;Error code 2 - no free memory
          Jmp     Short @MemMan_IAMA_END
@MemMan_IAMA_OK  :
          Xor     EAX,EAX
@MemMan_IAMA_END :
         POP     ECX
         POP     EBX
         POP     ESI
         POP     EDI
         POP     DS
          Ret
Memory_Manager_Internal_Allocate_Memory_Above ENDP
;############################################################################
;# Procedura Alokuje Pamet, EDX <= 8
;############################################################################
;# Vstup  : EDX - velikost ve 4kb strankach
;# Vstup  : EAX  - typ deskriptoru (tzn do ktere Tabulky Deskriptoru se ma zapisovat)
;#                 0 = Globalni deskriptor ve shared mem
;#                 1 = globalni descriptor ve fyslin mem
;#                 2 = Alokovat v Descriptorovem prostoru TaskMana
;#          ES  - bitova tabulka
;############################################################################
;# Vystup : EAX - Error Code
;#          EDX - selector na alokovany deskriptor, je-li EAX = 0
;############################################################################
;# Nici registery :
;############################################################################
Memory_Manager_Internal_Allocate_Memory_Below PROC
         PUSH    DS
         PUSH    EDI
         PUSH    ESI
         PUSH    EBX
         PUSH    ECX
        PUSH    EDX
        PUSH    EAX
          Mov    Bx,Data_Descriptor
          Mov    DS,BX
          Mov    EDI,Offset Memory_Allocate_Mask_8less ;Offset masek
          Xor    EBX,EBX                               ;pozice masky
          Mov    CL,9                                 ;cislo masky
          Xor    EAX,EAX
@MemMan_IAMB_CreateMask0 :
          STC
          RCR    AX,1
          Dec    EDX
          Jnz    Short @MemMan_IAMB_CreateMask0
@MemMan_IAMB_CreateRestMask :
          Mov    Word ptr DS:[EDI+EBX],AX
          Add    EBX,2
          ROR    EAX,1
          Dec    Cl
          Jnz    Short @MemMan_IAMB_CreateRestMask
          Mov    EDX,EDI                                 ;EDX = Zacatek Masek
          Xor    EDI,EDI                                 ;Zacneme od nuly
          Mov    ECX,ES
          LSL    ECX,ECX
          Sub    ECX,2
@MemMan_IAMB_LookFor_NonZero_Byte :
          Mov    Al,0FFh                                 ;Hledame AL <> $FF
          Repz   ScasB
          Jz     @MemMan_IAMB_Error_NoFreeMemory;Short @MemMan_IAMB_Error_NoFreeMemory
          Mov    ESI,EDI
          Dec    ESI
          Mov    Ah,Byte ptr ES:[ESI]
          Mov    Al,Byte ptr ES:[EDI]
         PUSH    ECX
          Xor    EBX,EBX
          Mov    Cl,8
@MemMan_IAMB_TestMask :
          Test   AX,Word ptr DS:[EDX+EBX]
          Jz    Short @MemMan_IAMB_Found
          Add    Bl,2
          Dec    Cl
          Jnz    Short @MemMan_IAMB_TestMask
         POP     ECX
          Jmp    Short @MemMan_IAMB_LookFor_NonZero_Byte
@MemMan_IAMB_Found:
         POP     ECX
;#nasli jsme, byte = esi, pozice = bx
          Shr    EBX,1
         POP     EAX
         POP     ECX
          Call    MemMan_CreateDescriptor
          OR      EAX,EAX
          Jnz     Short @MemMan_IAMB_END
          Shl     EBX,1
          Mov     Ax,Word ptr DS:[EDX+EBX]
          Or      Byte ptr ES:[ESI],Ah
          Or      Byte ptr ES:[ESI+1],AL
          Mov     EDX,EDI
          Jmp     Short @MemMan_IAMB_OK
@MemMan_IAMB_Error_NoFreeMemory :
         POP     EAX
         POP     EDX
@MemMan_IAMB_Error_NoFreeMemory2 :
          Xor     EAX,EAX
          Add     Al,2 ;Error code 2 - no free memory
          Jmp     Short @MemMan_IAMB_END
@MemMan_IAMB_OK  :
          Xor     EAX,EAX
@MemMan_IAMB_END :
         POP     ECX
         POP     EBX
         POP     ESI
         POP     EDI
         POP     DS
          Ret
Memory_Manager_Internal_Allocate_Memory_Below ENDP
