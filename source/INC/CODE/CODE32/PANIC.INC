;###########################################################################
;# Rutiny pro zobrazeni Fatal Erroru
;###########################################################################

; Zobrazi error hlasku a zastavi pocitac
; Vstup: EAX - error number
Fatal_Error Proc
      mov   bx, Data_Descriptor
      mov   ds, bx
      mov   ebx, eax

      mov   esi, offset Error_MSG_Table
Fatal_Error_find:
      lodsd
      cmp   eax, ebx
      jz    short Fatal_Error_Found
      or    eax, eax
      jz    short Fatal_Error_Found
      add   esi, 4
      jmp   short Fatal_Error_find

Fatal_Error_Found:
      lodsd
      mov   esi, eax
      mov   edi, eax
      cmp   word ptr ds:[Sel_Consoles], 0
      jz    short Fatal_Error_old_write
     push   ds
     pop    es
     push   edi
      mov   edi, offset New_Line_Txt
      call  kPrint
     pop    edi
      call  kPrint
     pop    dword ptr ds:[Error_Addr]
      mov   edi, offset Error_Addr_Txt
      call  kPrint
      jmp   short @Shut_Down_PrimaryMaster
Fatal_Error_old_write:
      xor   edi, edi
      call  Write_PChar
      mov   edi, 160
      mov   esi, offset Error_Addr_Old_Txt
      call  Write_PChar
     pop    eax
      call  hex2str_dd
      call  Write_text

      ;Vypneme harddisky...
@Shut_Down_PrimaryMaster :
      ;Mov   ebx,24h
      ;call  Ata_Gate
@Shut_Down_Stop_Machine :
      Call  waitKey
      Call  Restart
Fatal_Error Endp

