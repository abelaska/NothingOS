;####################################################################################
;# Inicializacni sekvence pro Paging
;####################################################################################

;####################################################################################
;# Procedura nastavuje descriptory, naplni hodnoty v adresarich a podadresarich
;# na linearni seznam
;####################################################################################
;# Vstup  : DS = Data_Descriptor
;#          Musi byt nastavene spravne udaje v promennych Swap_Size a RAM_Size
;# Vystup : Zapne se pageovani
;# Nici   : Nic
;####################################################################################
Page_Initializing PROC
         Pushad
          PUSH    DS ES FS GS
           ;vypocteme velikost pameti pro FysPageInfoItem
           Mov    EAX,Dword ptr DS:[RAM_Size]
           Mov    ECX,FPIT_Size
           Mul    ECX
           Shr    EAX,12
           Mov    Dword ptr DS:[FysPageInfoItem_Size],EAX
           ;vypocteme limitu pro vyvolani swapdemona - 80%
           Mov    EAX,Dword ptr DS:[RAM_Size]
           Xor    EDX,EDX
           Mov    EBX,100
           Div    EBX
           Mov    EBX,80
           Mul    EBX
           Mov    Dword ptr DS:[Pages_Used_Limit],EAX
           ;a ten zbytek
           Mov    EDX,Dword ptr DS:[RAM_Size]
           Shr    EDX,3

           Call   PAGE_InitializeDescriptors
           Call   PAGE_InitMemTab_Null
           Call   PAGE_InitializePages
           Mov    EAX,Dword ptr DS:[Pages_Base]
           Mov    CR3,EAX
           Mov    EAX,CR0
           And    EAX,1FFFFFFFH
           Or     EAX,80000000h
           Mov    CR0,EAX
           Call   Page_ReMap2_G
           Call   Page_Descriptor_FPIT_Initialization
         Mov   EAX,CR3
         Mov   CR3,EAX
         ;provedeme vyprazdneni registeru CS
         Mov     dword ptr DS:[Jmp2Protect],Offset @Jmp_FarAfter_Page_Init
         JMP     fword ptr DS:[JMP2Protect]
@Jmp_FarAfter_Page_Init :
          ;znovunacteni registeru SS
          Mov    EAX,SS
          Mov    SS,EAX
          ;znovunacteni normalnich segmentovych registeru
          POP    GS FS ES DS
          Call   FMPI_Init ;Inicializujeme FileMapedPagesInfo
          Popad
          Ret
Page_Initializing ENDP
;####################################################################################
;# Procedura nastavuje descriptory MemTab a Page
;####################################################################################
;# Vstup  : EDX = pocet byte, ktere je potreba na adresaci pameti FysLin
;# Vystup : EAX - adresa prvniho 4k bloku
;# Nici   : Nic
;####################################################################################
PAGE_InitializeDescriptors PROC
         Push  ES
          Mov   AX,GDT_Descriptor
          Mov   ES,AX
          Shr   EDX,12     ;pocet byte / 4k
                           ;EDX obsahuje pocet stranek, ktere musime alokovat pro MemTab
          Mov   ECX, VMem_Shared_Area_Size
          Shr   ECX,((12+3)+12)  ;12+3 = kolik byte na popis struktury, 12 - kolik 4kstranek ;)

          Mov   EAX, MemTab1_Base
          Mov   Word ptr ES:[Shared_Mem_Descriptor+0], CX
          Mov   Word ptr ES:[Shared_Mem_Descriptor+2], AX
          Ror   EAX, 16
          Mov   Byte ptr ES:[Shared_Mem_Descriptor+4], Al
          Mov   Byte ptr ES:[Shared_Mem_Descriptor+5], MemTab_Atr1
          Mov   Byte ptr ES:[Shared_Mem_Descriptor+6], MemTab_Atr2
          Mov   Byte ptr ES:[Shared_Mem_Descriptor+7], Ah
          Ror   EAX,16
          Inc   ECX
          Shl   ECX,12

          Add   EAX,ECX    ;bazova adresa FysLin_Mem_Descriptor
          Mov   Word ptr ES:[FysLin_Mem_Descriptor+0], DX
          Mov   Word ptr ES:[FysLin_Mem_Descriptor+2], AX
          Ror   EAX, 16
          Mov   Byte ptr ES:[FysLin_Mem_Descriptor+4], Al
          Mov   Byte ptr ES:[FysLin_Mem_Descriptor+5], MemTab_Atr1
          Mov   Byte ptr ES:[FysLin_Mem_Descriptor+6], MemTab_Atr2
          Mov   Byte ptr ES:[FysLin_Mem_Descriptor+7], Ah
          Ror   EAX,16
          Inc   EDX
          Shl   EDX,12
          Add   EAX,EDX   ;Bazova adresa Pages_Descriptor

          Mov   ECX,2
          Mov   Dword ptr DS:[Pages_Base],EAX
          Mov   Word ptr ES:[Pages_Descriptor+0], CX
          Mov   Word ptr ES:[Pages_Descriptor+2], AX
          Ror   EAX, 16
          Mov   Byte ptr ES:[Pages_Descriptor+4], Al
          Mov   Byte ptr ES:[Pages_Descriptor+5], Data_Atr1
          Mov   Byte ptr ES:[Pages_Descriptor+6], Data_Atr2
          Mov   Byte ptr ES:[Pages_Descriptor+7], Ah
          ROR   EAX,16
          Inc   ECX
          Shl   ECX,12
          Add   EAX,ECX

         ;nastaveni FysPageInfoItem
          Mov   ECX,Dword ptr DS:[FysPageInfoItem_Size]
          Mov   Word ptr ES:[FysPageInfoItem_Descriptor+0], CX
          Mov   Word ptr ES:[FysPageInfoItem_Descriptor+2], AX
          Ror   EAX, 16
          Mov   Byte ptr ES:[FysPageInfoItem_Descriptor+4], Al
          Mov   Byte ptr ES:[FysPageInfoItem_Descriptor+5], Data_Atr1
          Mov   Byte ptr ES:[FysPageInfoItem_Descriptor+6], Data_Atr2
          Mov   Byte ptr ES:[FysPageInfoItem_Descriptor+7], Ah
          ROR   EAX,16
          Inc   ECX
          Shl   ECX,12
          Add   EAX,ECX  ;vystup
          Mov   ECX,EAX
          Shr   ECX,12
          Mov   EDX,1024
          Sub   EDX,ECX
          Sub   EDX,((100000h-0A0000h)/4096)
          Mov   Dword ptr DS:[Pages_Used],ECX
          Mov   Dword ptr DS:[Free_FysPages_Count],EDX
         Pop  ES
          Ret
PAGE_InitializeDescriptors ENDP
;####################################################################################
;# Procedura nastavuje informace do Page Structury
;####################################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;####################################################################################
PAGE_InitializePages PROC
          Mov   AX,Pages_Descriptor
          Mov   ES,AX
          Mov   AX,Data_Descriptor
          Mov   DS,AX
          Mov   ECX,(4096 Shr 2)
          Mov   EAX,PagesMainFlags_NP
          Xor   EDI,EDI
          Rep   Stosd
          Mov   EAX,Dword ptr DS:[Pages_Base]
          Add   EAX,4096
          And   AX,0F000h
          Or    EAX,PagesMainFlags
          Mov   Dword ptr ES:[0],EAX          ;prvni podadresar 0..4M
          Add   EAX,4096
          Mov   Dword ptr ES:[1024],EAX        ;druhy podadresar 1G..1G+4M
          Mov   ECX,(4096 Shr 1)
          Mov   EAX,PAGE_CONST_NP_NORMAL_MEMORY
          Rep   Stosd
          ;nastavime nektere page na non-swap
          Mov   EBX,_PAGE_NonSwapable
          Mov   EDX,Pages_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,FysPageInfoItem_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,GDT_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,IDT_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,FysLin_Mem_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,BIOS_Area_Descriptor
          Call  PAGES_SetPages
          ;nastavime jako normal-swapable :)
          Mov   EBX,_PAGE_NormalSwap
          Mov   EDX,Data_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,Stack_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,Shared_Mem_Descriptor
          Call  PAGES_SetPages
          Mov   EDX,Code_Descriptor
          Call  PAGES_SetPages
         RET
@Page_Error :
          Mov   EAX,er_MM +0002h
          Call  Fatal_Error
PAGE_InitializePages ENDP
;####################################################################################
;# Procedura nastavuje do tabulek Mem_Tab1 a Mem_Tab2, nuluje tabulku Pages_Descriptor
;####################################################################################
;# Vstup  : DS   - Data Segment Kernelu
;#          EAX  - addresa prvniho volneho bloku
;# Vystup : None
;# Nici   : None
;####################################################################################
PAGE_InitMemTab_Null PROC
         PUSH   ES
         PUSH   DS
          Mov   EDX,EAX
          Shr   EDX,12
          Xor   EBX,EBX
          Mov   Bl,DL
          And   BL,0000111b
          Shr   EDX,3
          Xor   EDI,EDI
          Mov   ECX,Shared_Mem_Descriptor
          Mov   ES,CX
          LSL   ECX,ECX
          Xor   Al,Al
          Rep   Stosb
          Mov   ECX,EDX
          Mov   Al,0FFh
          Xor   EDI,EDI
          Rep   Stosb
          Or    EBX,EBX
          Jz    @PAGE_InitMemTab_Null_Skip
          Dec   EBX
          Mov   Al,Byte ptr DS:[JohnBit_Table2+EBX]
          Or    Byte ptr ES:[EDI],Al
@PAGE_InitMemTab_Null_Skip :
;MemTab1 by mel byt nastaven, MemTab2 = MemTab1
          PUSH  ES
          POP   DS
          Mov   ECX,FysLin_Mem_Descriptor
          Mov   ES,ECX
          LSL   ECX,ECX
          PUSH  ECX
           Xor   EDI,EDI
           Xor   Al,Al
           Rep   Stosb
          POP   ECX
          MOV   EBX,DS
          LSL   EBX,EBX
          Cmp   EBX,ECX
          JA    @MemCopy_Non_Change
          Mov   ECX,EBX
@MemCopy_Non_Change :
          Xor   ESI,ESI
          Xor   EDI,EDI
          Rep   Movsb
          Mov    EDI,20
          Mov    ECX,12
          Mov    AL,0FFh
          Rep    Stosb     ;nastaveni BIOSu...640k...

         ;vynulujeme FysPageInfoItem strukturu
          Mov   ECX,FysPageInfoItem_Descriptor
          Mov   ES,CX
          Xor   EDI,EDI
          LSL   ECX,ECX
          Inc   ECX
          Shr   ECX,2
          Xor   EAX,EAX
          Rep   Stosd
         POP    DS
         POP    ES
          Ret
PAGE_InitMemTab_Null ENDP
;########################################################################
;# Procedura nastavi descriptory pro praci nad 1G
;########################################################################
PAGE_ReMap2_G PROC
          ;nastavime jako non-swapable :)
          Mov   EBX,_PAGE_NormalSwap
          Mov   EDX,Data_Descriptor
          Call  PAGES_SetPages2
          Mov   EDX,Stack_Descriptor
          Call  PAGES_SetPages2
          Mov   EDX,Shared_Mem_Descriptor
          Call  PAGES_SetPages2
          Mov   EDX,Code_Descriptor
          Call  PAGES_SetPages2
          Ret
PAGE_ReMap2_G ENDP
;########################################################################
;# Procedura nastavi vsechny page daneho descriptoru na hodnotu
;# present, nonswap
;########################################################################
;# Vstup : EDX - selektor na descriptor
;#         EBX - typ swapu
;########################################################################
PAGES_SetPages PROC
         PUSH  EAX EBX EDX ECX EDI
           Mov   ECX,EDX
           Call  MemMan_ReturnDescriptorBaseAddress
           Mov   DX,Pages_descriptor
           Mov   ES,DX
           Mov   EDI,EAX
           Or    DI,1
           Shr   EAX,10
           Mov   EDX,4096
           Add   EDX,EAX
           LSL   ECX,ECX
           Inc   ECX
           Shr   ECX,12
           ;EDX = offset v Pages_descriptor
           ;ECX = pocet page, ktere budeme upravovat :)
           SHL   EBX,9
@PAGES_SetPages_Loop :
           Mov   EAX,Dword ptr ES:[EDX]
           Or    EAX,EBX
           OR    EAX,EDI
           Mov   Dword ptr ES:[EDX],EAX
           Add   EDX,4
           Add   EDI,4096
           Dec   ECX
           Jnz   Short @PAGES_SetPages_Loop
         POP   EDI ECX EDX EBX EAX
          Ret
PAGES_SetPages ENDP
;########################################################################
;# Procedura nastavi vsechny page daneho descriptoru na hodnotu
;# present, nonswap ale page od 1G, a k bazi descriptoru pricte 1G
;########################################################################
;# Vstup : EDX - selektor na descriptor
;#         EBX - typ swapu
;########################################################################
PAGES_SetPages2 PROC
         PUSH  EAX EBX EDX ECX EDI
         PUSH  EDX
           Mov   ECX,EDX
           Call  MemMan_ReturnDescriptorBaseAddress
           Mov   EDI,EAX
           Add   EDI,VMem_Shared_Area
           Call  MemMan_SetDescriptorBaseAddress
           Sub   EDI,VMem_Shared_Area
           Or    DI,1
           Shr   EAX,12
           Shl   EAX,2
           Mov   DX,Pages_descriptor
           Mov   GS,DX
           Mov   EDX,4096+4096
           Add   EDX,EAX
           LSL   ECX,ECX
           Inc   ECX
           Shr   ECX,12
           ;EDX = offset v Pages_descriptor
           ;ECX = pocet page, ktere budeme upravovat :)
           SHL   EBX,9
@PAGES_SetPages_Loop2 :
           Mov   EAX,Dword ptr GS:[EDX]
           Or    EAX,EBX
           OR    EAX,EDI
           Mov   Dword ptr GS:[EDX],EAX
           Add   EDX,4
           Add   EDI,4096
           Dec   ECX
           Jnz   Short @PAGES_SetPages_Loop2

         Mov   EAX,GDT_Descriptor
         Mov   FS,EAX

         Create_Temp_Descriptor EAX
         Mov   EBX,EDX
         POP  EDX
         Mov   EAX,Dword ptr FS:[EDX]
         Mov   Dword ptr FS:[EBX],EAX
         Mov   EAX,Dword ptr FS:[EDX+4]
         Mov   AH,Data_Atr1
         Mov   Dword ptr FS:[EBX+4],EAX
         Mov   ECX,EBX
         Mov   ES,ECX
         LSL   ECX,ECX
         Inc   ECX
         Shr   ECX,12
         Xor   EBX,EBX
@PAGES_Invalidation_Loop :
          Invlpg  Byte ptr ES:[EBX]
          Add     EBX,4096
          Dec     ECX
          Jnz     @PAGES_Invalidation_Loop

          Dispose_Temp_Descriptor ES
         POP   EDI ECX EDX EBX EAX
          Ret
PAGES_SetPages2 ENDP
;########################################################################
;# Procedura nastavi FPIT inicializacni hodnoty pro descriptory kernelu
;########################################################################
Page_Descriptor_FPIT_Initialization PROC
          Mov   EDX,Pages_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,FysPageInfoItem_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,GDT_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,IDT_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,FysLin_Mem_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,BIOS_Area_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,Data_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,Stack_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,Shared_Mem_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
          Mov   EDX,Code_Descriptor
          Call  FPIT_PAGE_Set_Kernels_Descriptor
         Ret
Page_Descriptor_FPIT_Initialization ENDP
;####################################################################################################






