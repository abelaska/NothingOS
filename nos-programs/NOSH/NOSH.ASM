.486P

Code Segment Public Use32 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;
CodeStart:
EBF_Signature           DB  'EBF',0            ;signatura
EBF_Header_Size         DD  offset HeadEnd     ;velikost hlavicky
EBF_Name                DB  'nosh                            ' ;vnitrni jmeno souboru - 32 znaku
EBF_Data_Size           DD  Data_Segment_Size  ;velikost data segmentu ve 4k (zarovnani nahoru)
EBF_Code_Header_Size    DD  Code_Segment_Size  ;velikost hlavicky a code segmentu ve 4k (zarovnani nahoru)
EBF_Code_Entry_Point    DD  Offset Enter_ptr   ;vstupni bod programu
EBF_Flags               DD  1                  ;flagy
EBF_Stack_Size          DD  16                 ;velikost zasobniku
EBF_LibsNum             DD  3                  ;pocet knihoven
;knihovny
DB           'memory          '                ;Jmeno knihovny
DW           00001h                            ;Minimalni verze knihovny
DD           offset (Mem_Call_Gate+4)          ;Offset v DataSegmentu, kam se ulozi Call Gate
DB           'taskman         '                ;Jmeno knihovny
DW           00001h                            ;Minimalni verze knihovny
DD           offset (TSK_Call_Gate+4)          ;Offset v DataSegmentu, kam se ulozi Call Gate
DB           'VFS             '                ;Jmeno knihovny
DW           00001h                            ;Minimalni verze knihovny
DD           offset (VFS_Call_Gate+4)          ;Offset v DataSegmentu, kam se ulozi Call Gate
HeadEnd :
EBF_CRC                 DD 0                   ;CRC hlavicky
;####################################################################################
;# Vstupni bod NOSHe
;####################################################################################
Enter_ptr :
;Zjistime VFS indody standardniho vstupu, vystupu a erroru
   Mov    EAX,00000002h  ;# 00000002 - TASK_Get_Current_Task_Num
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    EAX,00000012h  ;# 00000012 - TASK_Get_VFS_STDOut_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_Out],EAX
   Mov    EAX,00000013h  ;# 00000013 - TASK_Get_VFS_STDin_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_In],EAX
   Mov    EAX,00000014h  ;# 00000014 - TASK_Get_VFS_STDError_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_Error],EAX
;Alokujeme si buffer pro ukladani prikazove radky
   Xor    EAX,EAX
   Mov    EDX,4096
   Call   Fword ptr DS:[Mem_Call_Gate]
   Jc     @Error_Allocating
   Mov    Word ptr ds:[Read_Keyboard_sel], DX
;Alokujeme si buffer pro ukladani prikazove radky
   Xor    EAX,EAX
   Mov    EDX,4096
   Call   Fword ptr DS:[Mem_Call_Gate]
   Jc     @Error_Allocating
   mov    word ptr ds:[Command_Line_Sel], DX
   mov    ES, edx
   ; ES:EDI - ukazatel na buffer, do ktereho se ukladaji stiskle klavesy
   ;        -> prikazova radka
   ; nastavit bit ToScreen ve flagoch v klavesnici na 0
;############################################################################
  PUSH    ES
   Mov    EDI,DS
   Mov    ES,EDI
   Mov    EDI,Offset First_run_wellcome
   Call  Write_Pchar
  POP     ES  
Show_welcome_text:
;Fine, vypiseme uvitaci hlasku
  PUSH    ES
   Mov    EDI,DS
   Mov    ES,EDI
   Mov    EDI,Offset Welcome_txt
   Call   Write_Pchar
  POP     ES
Main_Loop:
   mov    edx, es
   lsl    edx, edx
   xor    edi, edi
   sub    edx,2
Main_Loop_read_key:
   call   ReadStdIn
   or     ecx, ecx
   jz     short Main_Loop_read_key
   mov    fs, word ptr ds:[Read_Keyboard_sel]
   xor    ebx, ebx
Decode_std_input:
   mov    al, byte ptr fs:[ebx]
   Or     Al,Al   ; extedned znaky
   Jnz    decode_std_input_test_exteded
   Mov    byte ptr ds:[Extended_char],1
   Jmp    Main_Loop_read_key
Decode_std_input_test_exteded :
   Cmp    Byte ptr ds:[Extended_char],0
   Je     decode_std_input_test_enter
   Mov    Byte ptr ds:[Extended_char],0
   Jmp    Main_Loop_read_key
Decode_std_input_test_enter :
   cmp    al, 0ah ; enter
   jne    short decode_std_input_test_backspace
   call   Save_Print_key
   dec    edi
   jmp    short Exec_prog
decode_std_input_test_backspace :
   cmp    al, 8 ; backspace
   jne    short decode_std_input_1
    or     edi, edi
    jz     short decode_std_input_2
    call   Save_Print_key
    dec    edi
    dec    edi
    inc    edx
    jmp    short decode_std_input_2
decode_std_input_1:
    dec    edx
    jnz    short decode_std_input_3
    inc    edx
    jmp    short decode_std_input_2
decode_std_input_3 :
    call   Save_Print_key
decode_std_input_2:
   inc    ebx
   dec    ecx
   jnz    short decode_std_input
   jmp    short Main_Loop_read_key
;########################################################################
;# Pokusime se spustit program
;########################################################################
Exec_prog:
   Or     EDI,EDI
   Jz     Show_Welcome_Text
   Xor    Al,Al
   Stosb
   Stosb
   Mov    ECX,EDI
   Xor    EDI,EDI
   Mov    Al,' '
   Repnz  Scasb
   jnz    @Exec_No_Parameters
   Dec    EDI
   Xor    Al,Al
   Stosb
@Exec_No_Parameters :
   Xor    ESI,ESI
   Mov    EBX, Dword ptr DS:[VFS_Standard_In]
   Mov    ECX,Dword ptr DS:[VFS_Standard_Out]
   Mov    EDI,Dword ptr DS:[VFS_Standard_Error]
   Xor    EDX,EDX
   Push   DS ES
    Mov    EAX,ES
    Push   DS
    Pop    ES
    Mov    DS,EAX
    Xor    EAX,EAX
    Call   Fword ptr ES:[TSK_Call_Gate]
    Or     EAX,EAX
    Jz     @File_Executing_OK
    Cmp    EAX,1
    Je     Exec_Error_Memory
    Cmp    EAX,2
    Je     Exec_Error_Not_Supported
    Cmp    EAX,3
    Je     Exec_Error_File_Corrupted
    Cmp    EAX,4
    Je     Exec_Error_Memory
    Cmp    EAX,5
    Je     Exec_Error_File_Not_Found
    Ror    EAX,16
    Cmp    Ax,1
    Je     Exec_Error_Lib_No_Exist
    cmp    Ax,3
    Je     Exec_Error_Lib_No_Ver
    Cmp    Ax,7
    Je     Exec_Error_File_Corrupted
    Jmp    Exec_Unknown_error
@File_Executing_OK :
   Pop     ES DS
    Jmp    Show_Welcome_Text
;#############################################################################
;# Jednotka nema dostatecnou verzi
;#############################################################################
Exec_Error_Lib_No_Ver :
     Call   Get_Library
     Call   Get_Lib_Version
     Mov    EDI, Offset Error_Exec_Error_Lib_No_Exist
     Call   Write_Pchar
     Mov    EDI, Offset LIBS_Output
     Call   Write_Pchar
     Mov    EDI, Offset Error_Exec_Error_Lib_Ver
     Call   Write_Pchar
     Mov    EDI, Dword ptr ES:[LIBS_Ver]
     Call   Write_Pchar
     Mov    EDI, Offset Error_Exec_Error_Lib_Ver_2
     Call   Write_Pchar
     Jmp    @File_Executing_OK
;#############################################################################
;# Jednotka neexistuje
;#############################################################################
Exec_Error_Lib_No_Exist :
     Call   Get_Library
     Mov    EDI, Offset Error_Exec_Error_Lib_No_Exist
     Call   Write_Pchar
     Mov    EDI, Offset LIBS_Output
     Call   Write_Pchar
     Mov    EDI, Offset Error_Exec_Error_Lib_No_Exist2
     Call   Write_Pchar
     Jmp    @File_Executing_OK
;#############################################################################
;# Format souboru neni podporovan
;#############################################################################
Exec_Error_Not_Supported :
     Mov    EDI, Offset Error_Exec_Error_Not_Supported
     Call   Write_Pchar
     Jmp    @File_Executing_OK
;#############################################################################
;# Format souboru je poskozen
;#############################################################################
Exec_Error_File_Corrupted :
     Mov    EDI, Offset Error_Exec_Error_File_Corrupted
     Call   Write_Pchar
     Jmp    @File_Executing_OK
;#############################################################################
;# Soubor nenalezen
;#############################################################################
Exec_Error_File_Not_Found :
     Mov    EDI, Offset Error_Exec_Error_File_Not_Found
     Call   Write_Pchar
     Jmp    @File_Executing_OK
;#############################################################################
;# Nedostatek pameti pro spusteni
;#############################################################################
Exec_Error_Memory :
     Mov    EDI, Offset Error_Exec_Error_Memory
     Call   Write_Pchar
     Jmp    @File_Executing_OK
;#############################################################################
;# Neznama chyba
;#############################################################################
Exec_Unknown_error :
     Mov    EDI, Offset Error_Exec_Unknown_error
     Call   Write_Pchar
     Jmp    @File_Executing_OK
;#############################################################################
;# Error pri inicializaci tasku, zabijeme se
;#############################################################################
@Error_Allocating :
   PUSH   DS
   POP    ES
   Mov    EDI,Offset Error_Allocation
   Call   fword ptr DS:[VFS_Call_Gate]
   Mov    EAX,1
   Call   Fword ptr DS:[TSK_Call_Gate]
   Jmp    $
;####################################################################################################
;# Procedura pro ulozeni klavesy a zobrazeni na obrazovku a
;####################################################################################################
;# Vstup  : AL - klavesa
;# Vystup : -
;####################################################################################################
Save_Print_key Proc
  push    eax ebx ecx edx
   stosb
   Xor    Ah,Ah
   cmp    al,'\'
   jne    Short Save_Print_key_cont
   mov    ah,1
Save_Print_key_cont :
  push    edi eax
   dec    edi
   Mov    EDX, Dword ptr DS:[VFS_Standard_Out]
   Xor    EBX, EBX
   Mov    ECX, 1
   Xor    EAX, EAX
   Call   fword ptr DS:[VFS_Call_Gate]
  pop     eax edi
   or     ah,ah
   Jz     short Save_Print_Key_Cont_2
   Xor    Ah,Ah
   Jmp    Short Save_Print_key_cont
Save_Print_Key_Cont_2 :
  Pop     edx ecx ebx eax
   ret
Save_Print_key Endp
;####################################################################################################
;# precte stisklou klavesu
;####################################################################################################
;# Vstup:  -
;# Vystup: ECX - pocet prectenych znaku
;####################################################################################################
ReadStdIn Proc
      push   eax edi edx es
       mov   edx,Dword ptr DS:[VFS_Standard_In]
       mov   eax,1
       mov   ecx,1
       mov   es, word ptr ds:[Read_Keyboard_sel]
       xor   edi, edi
      push   ds
       call  fword ptr ds:[VFS_Call_Gate]
       Jc    $
      pop    ds
      pop    es edx edi eax
       ret
ReadStdIn Endp
;####################################################################################################
;# Write_PChar
;####################################################################################################
;# Vstup : ES : EDI - retezec pchar na vypis
;#         ES : Data Segment
;####################################################################################################
Write_Pchar PROC
       PUSHAD
       PUSH   EDI
        Mov    ECX, ES
        LSL    ECX,ECX
        Sub    ECX,EDI
        PUSH   ECX
         Mov    Al,0
         Repnz  Scasb
        POP    EDX
        Sub    EDX, ECX
        Mov    ECX, EDX
        Dec    ECX
       POP    EDI
        Mov    EDX, Dword ptr ES:[VFS_Standard_Out]
        Xor    EBX, EBX
        Xor    EAX, EAX
        Call   fword ptr ES:[VFS_Call_Gate]
       POPAD
       Ret
Write_Pchar ENDP
;####################################################################################
;# Get Library Name
;####################################################################################
Get_Library PROC
       PUSHAD
         Ror    EAX,16
         Mov    ECX,EAX
         Mov    EBP,EBX
         Mov    EAX,01Ch
         Mov    EDI,Offset LIBS_Output
         Call   Fword ptr ES:[TSK_Call_Gate]
         Mov    EDI,Offset LIBS_Output
         Mov    ECX,15
Get_Library_Look_For_End:
         Cmp    Byte ptr ES:[EDI+ECX],' '
         Jne    Get_Library_Look_For_End_Found
         Dec    ECX
         Jnz    Get_Library_Look_For_End
Get_Library_Look_For_End_Found:
         Inc    ECX
         Mov    Byte ptr ES:[EDI+ECX],0
       POPAD
       Ret
Get_Library ENDP
;####################################################################################
;# Write Version
;####################################################################################
Get_Lib_Version PROC
       PUSHAD
       PUSH     DS
         Ror    EAX,16
         Mov    ECX,EAX
         Mov    EBP,EBX
         Mov    EAX,01Dh
         Call   Fword ptr ES:[TSK_Call_Gate]
         Mov    EDI, offset LIBS_Output_Ver
         Push   ES
         Pop    DS
          Mov   EBX, 10
          Xor   edx, edx
Pocitej_Dec:
         PUSH EAX
          xor   edx, edx
          div   dword ptr DS:[DEC_Numbers+EBX*4]
          mov   esi, offset Numbers_Tab
          add   esi, eax
          xchg  eax, ecx
          lodsb
          stosb
          xchg  eax, ecx
          xor   edx, edx
          mul   dword ptr DS:[DEC_Numbers+EBX*4]
          inc   ebx
          mov   ecx, eax
         POP EAX
          sub eax, ecx
          dec ebx
          jnz Pocitej_Dec
          Mov    EDI,Offset LIBS_Output_Ver
          Mov    Byte ptr ES:[EDI+10],0
          Mov    ECX,10
Get_Version_Look_For_End:
         Cmp    Byte ptr ES:[EDI],'0'
         Jne    Get_Version_Look_For_End_Found
         Inc    EDI
         Dec    ECX
         Jnz    Get_Version_Look_For_End
Get_Version_Look_For_End_Found:
         Mov    Dword ptr ES:[LIBS_Ver],EDI
       POP DS
       POPAD
       Ret
Get_Lib_Version ENDP
;####################################################################################
ALIGN 4096
CSize             equ ($ - CodeStart)
Code_Segment_Size equ (CSize Shr 12)
CodeEnd:
Code Ends
;####################################################################################
;# Data segment
;####################################################################################
Include ./Data.inc
End CodeStart


