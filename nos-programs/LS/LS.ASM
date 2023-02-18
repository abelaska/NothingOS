.486P

Code Segment Public Use32 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;
CodeStart:
EBF_Signature           DB  'EBF',0            ;signatura
EBF_Header_Size         DD  offset HeadEnd     ;velikost hlavicky
EBF_Name                DB  'ls                              ' ;vnitrni jmeno souboru - 32 znaku
EBF_Data_Size           DD  Data_Segment_Size  ;velikost data segmentu ve 4k (zarovnani nahoru)
EBF_Code_Header_Size    DD  Code_Segment_Size  ;velikost hlavicky a code segmentu ve 4k (zarovnani nahoru)
EBF_Code_Entry_Point    DD  Offset Enter_ptr   ;vstupni bod programu
EBF_Flags               DD  1                  ;flagy
EBF_Stack_Size          DD  16                 ;velikost zasobniku
EBF_LibsNum             DD  3                  ;pocet knihoven
;knihovny
DB           'memory          '                ;Jmeno knihovny
DW           00001h                            ;Minimalni verze knihovny
DD           offset (MEM_Call_Gate+4)          ;Offset v DataSegmentu, kam se ulozi Call Gate
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
   Mov    EAX,00000014h  ;# 00000014 - TASK_Get_VFS_STDError_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_Error],EAX
   Mov    EAX,00000022h ;Get my current dir
   Call   Fword ptr DS:[TSK_Call_Gate]
   ;EAX = VFS inode aktualniho adresare


        mov    esi,offset test_path3
        mov    edx, 3
        mov    eax, VFS_Open
        call   fword ptr FS:[VFS_Call_Gate]

mov ebx,4
mov edx,eax
int 0b1h
        mov    esi,offset test_path1
        mov    edx, 3
        mov    eax, VFS_Open
        call   fword ptr FS:[VFS_Call_Gate]

mov ebx,4
mov edx,eax
int 0b1h
        mov    esi,offset test_path2
        mov    edx, 3
        mov    eax, VFS_Open
        call   fword ptr FS:[VFS_Call_Gate]

mov ebx,4
mov edx,eax
int 0b1h
        mov    esi,offset test_path5
        mov    edx, 3
        mov    eax, VFS_Open
        call   fword ptr FS:[VFS_Call_Gate]

mov ebx,4
mov edx,eax
int 0b1h
        mov    esi,offset test_path4
        mov    edx, 3
        mov    eax, VFS_Open
        call   fword ptr FS:[VFS_Call_Gate]

mov ebx,4
mov edx,eax
int 0b1h

       jmp   LS_Exit


   mov    eax, TASK_Get_My_Parameters_Length
   Call   Fword ptr DS:[TSK_Call_Gate]
   or     eax, eax
   jz     show_help

   mov    dword ptr ds:[param_size], eax

   Mov    EDX, EAX
   mov    eax, TSKMM_Alloc_B
   Call   Fword ptr DS:[Mem_Call_Gate]
   jc     err_nomem
   mov    es, edx
   xor    edi, edi
   mov    word ptr FS:[command_line_sel], dx

   mov    eax, TASK_Get_My_Parameters
   Call   Fword ptr DS:[TSK_Call_Gate]
   ; ES:EDI - parametry

   mov    ecx, dword ptr ds:[param_size]
   mov    al, ' '
   xor    ebx, ebx
   xor    edi, edi
   xor    edx, edx
get_parm_count:
  push   edi
   mov    edx, edi
   repnz  scasb
   sub    edx, edi
   neg    edx
   cmp    byte ptr es:[edi], ' '
   jz     short get_parm_count_0
   dec    edx
   inc    ebx
get_parm_count_0:
   or     ecx, ecx
   jnz    get_parm_count_1
   inc    edx
   jmp    short get_parm_count_2
get_parm_count_1:
   dec    edi
   mov    byte ptr es:[edi], 0
   inc    edi
get_parm_count_2:
  push    edx
   or     ecx, ecx
   jnz    short get_parm_count
   ; EBX - pocet parametru

   cmp    ebx, 1
   jnz    show_help

        pop    eax ; nepotrebne
        pop    esi ;
     push    es
     pop     ds
        xor    esi, esi
        mov    eax, VFS_Dir
        call   fword ptr FS:[VFS_Call_Gate]
        jc     badpath

        mov    esi, dword ptr fs:[param_size]
        dec    esi
        cmp    byte ptr ds:[esi], '/'
        jz     Wr_Dir_loop_minus_1
        inc    esi
        mov    byte ptr ds:[esi], '/'
Wr_Dir_loop_minus_1:
        inc    esi

Wr_Dir_loop:
        mov    eax, dword ptr es:[edi]
        or     eax, eax
        jz     Wr_Dir_end
        add    edi, 4
        call   write_pchar
       push    es edi
       push    gs
       pop     es
        mov    edi, offset space_txt
        call   write_pchar
       pop     edi es

       push    esi
       push    edi
Wr_Dir_loop_0:
        mov    al, byte ptr es:[edi]
        mov    byte ptr ds:[esi], al
        or     al, al
        jz     short Wr_Dir_loop_1
        inc    edi
        inc    esi
        jmp    short Wr_Dir_loop_0
Wr_Dir_loop_1:
       pop     edi

       push    es edi
       push    ds
       pop     es
        mov    edi, 0
        call   write_pchar
       push    gs
       pop     es
        mov    edi, offset New_Line_txt
        call   write_pchar
       pop     edi es

        xor    esi, esi
        mov    edx, 3
        mov    eax, VFS_Open
        call   fword ptr FS:[VFS_Call_Gate]
;        jc     Wr_Dir_posun_before
       push    eax
        mov    edx, eax
        mov    eax, VFS_Get_Inode_mode
        call   fword ptr FS:[VFS_Call_Gate]
       push    eax
       pop     ebx
       pop     edx
;        jc     Wr_Dir_posun_before
       push    ebx
        mov    eax, VFS_Close
        call   fword ptr FS:[VFS_Call_Gate]
       pop     ebx

Wr_Dir_posun_before:
       pop     esi
Wr_Dir_posun:
        cmp    byte ptr es:[edi], 0
        jz     short Wr_Dir_next
        inc    edi
        jmp    short Wr_Dir_posun
Wr_Dir_next:
        inc    edi
        jmp    Wr_Dir_loop
Wr_Dir_end:
        jmp    LS_Exit

badpath:
       push    fs
       pop     es
        mov    edi, offset bad_path
        call   Write_Pchar
        jmp    LS_Exit
err_nomem:
       push    fs
       pop     es
        mov    edi, offset no_memory
        call   Write_Pchar
        jmp    LS_Exit
show_help:
       push    fs
       pop     es
        mov    edi, offset ls_intro
        call   Write_Pchar
LS_Exit :
   Mov    EAX,1
   Call   Fword ptr FS:[TSK_Call_Gate]
;####################################################################################################
;# Vstup : ES : EDI - retezec pchar na vypis
;#         ES : Data Segment
;####################################################################################################
Write_Pchar PROC
       PUSHAD
       PUSH   ds ES
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
        Mov    EDX, Dword ptr GS:[VFS_Standard_Out]
        Xor    EBX, EBX
        Xor    EAX, EAX
        Call   fword ptr GS:[VFS_Call_Gate]
       POP   ES ds
       POPAD
       Ret
Write_Pchar ENDP
;####################################################################################
;# vypise na obrazovku cislo EAX (dec)
;####################################################################################
Write_Dec PROC
       PUSHAD
       PUSH  ds ES
          Mov   EBX,DS
          Mov   ES,EBX
          Mov   EBX, 10
          Mov   EDI,Offset PREVOD_Output
Pocitej_Dec:
         PUSH EAX
          dec   ebx
          xor   edx, edx
          div   dword ptr GS:[DEC_Numbers+EBX*4]
          mov   esi, offset Numbers_Tab
          add   esi, eax
          xchg  eax, ecx
          lodsb
          stosb
          xchg  eax, ecx
          xor   edx, edx
          mul   dword ptr GS:[DEC_Numbers+EBX*4]
          inc   ebx
          mov   ecx, eax
         POP EAX
          sub eax, ecx
          dec ebx
          jnz Pocitej_Dec

          Mov    EDI,Offset PREVOD_Output
          Mov    Byte ptr GS:[EDI+10],0
          Mov    ECX,9
Write_Dec_Look_For_End:
         Cmp    Byte ptr GS:[EDI],'0'
         Jne    Write_Dec_Look_For_End_Found
         Inc    EDI
         Dec    ECX
         Jnz    Write_Dec_Look_For_End
Write_Dec_Look_For_End_Found:
         Call   Write_PCHAR
       POP  ES ds
       POPAD
       Ret
Write_Dec ENDP
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


