;################################################################################
;# Knihovna pro praci s privatni oblasti tasku
;################################################################################

;################################################################################
;# Procedura alokuje privatni pamet do LDTcka, RPL3, DPL3 po bytech
;################################################################################
;# Vstup  : EDX - pocet byte
;# Vystup : C = 0 --> EDX = Selector
;#          C = 1 --> EDX = Error Code
;#                          00000001 - Neni Descriptor
;#                          00000002 - Nedostatek virtualni pameti
;################################################################################
TSKMM_Alloc_B PROC
       PUSH   EAX EBX
        OR    EDX, EDX
        JZ    Short @TSKMM_Alloc_b_end
        Dec   EDX
        Shr   EDX, 12
        Inc   EDX
        Call  TSKMM_Alloc
  @TSKMM_Alloc_b_end:
       POP    EBX  EAX
        ret
TSKMM_Alloc_B ENDP


;################################################################################
;# Procedura alokuje privatni pamet do LDTcka, RPL3, DPL3
;################################################################################
;# Vstup  : EDX - pocet 4k stranek
;# Vystup : C = 0 --> EDX = Selector
;#          C = 1 --> EDX = Error Code
;#                          00000001 - Neni Descriptor
;#                          00000002 - Nedostatek virtualni pameti
;################################################################################
TSKMM_Alloc PROC
        PUSH  ESI EDI EAX EBX ECX
        PUSH    ES FS
          Mov   EAX,EDX
          Xor   EDX,EDX
          SLDT  DX
          Call  Memory_Manager_Create_Alias
          Jc    @TSKMM_Alloc_Error_No_Descriptor
          Mov   FS,EDX
          Mov   EDX,Tsk_Mem_Layout_Descriptor
          Mov   ES, EDX
          Xor   EDX,EDX
          Mov   ECX,(8192-1)
          Xor   ESI,ESI
          ;# FS  - Datovy alias na LDT
          ;# ES  - Mem Layout Descriptor
          ;# EDX - offset v tabulce - zacneme prohledavat tabulku od 1. zaznamu
          ;# EAX - pocet pageu pocitano od 1
          ;# ECX - Pocet zaznamu v tabulce
@TSKMM_Alloc_Loop :
          Add   EDX,TMA_Size
          Cmp   Word ptr ES:[EDX.TMA_Selector],0
          Je    Short @TSKMM_Alloc_Loop_Found_Free_Record
          Dec   ECX
          Jnz   Short @TSKMM_Alloc_Loop
          Jmp   @TSKMM_Alloc_No_Descriptor_LDT
     @TSKMM_Alloc_Loop_Found_Free_Record :
          Cmp   Dword ptr ES:[EDX.TMA_Block_Size],EAX
          Ja    Short @TSKMM_Alloc_Loop_Found_Record_Above
          Je    Short @TSKMM_Alloc_Loop_Found_Record_Equal
          Mov   ESI,EDX
          Dec   ECX
          Jnz   Short @TSKMM_Alloc_Loop
          Jmp   @TSKMM_Alloc_No_Memory
@TSKMM_Alloc_Loop_Found_Record_Above :
          ;# Nasli jsme block, ktery ma vetsi velikost
          ;# Offset bloku je v EDX, ted musime najit jeden volny zaznam
          ;# Do ktereho ulozime novy block
          ;# Je-li ESI nenulove znamena to, ze je pred timto blokem nejaky volny
          Or    ESI,ESI
          Jnz   @TSKMM_Alloc_Find_Next_Found
          Mov   ESI,EDX
@TSKMM_Alloc_Find_Next :
          Add   ESI,TMA_Size
          Cmp   Word ptr ES:[ESI.TMA_Selector],0
          Je    Short @TSKMM_Alloc_Find_Next_Found
          Dec   ECX
          Jnz   Short @TSKMM_Alloc_Find_Next
          Jmp   @TSKMM_Alloc_No_Memory
@TSKMM_Alloc_Find_Next_Found :
          Mov   ECX,EAX
          Sub   Dword ptr ES:[EDX.TMA_Block_Size],EAX
          Mov   Dword ptr ES:[ESI.TMA_Block_Size],EAX
          Mov   EDI,EAX
          Shl   EDI,12
          Mov   EAX,Dword ptr ES:[EDX.TMA_Block_Start]
          Add   Dword ptr ES:[EDX.TMA_Block_Start],EDI
          Mov   Dword ptr ES:[ESI.TMA_Block_Start],EAX
          Call  TSKMM_CreateDescriptor
          Jc    @TSKMM_Alloc_No_Descriptor_LDT
          Mov   Word ptr ES:[ESI.TMA_Selector],DX
          Jmp   Short @TSKMM_Alloc_OK
@TSKMM_Alloc_Loop_Found_Record_Equal :
          ;# Nasli jsme block, ktery ma stejnou velikost jako je ta, kterou
          ;# Potrebujeme :) takze staci cely zaznam zmenit na used...
          Mov   ECX, EAX
          Mov   EAX, Dword ptr ES:[EDX.TMA_Block_Start]
          Mov   EDI, EDX
          Call  TSKMM_CreateDescriptor
          Jc    @TSKMM_Alloc_No_Descriptor_LDT
          Mov   word ptr ES:[EDI.TMA_Selector],DX
@TSKMM_Alloc_OK :
;#
;#
;# Jeste musime dodelat pripocitani do TSS.TSS_Data_Memory
;#
;#
          Mov   EDI,EDX
          Mov   EDX,FS
          POP   FS ES
          Call  Memory_Manager_Dispose_Descriptor
          Mov   EDX,EDI
          POP   ECX EBX EAX EDI ESI
         CLC
         RET
;######################################################################
;# Errors
;######################################################################
@TSKMM_Alloc_No_Memory :
        Mov   EDX,FS
        POP   FS ES
        Call  Memory_Manager_Dispose_Descriptor
        POP   ECX EBX EAX EDI ESI
        Mov    EDX,2
         STC
         RET
@TSKMM_Alloc_No_Descriptor_LDT :
        Mov   EDX,FS
        POP   FS ES
        Call  Memory_Manager_Dispose_Descriptor
        POP   ECX EBX EAX EDI ESI
        Mov    EDX,1
         STC
         RET
@TSKMM_Alloc_Error_No_Descriptor :
        POP    FS ES
        POP   ECX EBX EAX EDI ESI
        Mov    EDX,1
         STC
         RET
TSKMM_Alloc ENDP
;################################################################################
;# Procedura dealokuje privatni pamet do LDTcka, RPL3, DPL3
;################################################################################
;# Vstup  : EDX - Selector na dealokovani
;################################################################################
TSKMM_Dealloc PROC
;# procedura musi uvolnit i swaple page - to je rozdil oproti procedure deallokace
;# ve sdilene pameti, tam se nastavi pouze na free. to my udelame taky, ale predtim
;# do fyspageinfotable nastavit hodnotu 10 - neukazuje nikam, muze se normalne uvolnit
;# chce-li uvolnit descriptor v LDT, jehoz baze ukazuje pod zacatek privatni oblasti,
;# tak zavolame fci dealloc normalni
;# Odecist v TSS.TSS_Data_Memory
         RET
TSKMM_Dealloc ENDP