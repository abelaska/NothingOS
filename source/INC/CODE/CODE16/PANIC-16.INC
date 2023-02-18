;#################################################################################
;# Rutiny pro zobrazovani Fatal Erroru v 16-bitovem prostredi
;#################################################################################

; Zobrazi error hlasku a zastavi pocitac
; Vstup: EAX - error number
Fatal_Error_16 Proc
      call  Clrscr_16
      mov   ebx, Data_Base shl 4
      mov   ds, bx

      mov   ebx, eax

      mov   esi, offset Error_MSG_Table
Fatal_Error_16_find:
      lodsd
      or    eax, eax
      jz    short Fatal_Error_16_Found
      cmp   eax, ebx
      jz    short Fatal_Error_16_Found
      add   esi, 4
      jmp   short Fatal_Error_16_find

Fatal_Error_16_Found:
      lodsd
      mov   esi, eax
      xor   edi, edi
      call  Write_PChar_16
      hlt
Fatal_Error_16 Endp

