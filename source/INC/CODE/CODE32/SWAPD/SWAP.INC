;###########################################################################
;# Rutiny SwapDeamona
;###########################################################################

;#####################################################################################
;# Zakladni Inicializacni procedura SwapD
;#####################################################################################
SwapDeamon_Initializing PROC
         PUSH DS ES
          Mov  AX,DATA_Descriptor
          Mov  DS,AX
          Mov  ES,Word ptr DS:[part_sel]
          Mov  ECX,Dword ptr DS:[swap_partition_num]
          OR   ECX,ECX
          JZ   @SwapDeamon_Initializing_Error
          Shr  ECX,16
          Xor  EBX,EBX
          Mov  Bl,Ch
          Dec  EBX
          Mov  EAX,Dword ptr DS:[Part_Per_Disk]
          Mul  EBX
          Mov  EBX,EAX
          Xor  EAX,EAX
          Mov  Al,Cl
          Dec  Al
          Mov  ECX,Partition_Info_Size
          Mul  ECX
          Add  EBX,EAX ;ES:EBX nyni ukazuje na zacatek informaci o partitione
          Mov  EAX,Dword ptr ES:[EBX.Partition_Size]
          Shr  EAX,3   ;kolik 4k bloku
          Mov  Dword ptr DS:[Swap_Partition_Size],EAX
          Mov  Dword ptr DS:[Swap_Partition_Free],EAX
         PUSH  EAX
          Shr  EAX,3   ;kolik bytu na popis ? :)
          Call Kalloc
          Mov  Dword ptr DS:[Swap_Partition_Descriptor],EAX
          Mov  ES,EAX
          Mov  Al,80h
          Mov  Byte ptr ES:[0],AL
          Dec  Dword ptr DS:[Swap_Partition_Free]
         POP   EAX
          Shr  EAX,8
          Mov  Dword ptr DS:[Swap_Partition_Size_MB],EAX
          Mov  Dword ptr DS:[Swap_Style],Swap_Mode_Active
          Print Swap_Init_txt
         POP  ES DS
          Ret
@SwapDeamon_Initializing_Error :
          Mov  Dword ptr DS:[Swap_Style],Swap_Mode_Pasive
          Print Swap_Init_Error_txt
          POP  ES DS
          RET
SwapDeamon_Initializing ENDP
;#####################################################################################
;# Je hodne malo pameti, takze bychom meli udelat cistu :)
;#####################################################################################
;# Vstup  : None
;# Vystup : If C Then Error
;#####################################################################################
SwapDeamon_Wake_Up_Monster PROC
           PUSHAD
           PUSH   GS DS ES
            Mov   EAX,Data_Descriptor
            Mov   GS,EAX
            Mov   EAX,FysPageInfoItem_Descriptor
            Mov   DS,EAX
            Mov   EAX,FysPageInfoItem_Descriptor
            Mov   ES,EAX
            Xor   EDI,EDI
            Mov   ECX,Dword ptr GS:[RAM_Size]
            Mov   EBX,FPIT_Size
           POP    ES DS GS
           POPAD
          RET
SwapDeamon_Wake_Up_Monster ENDP
;#####################################################################################
;# Procedura swapne presne danou fyzickou page
;#####################################################################################
;# Vstup : EAX - cislo 4k page v poradi
;#####################################################################################
SwapDeamon_Swap_Page PROC
          RET
SwapDeamon_Swap_Page ENDP
;#####################################################################################
;# Procedura vytahne swaplou page
;#####################################################################################
;# Vstup  : ES  - kam se natahne page :)
;#          EDX - obraz page
;# Vystup : V page jsou data natazene ze swapu
;# Nici   : Nic
;#####################################################################################
SwapDeamon_Restore_Page PROC
       PUSH  DS FS EBX EDI ECX EDX EAX
        Mov   EAX,Data_Descriptor
        Mov   DS,EAX
        Mov   FS,Dword ptr DS:[Swap_Partition_Descriptor]
        Shr   EDX,12  ;EDX = cislo ente 4k page ve swapu
        Mov   EDI,EDX
        Shr   EDI,3   ;EDI = cislo byte v Swap_Partition_Descriptor
        Xor   EBX,EBX
        Mov   Bl,Dl
        And   Bl,7
        Mov   Al,Byte ptr DS:[SetBit_Table2_Non]
        Mov   Ah,Byte ptr DS:[EDI]
        And   Byte ptr DS:[EDI],AL
        Or    Ah,Al
        Inc   Ah
        Jnz   @SwapDeamon_Restore_Page_Panic
        Mov   ECX,(4096 / 512)
        Xor   EDI,EDI
        Shl   EDX,3   ;LBA adresa od zacatku partitiony
        Mov   EAX,Dword ptr DS:[swap_partition_num]
        Mov   EBX,Drv_Std_Read
        Call  Drv_Gate
       POP   EAX EDX ECX EDI EBX FS DS
          RET
@SwapDeamon_Restore_Page_Panic :
        Mov   EAX,er_Swap +0000h
        Call  Fatal_error
SwapDeamon_Restore_Page ENDP
;#####################################################################################
;# Procedura swapne swaplou page mapovanou na soubor
;#####################################################################################
;# Vstup : ES  - kam se natahne page
;#         EDX - obraz page
;#####################################################################################
SwapDeamon_Swap_FileMaped_Page PROC
           RET
SwapDeamon_Swap_FileMaped_Page ENDP
;#####################################################################################
;# Procedura vytahne swaplou page mapovanou na soubor
;#####################################################################################
;# Vstup : ES  - kam se natahne page
;#         EDX - obraz page
;#####################################################################################
SwapDeamon_Restore_FileMaped_Page PROC
           RET
SwapDeamon_Restore_FileMaped_Page ENDP

